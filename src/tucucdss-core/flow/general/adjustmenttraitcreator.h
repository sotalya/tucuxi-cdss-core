// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef ADJUSTMENTTRAITCREATOR_H
#define ADJUSTMENTTRAITCREATOR_H

#include "tucucommon/datetime.h"

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/query/xpertrequestdata.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class is responsible for creating the adjustment trait that will be submitted
///        to Tucuxi core. It uses the whole dosage history.
///
///        Since this class must compute the intakes of the patient, it also defines his
///        last intake.
class AdjustmentTraitCreator : public AbstractXpertFlowStep
{
public:
    /// \brief Create the adjustment trait based on the information in the XpertRequestResult and
    ///        set the last intake.
    /// \param _xpertRequestResult XpertRequestResult containing the xpertRequest, the treatment,
    ///        and the drug model that will recieve the trait when it is created.
    void perform(XpertRequestResult& _xpertRequestResult) override;

protected:
    /// \brief Extract the prediction parameter type based on the treatment.
    /// \param _drugTreatment Drug treatment used to extract the prediction parameter type.
    /// \return Core::PredictionParameterType::Aposteriori if there are dosages and samples in the treatment,
    ///         otherwise Core::PredictionParameterType::Apriori.
    [[nodiscard]] Core::PredictionParameterType getPredictionParameterType(
            const Core::DrugTreatment* _drugTreatment) const;

    /// \brief Get the adjustment time. Also, if there is one last intake
    ///        of the patient, set it in the XpertRequestResult.
    ///        - If the adjustment time is set in the XpertRequest, it returns it.
    ///        - Otherwise:
    ///            - If there is a treatment in progress, it returns the next intake time.
    ///            - If there is a completed treatment, from the last intake time,it adds 2 * half-life of the
    ///              drug until the computation time is reached. The resulting time is returned.
    ///            - If there is no treatment, it returns the computing time plus 1 hour.
    /// \param _xpertRequestResult XpertRequestResult containing the treatment and the drug model to extract intakes
    ///        and the request.
    /// \param _fullFormulationAndRoute Full formulation and route associated to the treatment.
    /// \return The adjustment time.
    Common::DateTime getAdjustmentTimeAndLastIntake(
            XpertRequestResult& _xpertRequestResult,
            const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const;

    /// \brief Approximation of the adjustment time bases on the given intake series.
    ///        This function is called by "getAdjustmentTimeAndLastIntake".
    /// \param _xpertRequestResult XpertRequestResult used to get the half life of the drug model.
    /// \param _intakes Intake series used for approximation.
    /// \return The adjustment time if found one, otherwise an undefined time.
    Common::DateTime approximateAdjustmentTimeFromIntakes(
            XpertRequestResult& _xpertRequestResult, const Core::IntakeSeries& _intakes) const;

    /// \brief Given an intake series, try to extract the closest intake in the future. If none is found,
    ///        extract the closed in the past. By closest, we mean "the closest to the computation time".
    /// \param _intakes Intake series to work with.
    /// \return The time found, otherwise an undefined time if the series is empty.
    [[nodiscard]] Common::DateTime getTimeOfNearestFutureOrLatestIntake(const Core::IntakeSeries& _intakes) const;

    /// \brief Get a pointer on the last intake of the intake series that is before the computation time.
    /// \param _intakes Intake series to work with.
    /// \param _lastIntake Unique pointer to store the result.
    void getLatestIntake(Core::IntakeSeries _intakes, std::unique_ptr<Core::IntakeEvent>& _lastIntake) const;

    /// \brief Extract the start and end time of the adjustment.
    ///        - If the treatment is not a standard treatment:
    ///            - start = adjustment time
    ///            - end = start + 7 days
    ///        - else, if the treatment is a standard treatment:
    ///            - start = the start time of the oldest time range in the dosage history or
    ///                    the computation time if the dosage history is empty.
    ///            - end = start + standard treatment duration.
    /// \param _fullFormulationAndRoute Full formulation and route associated to the treatment.
    /// \param _xpertRequestResult XpertRequestResult used to get the dosage history and the xpertRequest.
    /// \param _adjustmentTime Estimated adjustment time.
    /// \param _start Resulting start time.
    /// \param _end Resulting end time.
    void getPeriod(
            const Core::FullFormulationAndRoute* _fullFormulationAndRoute,
            const XpertRequestResult& _xpertRequestResult,
            const Common::DateTime& _adjustmentTime,
            Common::DateTime& _start,
            Common::DateTime& _end) const;

    /// \brief Retrieve the loading option of the xpertRequest or the full
    ///        formulation and route if not defined in request.
    /// \param _request Request where the rest period option may be set.
    /// \param _fullFormulationAndRoute Full formulation and route from which use "isLoadingDoseRecommended"
    ///        if the XpertRequest does not specify a choice.
    /// \return The resulting loading option.
    Core::LoadingOption getLoadingOption(
            const XpertRequestData& _request, const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const;

    /// \brief Retrieve the rest period option of the xpertRequest or the full
    ///        formulation and route if not defined in request.
    /// \param _request Request where the rest period option may be set.
    /// \param _fullFormulationAndRoute Full formulation and route from which use "isRestPeriodRecommended"
    ///        if the XpertRequest does not specify a choice.
    /// \return The resulting rest period option.
    Core::RestPeriodOption getRestPeriodOption(
            const XpertRequestData& _request, const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const;

    /// \brief Define the steady state target option to be used.
    ///        - If the full formulation and route defines a standard treatment:
    ///             - within treatment time range.
    ///        - Else:
    ///             - at steady state.
    /// \param _fullFormulationAndRoute Full formulation and route from the drug model used with
    ///        the drug treatment which indicates whether it is a standard treatment.
    /// \return The corresponding steady state target option.
    Core::SteadyStateTargetOption getSteadyStateTargetOption(
            const Core::FullFormulationAndRoute* _fullFormulationAndRoute) const;

    /// \brief Fix the computation time to get the same values when testing
    ///        at different times. Retrieved from the XpertRequestResult in perform method.
    ///        It allows not to be forced to use local time when time-based calculations are needed.
    Common::DateTime m_computationTime;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // ADJUSTMENTTRAITCREATOR_H
