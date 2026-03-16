// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef DOSEVALIDATOR_H
#define DOSEVALIDATOR_H

#include "tucucore/drugmodel/formulationandroute.h"

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class is responsible for checking the plausibility of each dose.
///        It does this by comparing each patient's doses to the doses recommended
///        by the drug model.
///
///        The current implementation considers every dose. A future improvement for a
///        specific drug could be to consider only doses that are no older than X years/months/days...
///
///        It assumes that the LanguageManager is loaded with a complete translations file.
class DoseValidator : public AbstractXpertFlowStep
{
public:
    /// \brief Evaluate all doses in the treatment of the given XpertRequestResult.
    ///        There must be a drug model and a treatment set. It assumes that the drug model
    ///        is compatible.
    ///        If the evaluation fails (for example: incompatible dose units) the XpertRequestResult
    ///        is invalidated (i.e. it gets an error).
    /// \param _xpertRequestResult XpertRequestResult to evaluate doses.
    void perform(XpertRequestResult& _xpertRequestResult) override;

protected:
    /// \brief Parse a given DosageHistory and evaluate the contained doses.
    /// \param _dosageHistory Dosage history to parse.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::DosageHistory& _dosageHistory,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Parse a given DosageTimeRange and evaluate the contained doses.
    /// \param _timeRange Dosage time range to parse.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::DosageTimeRange& _timeRange,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Convert an abstract dosage to its real type to call the corresponding checkDoses method.
    /// \param _dosage Dosage to convert.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::Dosage& _dosage,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Parse a given DosageLoop and evaluate the contained doses.
    /// \param _dosageLoop Dosage loop to parse.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::DosageLoop& _dosageLoop,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Parse a given DosageRepeat and evaluate the contained doses.
    /// \param _dosageRepeat Dosage repeat to parse.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::DosageRepeat& _dosageRepeat,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Parse a given DosageSequence and evaluate the contained doses.
    /// \param _dosageSequence Dosage sequence to parse.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::DosageSequence& _dosageSequence,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Parse a given ParallelDosageSequence and evaluate the contained doses.
    /// \param _parallelDosageSequence Parallel dosage sequence to parse.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDoses(
            const Core::ParallelDosageSequence& _parallelDosageSequence,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Parse all doses from a DosageBoundedList. Mainly used by check dose method with
    ///        dosage sequence and parallel dosage sequence.
    /// \param _dosageBoundedList Dosage bounded list to evaluate the contained doses.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the results of the dose validations.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversions have failed.
    void checkDosageBoundedList(
            const Core::DosageBoundedList& _dosageBoundedList,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief Validate a dose value against the drug model's recommended doses for a given formulation and route.
    /// \param _dosage The dosage bounded pointer used as key in the results map.
    /// \param _dosageFr Formulation and route of the dosage to validate.
    /// \param _doseValue The dose value to check.
    /// \param _doseUnit The unit of the dose value.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the result of the dose validation.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversion has failed.
    void validateDose(
            const Core::DosageBounded* _dosage,
            const Core::FormulationAndRoute& _dosageFr,
            double _doseValue,
            const Common::TucuUnit& _doseUnit,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief For a list of doses at a specific time, check its compatibility with the drug model's recommended doses.
    ///        The drug model doses are located in the list of formulations and routes in argument.
    /// \param _singleDoseAtTimeList Simple dose list to check.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the result of the single dose evaluation.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversion has failed.
    void checkDoses(
            const Core::SingleDoseAtTimeList& _singleDoseAtTimeList,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief For a simple dose list, check its compatibility with the drug model's recommended doses.
    ///        The drug model doses are located in the list of formulations and routes in argument.
    /// \param _simpleDoseList Simple dose list to check.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the result of the single dose evaluation.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversion has failed.
    void checkDoses(
            const Core::SimpleDoseList& _simpleDoseList,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;

    /// \brief For a single dose, check its compatibility with the drug model's recommended doses.
    ///        The drug model doses are located in the list of formulations and routes in argument.
    /// \param _singleDose Single dose to check.
    /// \param _modelFormulationsAndRoutes Formulations and routes available in the drug model.
    /// \param _results Map to store the result of the single dose evaluation.
    /// \throw invalid_argument If compatible formulations and routes are not found or
    ///                         if unit conversion has failed.
    void checkDoses(
            const Core::SingleDose& _singleDose,
            const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
            std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // DOSEVALIDATOR_H
