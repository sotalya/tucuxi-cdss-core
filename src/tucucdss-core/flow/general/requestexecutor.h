// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef REQUESTEXECUTOR_H
#define REQUESTEXECUTOR_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This step takes the adjustment trait in the XpertRequestResult
///        and executes it, then makes a new request to extract statistics
///        at steady states and requests to get parameters at "previous types".
class RequestExecutor : public AbstractXpertFlowStep
{
public:
    /// \brief Extract the adjustment trait from the XpertRequestResult, make the request for Tucuxi core and submit it.
    ///        If something fails, the error message of the XpertRequestResult is set and it must not be processed anymore.
    /// \param _xpertRequestResult XpertRequestResult containing the adjustment trait to use.
    void perform(XpertRequestResult& _xpertRequestResult) override;

protected:
    /// \brief This method is called after the first request to Tucuxi core succeed (in perform).
    ///        It collects the statistics at steady state and the parameters for previous prediction types.
    ///        To do so, it creates new traits the extend the end date to reach steady state or that change the parameters type.
    /// \param _xpertRequestResult XpertRequestResult to set the additional data and to get the base adjustment trait.
    void gatherAdditionalData(XpertRequestResult& _xpertRequestResult) const;

    /// \brief Copy a computing adjustment trait but with a different end time,
    ///        points per hour or prediction parameter type. Set the best candidate option to BestDosage,
    ///        since that is the only one we are interested in.
    /// \param _baseTrait Base adjustment trait to copy.
    /// \param _newEnd New end time to use.
    /// \param _newNbPointsPerHour New number of points per hour to use.
    /// \param _newPredictionType New prediction parameters type to use.
    /// \param _resultingAdjustment Resulting new adjustment trait.
    void tweakComputingTraitAdjustment(
            const Core::ComputingTraitAdjustment* _baseTrait,
            const Common::DateTime& _newEnd,
            double _newNbPointsPerHour,
            Core::PredictionParameterType _newPredictionType,
            std::unique_ptr<Core::ComputingTraitAdjustment>& _resultingAdjustment) const;

    void tweakComputingTraitConcentration(
            const Core::ComputingTraitAdjustment* _baseTrait,
            const Common::DateTime& _newEnd,
            double _newNbPointsPerHour,
            Core::PredictionParameterType _newPredictionType,
            std::unique_ptr<Core::ComputingTraitConcentration>& _resultingConcentration) const;

    void tweakComputingTraitConcentrationSteadyState(
            const Core::ComputingTraitAdjustment* _baseTrait,
            const Common::DateTime& _newEnd,
            double _newNbPointsPerHour,
            Core::PredictionParameterType _newPredictionType,
            std::unique_ptr<Core::ComputingTraitConcentration>& _resultingConcentration) const;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // REQUESTEXECUTOR_H
