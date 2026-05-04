// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <chrono>
#include <memory>
#include <numeric>
#include <vector>

#include "padcollector.h"

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/definitions.h"

#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

static uint32_t const NB_LOOKBACK_HOURS = 48;

void PADCollector::perform(XpertRequestResult& _xpertRequestResult)
{
    // Check if there is a drug model.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model set.");
        return;
    }

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

    if (_xpertRequestResult.getTreatment()->getDosageHistory().isEmpty()) {
        // If no dosage history there is no way to compute the pre-adjustments PK parameters
        return;
    }

    Common::DateTime endDate = _xpertRequestResult.getXpertRequest().getAdjustmentTime();
    Common::DateTime startDate = endDate - chrono::hours(NB_LOOKBACK_HOURS);

    // Compute pre-adjustment apriori data.
    std::string responseId;
    Core::PercentileRanks ranks(99);
    iota(ranks.begin(), ranks.end(), 1);
    double nbPointsPerHour = 20;

    Core::ComputingOption computingOption{
            Core::PredictionParameterType::Apriori, Core::CompartmentsOption::AllActiveMoieties};

    unique_ptr<Core::ComputingTraitPercentiles> percentileTrait = make_unique<Core::ComputingTraitPercentiles>(
            responseId, startDate, endDate, ranks, nbPointsPerHour, computingOption);

    unique_ptr<Core::PercentilesData> percentilesResult = nullptr;

    executeRequestAndGetResult<Core::ComputingTraitPercentiles, Core::PercentilesData>(
            std::move(percentileTrait), _xpertRequestResult, percentilesResult);

    if (percentilesResult == nullptr) {
        _xpertRequestResult.setErrorMessage("Percentiles computation failed.");
        return;
    }

    _xpertRequestResult.setAPrioriPercentiles(std::move(percentilesResult));

    // Compute pre-adjustment aposteriori data.
    // We rely on adjustment traits to pre-fill the request.
    unique_ptr<Core::ComputingTraitAdjustment> baseAdjustmentTrait =
            make_unique<Core::ComputingTraitAdjustment>(*_xpertRequestResult.getAdjustmentTrait());

    Core::ComputingOption options{
            Core::PredictionParameterType::Aposteriori,
            baseAdjustmentTrait->getComputingOption().getCompartmentsOption(),
            baseAdjustmentTrait->getComputingOption().retrieveStatistics(),
            baseAdjustmentTrait->getComputingOption().retrieveParameters(),
            baseAdjustmentTrait->getComputingOption().retrieveCovariates()};

    unique_ptr<Core::ComputingTraitConcentration> aposterioriConcentrationTrait =
            make_unique<Core::ComputingTraitConcentration>("", startDate, endDate, nbPointsPerHour, options);

    unique_ptr<Core::SinglePredictionData> aposterioriConcentrationResult = nullptr;
    executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
            std::move(aposterioriConcentrationTrait), _xpertRequestResult, aposterioriConcentrationResult);

    // If execution failed.
    if (aposterioriConcentrationResult == nullptr) {
        _xpertRequestResult.setErrorMessage("Failed to extract aposteriori parameters.");
        return;
    }

    _xpertRequestResult.setAPosterioriConcentrations(std::move(aposterioriConcentrationResult));
}

} // namespace Xpert
} // namespace Tucuxi
