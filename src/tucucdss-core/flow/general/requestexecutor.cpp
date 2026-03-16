// SPDX-License-Identifier: AGPL-3.0-or-later
#include <memory>

#include "requestexecutor.h"

#include "tucucommon/loggerhelper.h"

#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void RequestExecutor::perform(XpertRequestResult& _xpertRequestResult)
{
    // Check if there is an adjustment trait.
    if (_xpertRequestResult.getAdjustmentTrait() == nullptr) {
        _xpertRequestResult.setErrorMessage("No adjustment trait set.");
        return;
    }

    // Check if there is drug model.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model set.");
        return;
    }

    // Create a copy of the adjustment trait and prepare the response holder.
    unique_ptr<Core::ComputingTraitAdjustment> baseAdjustmentTrait =
            make_unique<Core::ComputingTraitAdjustment>(*_xpertRequestResult.getAdjustmentTrait());
    unique_ptr<Core::AdjustmentData> adjustmentResult = nullptr;

    // Execute the request
    executeRequestAndGetResult<Core::ComputingTraitAdjustment, Core::AdjustmentData>(
            std::move(baseAdjustmentTrait), _xpertRequestResult, adjustmentResult);

    // If the request execution failed
    if (adjustmentResult == nullptr) {
        _xpertRequestResult.setErrorMessage("Adjustment request execution failed.");
        return;
    }

    // If all went well.

    if (adjustmentResult->getAdjustments().empty()) {
        _xpertRequestResult.setErrorMessage("No adjustment found.");
        return;
    }

    // Save the parameters for the current request type (A priori or A posteriori).
    _xpertRequestResult.addParameters(adjustmentResult->getAdjustments().front().getData().front().m_parameters);

    // Save the adjustment data into the XpertRequestResult
    _xpertRequestResult.setAdjustmentData(std::move(adjustmentResult));
    // Get the statistics at steady state and the parameters of previous type.
    gatherAdditionalData(_xpertRequestResult);
}

void RequestExecutor::gatherAdditionalData(XpertRequestResult& _xpertRequestResult) const
{
    unique_ptr<Core::ComputingTraitAdjustment> baseAdjustmentTrait =
            make_unique<Core::ComputingTraitAdjustment>(*_xpertRequestResult.getAdjustmentTrait());

    // ------- statistics at steady state ---------

    // Update the end time to approximate a steady state to extract the statistics at steady state.
    // Ideally, this would have been done in a single query, but it is not possible to remove the extra cycle
    // data in the current core state. We should have included them with the xpert request result.
    // A steady state is approximated with adjustment time + multiplier * half life.
    const Core::HalfLife& halfLife = _xpertRequestResult.getDrugModel()->getTimeConsiderations().getHalfLife();
    double hoursToAdd =
            Common::UnitManager::convertToUnit(halfLife.getValue(), halfLife.getUnit(), Common::TucuUnit("h"));
    Common::DateTime staeadyEndTime =
            baseAdjustmentTrait->getAdjustmentTime()
            + Duration(chrono::hours(static_cast<int>(halfLife.getMultiplier() * hoursToAdd)));

    unique_ptr<Core::ComputingTraitConcentration> steadyStateConcentrationTrait = nullptr;
    tweakComputingTraitConcentrationSteadyState(
            baseAdjustmentTrait.get(),
            staeadyEndTime,
            20,
            baseAdjustmentTrait->getComputingOption().getParametersType(),
            steadyStateConcentrationTrait);
    unique_ptr<Core::SinglePredictionData> steadyConcentrationResult = nullptr;

    Core::DosageHistory& history = _xpertRequestResult.getModifiableTreatment()->getModifiableDosageHistory();
    Common::DateTime oldEndDate;
    if (!history.getDosageTimeRanges().empty()) {
        oldEndDate = history.getDosageTimeRanges().back()->getEndDate();
        // We articifially modify the end date to allow a steady state computation
        history.getDosageTimeRanges().back()->setEndDate(oldEndDate + 365 * 24h);
    }
    executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
            std::move(steadyStateConcentrationTrait), _xpertRequestResult, steadyConcentrationResult);

    if (!history.getDosageTimeRanges().empty()) {
        // Back to the original history
        history.getDosageTimeRanges().back()->setEndDate(oldEndDate);
    }

    if (steadyConcentrationResult == nullptr) {
        _xpertRequestResult.setErrorMessage("Failed to extract statistics at steady state.");
        return;
    }

    if (steadyConcentrationResult->getData().empty()) {
        steadyConcentrationResult = nullptr;
        // auto history = _xpertRequestResult.getAdjustmentData()->getAdjustments()[0].getDosageHistory();
        tweakComputingTraitConcentration(
                baseAdjustmentTrait.get(),
                staeadyEndTime,
                20,
                baseAdjustmentTrait->getComputingOption().getParametersType(),
                steadyStateConcentrationTrait);

        Core::DrugTreatment treatment = Core::DrugTreatment();

        const auto& adjustments = _xpertRequestResult.getAdjustmentData()->getAdjustments();
        const auto& dosageHistory = adjustments[0].getDosageHistory();
        const auto& dosageTimeRangeList = dosageHistory.getDosageTimeRanges();

        for (auto& dosageTimeRange : dosageTimeRangeList) {
            treatment.getModifiableDosageHistory().addTimeRange(*dosageTimeRange);
        }

        executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
                std::move(steadyStateConcentrationTrait), _xpertRequestResult, treatment, steadyConcentrationResult);
    }

    _xpertRequestResult.setCycleStats(steadyConcentrationResult->getData().back().m_statistics);
    /*
    for(size_t i = 0; i < steadyConcentrationResult->getData().size(); i++) {
        auto stats = steadyConcentrationResult->getData().at(i).m_statistics;
        auto s = stats.getStatistic(0, Core::CycleStatisticType::Residual);
        DateTime d;
        double value;
        s.getValue(d, value, i);
        std::cout << "s" << i << " : " << value << '\n';
    }
*/

    // ------- Parameters type "A priori" ---------

    // We don't want to extract them again, so we check the base trait prediction parameter type.
    if (baseAdjustmentTrait->getComputingOption().getParametersType() == Core::PredictionParameterType::Aposteriori) {

        unique_ptr<Core::ComputingTraitConcentration> aprioriConcentrationTrait = nullptr;
        tweakComputingTraitConcentration(
                baseAdjustmentTrait.get(),
                baseAdjustmentTrait->getEnd(),
                1,
                Core::PredictionParameterType::Apriori,
                aprioriConcentrationTrait);
        unique_ptr<Core::SinglePredictionData> aprioriConcentrationResult = nullptr;
        executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
                std::move(aprioriConcentrationTrait), _xpertRequestResult, aprioriConcentrationResult);

        // If execution failed.
        if (aprioriConcentrationResult == nullptr) {
            _xpertRequestResult.setErrorMessage("Failed to extract apriori parameters.");
            return;
        }

        if (aprioriConcentrationResult->getData().empty()) {
            aprioriConcentrationResult = nullptr;

            tweakComputingTraitConcentration(
                    baseAdjustmentTrait.get(),
                    baseAdjustmentTrait->getEnd(),
                    1,
                    Core::PredictionParameterType::Apriori,
                    aprioriConcentrationTrait);
            Core::DrugTreatment treatment = Core::DrugTreatment();

            const auto& adjustments = _xpertRequestResult.getAdjustmentData()->getAdjustments();
            const auto& dosageHistory = adjustments[0].getDosageHistory();
            const auto& dosageTimeRangeList = dosageHistory.getDosageTimeRanges();

            for (auto& dosageTimeRange : dosageTimeRangeList) {
                treatment.getModifiableDosageHistory().addTimeRange(*dosageTimeRange);
            }

            executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
                    std::move(aprioriConcentrationTrait), _xpertRequestResult, treatment, aprioriConcentrationResult);
        }

        // Saving the "A priori" parameters.
        _xpertRequestResult.addParameters(aprioriConcentrationResult->getData().front().m_parameters);
    }

    // ------- Parameters "Typical patient" ---------

    unique_ptr<Core::ComputingTraitConcentration> populationConcentrationTrait = nullptr;
    tweakComputingTraitConcentration(
            baseAdjustmentTrait.get(),
            baseAdjustmentTrait->getEnd(),
            1,
            Core::PredictionParameterType::Population,
            populationConcentrationTrait);
    unique_ptr<Core::SinglePredictionData> populationConcentrationResult = nullptr;
    executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
            std::move(populationConcentrationTrait), _xpertRequestResult, populationConcentrationResult);

    if (populationConcentrationResult == nullptr) {
        _xpertRequestResult.setErrorMessage("Failed to extract statistics at steady state.");
        return;
    }

    if (populationConcentrationResult->getData().empty()) {
        populationConcentrationResult = nullptr;

        tweakComputingTraitConcentration(
                baseAdjustmentTrait.get(),
                baseAdjustmentTrait->getEnd(),
                1,
                Core::PredictionParameterType::Population,
                populationConcentrationTrait);

        Core::DrugTreatment treatment = Core::DrugTreatment();

        const auto& adjustments = _xpertRequestResult.getAdjustmentData()->getAdjustments();
        const auto& dosageHistory = adjustments[0].getDosageHistory();
        const auto& dosageTimeRangeList = dosageHistory.getDosageTimeRanges();

        for (auto& dosageTimeRange : dosageTimeRangeList) {
            treatment.getModifiableDosageHistory().addTimeRange(*dosageTimeRange);
        }

        executeRequestAndGetResult<Core::ComputingTraitConcentration, Core::SinglePredictionData>(
                std::move(populationConcentrationTrait), _xpertRequestResult, treatment, populationConcentrationResult);
    }

    _xpertRequestResult.addParameters(populationConcentrationResult->getData().front().m_parameters);
}


void RequestExecutor::tweakComputingTraitConcentration(
        const Core::ComputingTraitAdjustment* _baseTrait,
        const Common::DateTime& _newEnd,
        double _newNbPointsPerHour,
        Core::PredictionParameterType _newPredictionType,
        std::unique_ptr<Core::ComputingTraitConcentration>& _resultingConcentration) const
{
    Core::ComputingOption newOptions{
            _newPredictionType,
            _baseTrait->getComputingOption().getCompartmentsOption(),
            _baseTrait->getComputingOption().retrieveStatistics(),
            _baseTrait->getComputingOption().retrieveParameters(),
            _baseTrait->getComputingOption().retrieveCovariates()};

    _resultingConcentration = make_unique<Core::ComputingTraitConcentration>(
            "", _baseTrait->getStart(), _newEnd, _newNbPointsPerHour, newOptions);
}

void RequestExecutor::tweakComputingTraitConcentrationSteadyState(
        const Core::ComputingTraitAdjustment* _baseTrait,
        const Common::DateTime& _newEnd,
        double _newNbPointsPerHour,
        Core::PredictionParameterType _newPredictionType,
        std::unique_ptr<Core::ComputingTraitConcentration>& _resultingConcentration) const
{
    Core::ComputingOption newOptions{
            _newPredictionType,
            _baseTrait->getComputingOption().getCompartmentsOption(),
            _baseTrait->getComputingOption().retrieveStatistics(),
            _baseTrait->getComputingOption().retrieveParameters(),
            _baseTrait->getComputingOption().retrieveCovariates()};

    _resultingConcentration = make_unique<Core::ComputingTraitConcentration>(
            "", _newEnd - Duration(72h), _newEnd, _newNbPointsPerHour, newOptions);
}

void RequestExecutor::tweakComputingTraitAdjustment(
        const Core::ComputingTraitAdjustment* _baseTrait,
        const Common::DateTime& _newEnd,
        double _newNbPointsPerHour,
        Core::PredictionParameterType _newPredictionType,
        std::unique_ptr<Core::ComputingTraitAdjustment>& _resultingAdjustment) const
{

    // Make new computing option with the new prediction parameter type.
    Core::ComputingOption newOptions{
            _newPredictionType,
            _baseTrait->getComputingOption().getCompartmentsOption(),
            _baseTrait->getComputingOption().retrieveStatistics(),
            _baseTrait->getComputingOption().retrieveParameters(),
            _baseTrait->getComputingOption().retrieveCovariates()};

    // Make a new adjustment trait with new number of points per hour, new end time and
    // for the best dosage.
    _resultingAdjustment = make_unique<Core::ComputingTraitAdjustment>(
            "",
            _baseTrait->getStart(),
            _newEnd,
            _newNbPointsPerHour,
            newOptions,
            _baseTrait->getAdjustmentTime(),
            Core::BestCandidatesOption::BestDosage,
            _baseTrait->getLoadingOption(),
            _baseTrait->getRestPeriodOption(),
            _baseTrait->getSteadyStateTargetOption(),
            _baseTrait->getTargetExtractionOption(),
            _baseTrait->getFormulationAndRouteSelectionOption());
}

} // namespace Xpert
} // namespace Tucuxi
