// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef COVARIATEVALIDATORANDMODELSELECTOR_H
#define COVARIATEVALIDATORANDMODELSELECTOR_H

#include "tucucommon/datetime.h"

#include "tucucore/dosage.h"

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/query/xpertrequestdata.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This is a class responsible for obtaining the best drug model for a given drug
///        and validating the relevance of covariates.
///
///        For each available model of a given drug that matches to the formulation and route
///        of the doses, we calculate a dissimilarity score S based on the covariates of the model.
///        S =∑ (missing covariate) + ∑(covariate not respecting constraint (maximum once per covariate definition))
///        The model with the lowest dissimilarity score is chosen. In case of a tie, the model with the most
///        covariate definitions is chosen.
///
///        The selected drug model must support the xpertRequest language or at least English.
///        If it doesn't, the XpertRequestResult gets its error message set and it is not processed anymore.
///
class CovariateValidatorAndModelSelector : public AbstractXpertFlowStep
{
public:
    /// \brief For a given XpertRequestResult, get the best drug model and define
    ///        the CovariateValidationResult vector.
    /// \param _xpertRequestResult XpertRequestResult to process.
    void perform(XpertRequestResult& _xpertRequestResult) override;

protected:
    /// \brief Check that patient dose formulations and routes are all equal.
    /// \param _dosageHistory Patient's dosage history.
    /// \return True if all the formulations and routes are equal, otherwise false.
    [[nodiscard]] bool checkFormulationsAndRoutesCompatibility(const Core::DosageHistory& _dosageHistory) const;

    /// \brief Get the oldest date of the patient's covariates.
    /// \param _patientCovariates List of patient's covariates.
    /// \return Date and time of the oldest covariate. If there are no covariates, returns
    ///         the computation time.
    [[nodiscard]] Common::DateTime getOldestCovariateDateTime(const Core::PatientVariates& _patientCovariates) const;

    /// \brief For some given covariate definitions from a drug model,
    ///        compute the drug model score based on the patient's covariates.
    /// \param _patientCovariates Patient's covariates.
    /// \param _modelDefinitions Covariate definitions of the drug model.
    /// \param _lang Language of the xpertRequest to get the correct translation of the error message
    ///         of the definition when filling _results.
    /// \param _results Vector to store the covariate validation results for this model.
    /// \return The score of the model.
    /// \throw invalid_argument When the conversion between the patient covariate and model
    ///        definition fails, if a definition does not support english and the requested language
    ///        and if the validation of a covariate definition fails.
    unsigned computeScore(
            const Core::PatientVariates& _patientCovariates,
            const Core::CovariateDefinitions& _modelDefinitions,
            OutputLang _lang,
            std::vector<CovariateValidationResult>& _results) const;

    /// \brief For a given covariate definition, obtain  its validation operation and check
    ///        whether the patient covariate's value is valid.
    ///        If the operation could be performed completely, push the patient's covariate and
    ///        its definition in the results.
    /// \param _patientConvertedValue Converted value of the patient covariate to match the unit of definition.
    /// \param _definition Covariate definition to obtain the validation operation and to push in
    ///                    the validation results.
    /// \param _patient Patient covariate to be pushed into the results in case of operation that
    ///                 returns true.
    /// \param _lang Language of the request to get the correct translation of the error message from the definition.
    /// \param _results Vector of covariate validation results to push patient and definition covariate.
    /// \throw invalid_argument When the operation could not be fully performed due to an error.
    bool checkOperation(
            double _patientConvertedValue,
            const Core::CovariateDefinition* _definition,
            const Core::PatientCovariate* _patient,
            OutputLang _lang,
            std::vector<CovariateValidationResult>& _results) const;

    /// \brief For a given set of covariate definitions. This method checks that they
    ///        all support the requested output language or at least English.
    ///        The name, the description and the validation error message are checked.
    /// \param _modelDefinitions List of covariate definitions.
    /// \param _lang Output Desired language.
    /// \return Return true if english or _lang is supported otherwise false.
    [[nodiscard]] bool checkCovariateDefinitionsSupportedLanguage(
            const Core::CovariateDefinitions& _modelDefinitions, OutputLang _lang) const;

    /// \brief Fix the computation time to get the same age when executing
    ///        at different times. Retrieved from the XpertRequestResult in perform method.
    ///        It allows not to be forced to use local time when time-based calculations are needed.
    Common::DateTime m_computationTime;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // COVARIATEVALIDATORANDMODELSELECTOR_H
