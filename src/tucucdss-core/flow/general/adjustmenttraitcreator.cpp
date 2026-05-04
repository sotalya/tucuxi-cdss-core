// SPDX-License-Identifier: AGPL-3.0-or-later
#include <chrono>

#include "adjustmenttraitcreator.h"

#include "tucucommon/loggerhelper.h"
#include "tucucommon/unit.h"

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/intakeevent.h"
#include "tucucore/intakeextractor.h"

#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"


using namespace std;

namespace Tucuxi {
namespace Xpert {

void AdjustmentTraitCreator::perform(XpertRequestResult& _xpertRequestResult)
{
    Common::LoggerHelper logHelper;

    // Fixing the computation time. Useful for computing the adjustment / start / end times...
    m_computationTime = _xpertRequestResult.getXpertQueryResult().getComputationTime();

    // Check if there is a treatment.
    if (_xpertRequestResult.getTreatment() == nullptr) {
        _xpertRequestResult.setErrorMessage("No treatment set.");
        return;
    }

    // Check if there is a drug model.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model set.");
        return;
    }

    // Get the corresponding formulation and route from the drug model.
    const Core::FormulationAndRoutes& modelFormulationsAndRoutes =
            _xpertRequestResult.getDrugModel()->getFormulationAndRoutes();
    const Core::FullFormulationAndRoute* fullFormulationAndRoute = nullptr;

    // If there is no treatment, get the default formulation and route.
    if (_xpertRequestResult.getTreatment()->getDosageHistory().isEmpty()) {
        fullFormulationAndRoute = modelFormulationsAndRoutes.getDefault();
    }
    else {
        // Else find the corresponding one.
        fullFormulationAndRoute = modelFormulationsAndRoutes.get(
                _xpertRequestResult.getTreatment()->getDosageHistory().getLastFormulationAndRoute());
    }

    // ------ Preparing the parameters for the adjustment trait creation ------

    // Response identifier.
    std::string responseId;

    // Points per hour.
    double nbPointsPerHour = 20;

    // Computing options.
    Core::ComputingOption computingOption{
            getPredictionParameterType(_xpertRequestResult.getTreatment()),
            Core::CompartmentsOption::AllActiveMoieties,
            Core::RetrieveStatisticsOption::RetrieveStatistics,
            Core::RetrieveParametersOption::RetrieveParameters,
            Core::RetrieveCovariatesOption::RetrieveCovariates,
            Core::ForceUgPerLiterOption::DoNotForce};

    // Adjustment date.
    Common::DateTime adjustmentTime = getAdjustmentTimeAndLastIntake(_xpertRequestResult, fullFormulationAndRoute);

    // Period.
    Common::DateTime start;
    Common::DateTime end;

    try {

        getPeriod(fullFormulationAndRoute, _xpertRequestResult, adjustmentTime, start, end);
    }
    catch (const invalid_argument& e) {
        // We catch the fact the the treatment may already be over (if it is a standard treatment).
        _xpertRequestResult.setErrorMessage(std::string(e.what()));
        return;
    }

    // Best candidate option.
    Core::BestCandidatesOption candidatesOption = Core::BestCandidatesOption::BestDosagePerInterval;

    // Loading option.
    Core::LoadingOption loadingOption =
            getLoadingOption(_xpertRequestResult.getXpertRequest(), fullFormulationAndRoute);

    // Rest period option.
    Core::RestPeriodOption restPeriodOption =
            getRestPeriodOption(_xpertRequestResult.getXpertRequest(), fullFormulationAndRoute);

    // Steady state target option.
    Core::SteadyStateTargetOption steadyStateTargetOption = getSteadyStateTargetOption(fullFormulationAndRoute);

    // Target extraction option.
    Core::TargetExtractionOption targetExtractionOption =
            _xpertRequestResult.getXpertRequest().getTargetExtractionOption();

    // Formulation and route selection option.
    Core::FormulationAndRouteSelectionOption formulationAndRouteSelectionOption =
            _xpertRequestResult.getXpertRequest().getFormulationAndRouteSelectionOption();

    // Creating and setting the trait.
    Core::ComputingTraitAdjustment computingTraitAdjustment = Core::ComputingTraitAdjustment(
            responseId,
            start,
            end,
            nbPointsPerHour,
            computingOption,
            adjustmentTime,
            candidatesOption,
            loadingOption,
            restPeriodOption,
            steadyStateTargetOption,
            targetExtractionOption,
            formulationAndRouteSelectionOption);

    _xpertRequestResult.setAdjustmentTrait(computingTraitAdjustment);
}

Core::PredictionParameterType AdjustmentTraitCreator::getPredictionParameterType(
        const Core::DrugTreatment* _drugTreatment) const
{
    // If there is doses and samples.
    if (!_drugTreatment->getDosageHistory().isEmpty() && !_drugTreatment->getSamples().empty()) {
        return Core::PredictionParameterType::Aposteriori;
    }
    return Core::PredictionParameterType::Apriori;
}

Common::DateTime AdjustmentTraitCreator::getAdjustmentTimeAndLastIntake(
        XpertRequestResult& _xpertRequestResult, const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const
{
    // In any case, we need the intake series, so we prepare it now. With it we can extract the last intake.
    // Get the latest (in the past) dosage time range start as the extraction starting time.
    Common::DateTime startTimeOfTheLatestDosage =
            getLatestDosageTimeRangeStart(_xpertRequestResult.getTreatment()->getDosageHistory(), m_computationTime);

    Core::IntakeExtractor intakeExtractor;
    Core::IntakeSeries intakes;
    Core::ComputingStatus cs = intakeExtractor.extract(
            _xpertRequestResult.getTreatment()->getDosageHistory(),
            startTimeOfTheLatestDosage,
            Common::DateTime::undefinedDateTime(),
            1,
            _fullFormulationAndRoute->getValidDoses()->getUnit(),
            intakes);

    // If time is not undefined and the computing result of intake series is ok, then get the last intake.
    if (!startTimeOfTheLatestDosage.isUndefined() && cs == Core::ComputingStatus::Ok) {

        // Set the last intake in the XpertRequestResult.
        unique_ptr<Core::IntakeEvent> lastIntake;
        getLatestIntake(intakes, lastIntake);
        _xpertRequestResult.setLastIntake(std::move(lastIntake));
    }

    // If an adjustment time has been set manually in the request, use it.
    if (!_xpertRequestResult.getXpertRequest().getAdjustmentTime().isUndefined()) {
        return _xpertRequestResult.getXpertRequest().getAdjustmentTime();
    }

    // If time is not undefined and the computing result of intake series is ok
    if (!startTimeOfTheLatestDosage.isUndefined() && cs == Core::ComputingStatus::Ok) {

        // This leaves 3 possible scenarios:
        // 1) There is a treatment in progress -> The next intake after computation time is the adjustment time.
        // 2) There was a treatment that is now finished -> Take the latest intake and add as many times as necessary 2*half-life to reach _start.
        //    The resulting time is the adjustment time.
        // 3) There is no dosage history in the past -> The adjustment time can be at any time, arbitrarily the computation time + 1 hour.

        // We process  1 and 2 (in approximateAdjustmentTimeFromIntakes) before 3, because some computations may "fail",
        // in that case we fallback into 3.

        Common::DateTime adjustmentBasedOnIntakes = approximateAdjustmentTimeFromIntakes(_xpertRequestResult, intakes);

        // We check if 1 or 2 was successful.
        if (!adjustmentBasedOnIntakes.isUndefined()) {
            return adjustmentBasedOnIntakes;
        }
    }

    // 3)  There is no dosage history in the past -> The adjustment time can be at any time, arbitrarily the computation time + 1 hour.
    // Or fallback from 1 and 2
    return m_computationTime + Duration(chrono::hours(1));
}

Common::DateTime AdjustmentTraitCreator::approximateAdjustmentTimeFromIntakes(
        XpertRequestResult& _xpertRequestResult, const Core::IntakeSeries& _intakes) const
{

    // Now, we look for the nearest intake in the future. If there is none, for the last one in the past.
    Common::DateTime possibleAdjustmentTime = getTimeOfNearestFutureOrLatestIntake(_intakes);

    // For some reasons, the intakes series may be empty. In this case, just return undefined date.
    if (possibleAdjustmentTime.isUndefined()) {
        return Common::DateTime::undefinedDateTime();
    }

    // The time of extracted intake is in the past.
    if (possibleAdjustmentTime < m_computationTime) {

        // Add 2 * helf-life until the computing time is reached, it defines the adjustment time.
        const Core::HalfLife& halfLife = _xpertRequestResult.getDrugModel()->getTimeConsiderations().getHalfLife();
        while (possibleAdjustmentTime < m_computationTime) {
            double timeValue =
                    Common::UnitManager::convertToUnit(halfLife.getValue(), halfLife.getUnit(), Common::TucuUnit("h"));
            possibleAdjustmentTime += Common::Duration(chrono::hours(static_cast<int>(2 * timeValue)));
        }
    }

    // At that point, the possible adustment time is:
    // - The nearest  intake in the future -> use it as it is.
    // or
    // - The latest intake in the past on which we added the half life until it reached the computation time.
    return possibleAdjustmentTime;
}

Common::DateTime AdjustmentTraitCreator::getTimeOfNearestFutureOrLatestIntake(const Core::IntakeSeries& _intakes) const
{
    // Saved time to return.
    Common::DateTime savedTime = Common::DateTime::undefinedDateTime();

    // We look from the end of the intakes for the nearest intake in the future
    // or the latest in the past if there is no intake in the future.
    for (auto it = _intakes.rbegin(); it != _intakes.rend(); ++it) {

        // Since we iterate from the end, we remember each future take that is closest to the computation time.
        if (savedTime.isUndefined() || ((*it).getEventTime() < savedTime && (*it).getEventTime() > m_computationTime)) {
            savedTime = (*it).getEventTime();
        }

        // If it is in the past.
        if ((*it).getEventTime() < m_computationTime) {

            // If there is an intake in the future
            if (!savedTime.isUndefined()) {
                return savedTime;
            }
            // There is no future intake.
            return (*it).getEventTime();
        }
    }

    // There is no intake in the series.
    return savedTime; // Undefined date.
}

void AdjustmentTraitCreator::getLatestIntake(
        Core::IntakeSeries _intakes, unique_ptr<Core::IntakeEvent>& _lastIntake) const
{
    Core::IntakeEvent* latestIntake = nullptr;

    // For each intake
    for (Core::IntakeEvent& intake : _intakes) {

        // The intake is saved if:
        //  - the saved one is nullptr and the intake is before the computation time.
        //  or
        //  - the saved one is the oldest and the intake is before the computation
        if (intake.getEventTime() < m_computationTime
            && (latestIntake == nullptr || latestIntake->getEventTime() < intake.getEventTime())) {
            latestIntake = &intake;
        }

        // If all the intakes in the past have been parsed.
        if (intake.getEventTime() > m_computationTime) {
            break;
        }
    }

    // If an intake is found return it, otherwise nullptr.
    _lastIntake = latestIntake == nullptr ? nullptr : make_unique<Core::IntakeEvent>(*latestIntake);
}

void AdjustmentTraitCreator::getPeriod(
        const Core::FullFormulationAndRoute* _fullFormulationAndRoute,
        const XpertRequestResult& _xpertRequestResult,
        const Common::DateTime& _adjustmentTime,
        Common::DateTime& _start,
        Common::DateTime& _end) const
{
    // If the full formulation and route defines a standard treatment with a fixed duration.
    if (_fullFormulationAndRoute->getStandardTreatment() != nullptr
        && _fullFormulationAndRoute->getStandardTreatment()->getIsFixedDuration()) {

        // Getting treatment start.
        _start = getOldestDosageTimeRangeStart(
                _xpertRequestResult.getTreatment()->getDosageHistory(), m_computationTime);

        // Getting treatment end ( start + standard treatment duration )
        double treatmentDuration = _fullFormulationAndRoute->getStandardTreatment()->getDuration();
        treatmentDuration = Common::UnitManager::convertToUnit(
                treatmentDuration, _fullFormulationAndRoute->getStandardTreatment()->getUnit(), Common::TucuUnit("d"));
        _end = _start + Common::Duration(Common::days(static_cast<int>(treatmentDuration)));

        // If the treatment is over.
        if (_end <= _adjustmentTime) {
            throw invalid_argument(
                    "Based on the standard treatment in the model:"
                    + _xpertRequestResult.getDrugModel()->getDrugModelId()
                    + ", considering that the oldest dosage is the treatment start, the treatment is already over at the time of the adjustment.");
        }

        return;
    }

    // If this is not a standard treatment.
    _start = _adjustmentTime;
    _end = _start + Common::Duration(Common::days(7));
}

Core::LoadingOption AdjustmentTraitCreator::getLoadingOption(
        const XpertRequestData& _request, const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const
{
    switch (_request.getLoadingOption()) {
    case LoadingOption::LoadingDoseAllowed:
        return Core::LoadingOption::LoadingDoseAllowed;
    case LoadingOption::NoLoadingDose:
        return Core::LoadingOption::NoLoadingDose;
    case LoadingOption::Unspecified:
        return _fullFormulationAndRoute->isLoadingDoseRecommended() ? Core::LoadingOption::LoadingDoseAllowed
                                                                    : Core::LoadingOption::NoLoadingDose;
    default:
        // Should never happen, as there are only 3 enum values
        return Core::LoadingOption::NoLoadingDose;
    }
}

Core::RestPeriodOption AdjustmentTraitCreator::getRestPeriodOption(
        const XpertRequestData& _request, const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const
{
    switch (_request.getRestPeriodOption()) {
    case RestPeriodOption::RestPeriodAllowed:
        return Core::RestPeriodOption::RestPeriodAllowed;
    case RestPeriodOption::NoRestPeriod:
        return Core::RestPeriodOption::NoRestPeriod;
    case RestPeriodOption::Unspecified:
        return _fullFormulationAndRoute->isRestPeriodRecommended() ? Core::RestPeriodOption::RestPeriodAllowed
                                                                   : Core::RestPeriodOption::NoRestPeriod;
    default:
        // Should never happen, as there are only 3 enum values
        return Core::RestPeriodOption::NoRestPeriod;
    }
}

Core::SteadyStateTargetOption AdjustmentTraitCreator::getSteadyStateTargetOption(
        const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const
{
    // If the full formulation and route defines a standard treatment with a fixed duration.
    if (_fullFormulationAndRoute->getStandardTreatment() != nullptr) {
        return Core::SteadyStateTargetOption::WithinTreatmentTimeRange;
    }
    return Core::SteadyStateTargetOption::AtSteadyState;
}

} // namespace Xpert
} // namespace Tucuxi
