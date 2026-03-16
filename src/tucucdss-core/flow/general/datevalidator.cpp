// SPDX-License-Identifier: AGPL-3.0-or-later
#include <memory>

#include "datevalidator.h"

#include "tucucommon/loggerhelper.h"

#include "tucucdss-core/language/languagemanager.h"
#include "tucucdss-core/result/dosedatevalidationresult.h"
#include "tucucdss-core/result/sampledatevalidationresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

#define NUM_HALF_LIVES 5

void DateValidator::perform(XpertRequestResult& _xpertRequestResult)
{
    Common::LoggerHelper logHelper;

    // Check if there is a treatment, otherwise the date checks are meaningless.
    if (_xpertRequestResult.getTreatment() == nullptr) {
        return;
    }

    // If there are samples, check if there is a dosage history (otherwise checks
    // are meaningless).
    if (!_xpertRequestResult.getTreatment()->getSamples().empty()
        && _xpertRequestResult.getTreatment()->getDosageHistory().isEmpty()) {
        return;
    }

    // Check if there is a drug model, otherwise the date checks are meaningless.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        return;
    }

    // From the drug model retrieve the drug's half-life.
    Core::DrugModel const* const drugModel = _xpertRequestResult.getDrugModel();
    Core::TimeConsiderations const& tc = drugModel->getTimeConsiderations();
    Core::HalfLife const& hl = tc.getHalfLife();

    double halfLifeSeconds = 0.0;
    try {
        halfLifeSeconds = Common::UnitManager::convertToUnit(hl.getValue(), hl.getUnit(), Common::TucuUnit("s"));
    }
    catch (const std::invalid_argument&) {
        logHelper.warn(
                std::string("Unsupported half-life unit [") + hl.getUnit().toString()
                + std::string("], skipping dates validation"));
        return;
    }
    Common::Duration halfLife = Common::Duration(std::chrono::seconds(static_cast<long>(halfLifeSeconds + 0.5)));

    // Retrieve the dosage history.
    Core::DosageHistory const& dosageHistory = _xpertRequestResult.getTreatment()->getDosageHistory();

    std::vector<Common::DateTime> doseDates = extractDoseDates(dosageHistory);

    // Retrieve samples and sort them.
    std::vector<std::unique_ptr<Core::Sample>> const& samples = _xpertRequestResult.getTreatment()->getSamples();
    std::vector<Core::Sample*> samplesCopy(samples.size());
    std::transform(
            samples.begin(), samples.end(), samplesCopy.begin(), [](const auto& _sample) { return _sample.get(); });
    auto compareByDate = [](Core::Sample* _a, Core::Sample* _b) { return _a->getDate() < _b->getDate(); };
    std::sort(samplesCopy.begin(), samplesCopy.end(), compareByDate);

    std::vector<Common::DateTime> sampleDates(samplesCopy.size());
    std::transform(samplesCopy.begin(), samplesCopy.end(), sampleDates.begin(), [](Core::Sample* _sample) {
        return _sample->getDate();
    });

    // If no adjustment time is specified, skip the checks with a warning.
    if (_xpertRequestResult.getXpertRequest().getAdjustmentTime().isUndefined()) {
        logHelper.warn("No adjustment date found, skipping dates validation");
        return;
    }
    Common::DateTime adjustmentDate = _xpertRequestResult.getXpertRequest().getAdjustmentTime();

    // -------------------------------------------------------------------------
    // Perform a battery of tests targeted at validating the dates.
    // -------------------------------------------------------------------------
    std::vector<SampleDateValidationResult> sampleResults;
    // Sample date too far in the future after the last dose. This should be
    // caught by the sample validator, but the check there is slightly different
    // (no half-life check).
    if (!checkDoseToMeasureDistances(doseDates, sampleDates, halfLife * NUM_HALF_LIVES)) {
        _xpertRequestResult.setErrorMessage(LanguageManager::getInstance().translate("meas_too_far_doses"));
        return;
    }

    // The sample date is too far in the past with respect to the adjustment date
    // that it is not relevant anymore.
    if (!sampleDates.empty()) {
        auto it = std::upper_bound(sampleDates.begin(), sampleDates.end(), adjustmentDate);

        if (it == sampleDates.begin()) {
            _xpertRequestResult.setErrorMessage(LanguageManager::getInstance().translate("meas_after_adj"));
            return;
        }
        else {
            --it; // last date <= adjustmentDate
            if (*it + halfLife * NUM_HALF_LIVES < adjustmentDate) {
                std::string warning = LanguageManager::getInstance().translate("meas_too_far_adj");
                logHelper.warn(
                        std::string("Sample with date ") + it->str()
                        + std::string(" is too far from the adjustment date"));
                Core::Sample const* const samplePtr = findSampleByDate(samples, *it);
                sampleResults.push_back(SampleDateValidationResult(samplePtr, warning, WarningLevel::NORMAL));
            }
        }
    }

    _xpertRequestResult.setSampleDateResults(std::move(sampleResults));
}


Core::Sample* DateValidator::findSampleByDate(
        const std::vector<std::unique_ptr<Core::Sample>>& _samples, const Common::DateTime& _date)
{
    for (const auto& samplePtr : _samples) {
        if (samplePtr && samplePtr->getDate() == _date) {
            return samplePtr.get(); // Raw pointer, non-owning
        }
    }
    // This should never occur --- we have built the vector of dates from the
    // given sample vector.
    throw std::runtime_error("DateValidator: error in sample dates handling");
}


bool DateValidator::checkDoseToMeasureDistances(
        const std::vector<Common::DateTime>& _doseDates,
        const std::vector<Common::DateTime>& _measureDates,
        const Common::Duration& _threshold)
{
    std::vector<DateTime> sortedDoses = _doseDates;
    std::sort(sortedDoses.begin(), sortedDoses.end());

    for (const auto& measure : _measureDates) {
        // Find the first dose strictly greater than measure.
        auto it = std::upper_bound(sortedDoses.begin(), sortedDoses.end(), measure);

        if (it == sortedDoses.begin()) {
            // No dose before this measure => violation or special case.
            return false;
        }

        // Move back to the last dose <= measure.
        auto doseIt = std::prev(it);
        const auto& lastDose = *doseIt;

        Duration diff = measure - lastDose;

        if (diff > _threshold) {
            return false;
        }
    }

    return true;
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(Core::DosageHistory const& _dosageHistory) const
{
    std::vector<Common::DateTime> doseDates;

    for (const unique_ptr<Core::DosageTimeRange>& timeRange : _dosageHistory.getDosageTimeRanges()) {
        std::vector<Common::DateTime> dates = extractDoseDates(*timeRange);
        doseDates.insert(doseDates.end(), std::make_move_iterator(dates.begin()), std::make_move_iterator(dates.end()));
    }

    return doseDates;
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(Core::DosageTimeRange const& _timeRange) const
{
    return extractDoseDates(*_timeRange.getDosage(), _timeRange);
}


// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TRY_CHECK(Type)                                                                          \
    if (dynamic_cast<const Tucuxi::Core::Type*>(&_dosage)) {                                     \
        return extractDoseDates(*dynamic_cast<const Tucuxi::Core::Type*>(&_dosage), _timeRange); \
    }


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::Dosage& _dosage, Core::DosageTimeRange const& _timeRange) const
{
    // The calls order is important here.
    // First start with the subclasses, else it won't work
    TRY_CHECK(SingleDose);
    TRY_CHECK(SimpleDoseList);
    TRY_CHECK(SingleDoseAtTimeList);
    TRY_CHECK(ParallelDosageSequence);
    TRY_CHECK(DosageSequence);
    TRY_CHECK(DosageRepeat);
    TRY_CHECK(DosageLoop);

    // We should never get here!
    Common::LoggerHelper logHelper;
    logHelper.error("Incompatible dosage types in dates validation");
    return std::vector<Common::DateTime>();
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::DosageLoop& _dosageLoop, Core::DosageTimeRange const& _timeRange) const
{
    return extractDoseDates(*_dosageLoop.getDosage(), _timeRange);
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::DosageRepeat& _dosageRepeat, Core::DosageTimeRange const& _timeRange) const
{
    return extractDoseDates(*_dosageRepeat.getDosage(), _timeRange);
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::DosageSequence& _dosageSequence, Core::DosageTimeRange const& _timeRange) const
{
    return extractDoseDatesDosageBoundedList(_dosageSequence.getDosageList(), _timeRange);
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::ParallelDosageSequence& _parallelDosageSequence, Core::DosageTimeRange const& _timeRange) const
{
    return extractDoseDatesDosageBoundedList(_parallelDosageSequence.getDosageList(), _timeRange);
}


std::vector<Common::DateTime> DateValidator::extractDoseDatesDosageBoundedList(
        const Core::DosageBoundedList& _dosageBoundedList, Core::DosageTimeRange const& _timeRange) const
{
    std::vector<Common::DateTime> doseDates;

    for (const std::unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosageBoundedList) {
        std::vector<Common::DateTime> dates = extractDoseDates(*dosage, _timeRange);
        doseDates.insert(doseDates.end(), std::make_move_iterator(dates.begin()), std::make_move_iterator(dates.end()));
    }

    return doseDates;
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::SingleDoseAtTimeList& _singleDoseAtTimeList, Core::DosageTimeRange const& _timeRange) const
{
    std::vector<Common::DateTime> doseDates;
    auto dosageList = _singleDoseAtTimeList.getDosageList();

    for (auto const& dosage : dosageList) {
        doseDates.push_back(dosage.getDateTime());
    }

    return doseDates;
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::SimpleDoseList& _simpleDoseList, Core::DosageTimeRange const& _timeRange) const
{
    std::vector<Common::DateTime> doseDates;
    auto dosageList = _simpleDoseList.getDosageList();

    for (auto const& dosage : dosageList) {
        doseDates.push_back(dosage.getDateTime());
    }

    return doseDates;
}


std::vector<Common::DateTime> DateValidator::extractDoseDates(
        const Core::SingleDose& _singleDose, Core::DosageTimeRange const& _timeRange) const
{
    std::vector<Common::DateTime> doseDates;
    Common::DateTime currentDate = _singleDose.getFirstIntakeInterval(_timeRange.getStartDate());

    while (currentDate < _timeRange.getEndDate()) {
        doseDates.push_back(currentDate);
        currentDate += _singleDose.getTimeStep();
    }

    return doseDates;
}

} // namespace Xpert
} // namespace Tucuxi
