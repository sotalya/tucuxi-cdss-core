// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <chrono>
#include <memory>
#include <numeric>
#include <vector>

#include "samplevalidator.h"

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/definitions.h"

#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void SampleValidator::perform(XpertRequestResult& _xpertRequestResult)
{
    // Check if there is a treatment.
    if (_xpertRequestResult.getTreatment() == nullptr) {
        _xpertRequestResult.setErrorMessage("No treatment set.");
        return;
    }

    // If there are samples, check if there is a dosage history.
    if (!_xpertRequestResult.getTreatment()->getSamples().empty()
        && _xpertRequestResult.getTreatment()->getDosageHistory().isEmpty()) {
        _xpertRequestResult.setErrorMessage("Samples found but dosage history is empty.");
        return;
    }

    // Check if there is a drug model.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model set.");
        return;
    }

    std::vector<SampleValidationResult> results;

    const std::vector<std::unique_ptr<Core::Sample>>& samples = _xpertRequestResult.getTreatment()->getSamples();

    // To order samples, to ensure a smooth computing then.
    std::vector<Core::Sample*> samplesCopy(samples.size());
    std::transform(
            samples.begin(), samples.end(), samplesCopy.begin(), [](const auto& _sample) { return _sample.get(); });

    auto compareByDate = [](Core::Sample* _a, Core::Sample* _b) { return _a->getDate() < _b->getDate(); };

    std::sort(samplesCopy.begin(), samplesCopy.end(), compareByDate);

    // End ordering

    std::vector<std::vector<const Core::Sample*>> groupedSamples = groupSamplesByTime(samplesCopy);

    for (const auto& group : groupedSamples) {
        if (group.empty()) {
            continue;
        }

        std::string responseId;
        Common::DateTime minDate = group.front()->getDate() - chrono::hours(1);
        Common::DateTime maxDate = group.back()->getDate() + chrono::hours(1);

        Core::PercentileRanks ranks(99);
        iota(ranks.begin(), ranks.end(), 1);
        double nbPointsPerHour = 20;

        Core::ComputingOption computingOption{
                Core::PredictionParameterType::Apriori, Core::CompartmentsOption::AllActiveMoieties};

        unique_ptr<Core::ComputingTraitPercentiles> percentileTrait = make_unique<Core::ComputingTraitPercentiles>(
                responseId, minDate, maxDate, ranks, nbPointsPerHour, computingOption);

        unique_ptr<Core::PercentilesData> percentilesResult = nullptr;

        executeRequestAndGetResult<Core::ComputingTraitPercentiles, Core::PercentilesData>(
                std::move(percentileTrait), _xpertRequestResult, percentilesResult);

        if (percentilesResult == nullptr) {
            _xpertRequestResult.setErrorMessage("Percentiles computation failed.");
            return;
        }


        for (const Core::Sample* sample : group) {
            unsigned groupOver99Percentiles = 0;
            bool isAscending = true;

            try {
                groupOver99Percentiles =
                        findGroupPositionOver99Percentiles(percentilesResult.get(), *sample, isAscending);
            }
            catch (const invalid_argument& e) {
                _xpertRequestResult.setErrorMessage(
                        "Error handling sample of " + dateTimeToString(sample->getDate()) + ", details: " + e.what());
                return;
            }

            results.emplace_back(
                    SampleValidationResult(sample, groupOver99Percentiles, WarningLevel::NORMAL, isAscending));
        }
    }
    _xpertRequestResult.setSampleResults(std::move(results));
}

unsigned SampleValidator::findGroupPositionOver99Percentiles(
        const Core::PercentilesData* _percentilesData, const Core::Sample& _sample, bool& _isAscending) const
{
    // First, find the cycleData that holds the values that surround the sample date
    // We simply use the first percentile to do that.
    _isAscending = true;

    int cycleDataIndex = -1;

    const vector<Core::CycleData>& firstPercentileData = _percentilesData->getPercentileData(0);
    for (size_t cycleIndex = 0; cycleIndex < firstPercentileData.size(); ++cycleIndex) {

        // If the sample date is between the cycleData start and end date.
        if (_sample.getDate() >= firstPercentileData[cycleIndex].m_start
            && _sample.getDate() <= firstPercentileData[cycleIndex].m_end) {
            cycleDataIndex = static_cast<int>(cycleIndex);
            break;
        }
    }

    // If we didn't find a cycleData.
    if (cycleDataIndex == -1) {
        throw invalid_argument("No cycle data contains the sample date.");
    }


    // Iterate over the percentiles (1-99) to find the first percentile that contains
    // a bigger value than the corresponding sample.

    // This variable saves the position of the time offsets to not being forced
    // to parse everything again when switching percentile.
    size_t savedIndexOfPair = 0;
    double lastSlope = 0.0;

    for (size_t percentileIndex = 0; percentileIndex < 99; ++percentileIndex) {

        // Get the percentile.
        const vector<Core::CycleData>& percentileData = _percentilesData->getPercentileData(percentileIndex);

        // Get its times and concentrations.
        const vector<double>& times = percentileData[cycleDataIndex].m_times[0];
        const auto& concentrations = percentileData[cycleDataIndex].m_concentrations[0];

        // Iterates through pairs of time offsets and look for the pair that bounds the sample date.
        for (size_t i = savedIndexOfPair; i < times.size() - 1; ++i) {

            // Get the bounded dates and times.
            Common::DateTime t0 =
                    percentileData[cycleDataIndex].m_start + chrono::minutes(static_cast<int>(times[i] * 60));
            Common::DateTime t1 =
                    percentileData[cycleDataIndex].m_start + chrono::minutes(static_cast<int>(times[i + 1] * 60));

            // If the sample date time is not bounded, jump to the next pair.
            if (!(t0 <= _sample.getDate() && _sample.getDate() <= t1)) {
                continue;
            }

            lastSlope = concentrations[i + 1] - concentrations[i];

            // Convert the sample value to the cycleData unit.
            double convertedSampleConcentration = Common::UnitManager::convertToUnit(
                    _sample.getValue(), _sample.getUnit(), percentileData[cycleDataIndex].m_unit);

            // If the measure is lower than the interpolated mesure, it belongs to the current percentile
            // (+1 because we iterate from 0 to 98)
            // The interpolation proceeds  as follows (assuming the concentration interpolation is linear):
            // 1) Compute rp, the "relative" position of the sample date between the pair. 0 being above t0 and 1 being above t1.
            // 2) Compute the interpolated concentration with respect to rp. concentration(t0) + rp * ( concentration(t1) - concentration(t0) )

            // 1) Relative position rp using date as seconds. Assuming t0 <= sample date <= t1,
            //    the interpolation rate is (sample date - t0) / (t1 - t0)
            double rp = (_sample.getDate().toSeconds() - t0.toSeconds()) / (t1.toSeconds() - t0.toSeconds());

            // 2) Retrieving the interpolated concentration
            double interpolatedConcentration = concentrations[i] + rp * (concentrations[i + 1] - concentrations[i]);

            // If the sample concentration is below the percentile concentration, it belongs to its group.
            if (convertedSampleConcentration <= interpolatedConcentration) {
                _isAscending = (lastSlope >= 0.0);
                return percentileIndex + 1;
            }
            // Jump to the next percentile.
            savedIndexOfPair = i; // Next percentile goes directly to the the good times pair.
            break;                // Try the next percentile.
        }
    }

    _isAscending = (lastSlope >= 0.0);
    // If the mesure is not in the groups before the 99th percentile, it belongs
    // to the last group, the 100th.
    return 100;
}

std::vector<std::vector<const Core::Sample*>> SampleValidator::groupSamplesByTime(
        const std::vector<Core::Sample*>& _samples)
{
    std::vector<std::vector<const Core::Sample*>> groupedSamples;

    groupedSamples.emplace_back();

    // TODO : Should we just return an empty vector? Would seem reasonable.
    if (_samples.empty()) {
        // No samples have been collected, early return.
        return groupedSamples;
    }

    groupedSamples.back().push_back(_samples[0]);
    if (_samples.size() < 2) {
        return groupedSamples;
    }

    for (size_t i = 1; i < _samples.size(); ++i) {
        if (_samples.at(i)->getDate() - _samples.at(i - 1)->getDate() > chrono::hours(2)) {
            groupedSamples.emplace_back();
        }

        groupedSamples.back().push_back(_samples.at(i));
    }

    return groupedSamples;
} // LCOV_EXCL_LINE

} // namespace Xpert
} // namespace Tucuxi
