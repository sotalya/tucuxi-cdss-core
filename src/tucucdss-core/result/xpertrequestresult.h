// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef XPERTREQUESTRESULT_H
#define XPERTREQUESTRESULT_H

#include <string>
#include <vector>

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "tucucdss-core/query/configimport.h"
#include "tucucdss-core/query/justification.h"
#include "tucucdss-core/query/xpertrequestdata.h"
#include "tucucdss-core/result/covariatevalidationresult.h"
#include "tucucdss-core/result/dosedatevalidationresult.h"
#include "tucucdss-core/result/dosevalidationresult.h"
#include "tucucdss-core/result/sampledatevalidationresult.h"
#include "tucucdss-core/result/samplevalidationresult.h"

namespace Tucuxi {
namespace Xpert {

class XpertQueryResult;

/// \brief This is object stores the results of TuberXpert flow steps against a given
///        xpertRequest.
///
///        An XpertRequestResult allows to retrieve all information needed to
///        generate the adjustment report.
///
///        Typically, an XpertRequestResult gives access to:
///            - The XpertQueryResult which provides the admin data, the computation time,
///              the request index and the the output directory path.
///            - The XpertRequestData that at the origin of this XpertRequestResult.
///            - The drug treatment for the xpertRequest drug.
///            - The selected drug model.
///            - The covariate validation results (CovariateValidationResult).
///            - The dose validation results (DoseValidationResult).
///            - The sample validation results (SampleValidationResult).
///            - The adjustment trait used to find a better treatment.
///            - The adjustment data returned by Tucuxi computation core.
///            - The last intake.
///            - The parameters (Typical patient, A priori and eventually A posteriori).
///            - The statistics at steady state.
class XpertRequestResult
{
public:
    /// \brief Constructor. Used in the construction of XpertQueryResult.
    /// \param _xpertQueryResult Where to retrieve the admin data, the computation time,
    ///                          the request index and the the output directory path.
    ///                          Must survive as long as this object is alive (stored reference).
    /// \param _xpertRequest Related requestXpert.
    /// \param _drugTreatment Associated treatment if extraction was successful.
    /// \param _errorMessage If the extraction of the treatment was not successful, the corresponding
    ///                      error message or empty string.
    XpertRequestResult(
            const XpertQueryResult& _xpertQueryResult,
            std::unique_ptr<XpertRequestData> _xpertRequest,
            std::unique_ptr<Core::DrugTreatment> _drugTreatment,
            std::string _errorMessage);

    // Getters

    /// \brief Get the related xpertRequest.
    /// \return The data of xpertRequest.
    [[nodiscard]] const XpertRequestData& getXpertRequest() const;

    /// \brief Get the treatment for the xpertRequest drug.
    /// \return The drug treatment. May be nullptr if extraction failed.
    [[nodiscard]] const Core::DrugTreatment* getTreatment() const;

    /// \brief Get the treatment for the xpertRequest drug.
    /// \return The drug treatment. May be nullptr if extraction failed.
    [[nodiscard]] Core::DrugTreatment* getModifiableTreatment() const;

    /// \brief Get the error message that might be set during the flow steps.
    /// \return The error message. Empty string if everything is fine.
    [[nodiscard]] std::string getErrorMessage() const;

    /// \brief Get the drug model chosen during the CovariateValidatiorAndModelSelector flow step.
    /// \return The selected drug model or nullptr if none.
    [[nodiscard]] const Core::DrugModel* getDrugModel() const;

    /// \brief Get the covariate validation results of the CovariateValidatorAndModelSelector
    ///        flow step.
    /// \return The vector containing each CovariateValidationResult for each covariate.
    ///         May be empty if the flow step has failed/has not been performed.
    [[nodiscard]] const std::vector<CovariateValidationResult>& getCovariateValidationResults() const;

    /// \brief Get the dose validation results of the DoseValidator flow step.
    /// \return The vector containing each DoseValidationResult for each dose found in the treatment.
    ///         This may be empty if there is no dosage or if the flow step has failed/has not been performed.
    [[nodiscard]] const std::map<const Core::DosageBounded*, DoseValidationResult>& getDoseValidationResults() const;

    /// \brief Get the sample validation results of the SampleValidator flow step.
    /// \return The vector containing each SampleValidationResult for each sample found in the treatment.
    ///         This may be empty if there is no sample or if the flow step has failed/has not been performed.
    [[nodiscard]] const std::vector<SampleValidationResult>& getSampleValidationResults() const;

    /// \brief Get the dose date validation results of the DateValidator flow
    ///        step.
    /// \return The vector containing each DoseDateValidationResult for the
    ///         date of each dose found in the treatment.
    ///         This may be empty if there are no doses or if the flow
    ///         step has failed/has not been performed.
    [[nodiscard]] const std::vector<DoseDateValidationResult>& getDoseDateValidationResults() const;

    /// \brief Get the sample date validation results of the DateValidator flow
    ///        step.
    /// \return The vector containing each SampleDateValidationResult for the
    ///         date of each sample found in the treatment.
    ///         This may be empty if there are no samples or if the flow
    ///         step has failed/has not been performed.
    [[nodiscard]] const std::vector<SampleDateValidationResult>& getSampleDateValidationResults() const;

    /// \brief Get the adjustment trait used to make the adjustment request.
    ///        The trait is made during the AdjustmentTraitCreator flow step.
    /// \return The adjustment trait. This may be nullptr if the flow step has failed/has not been performed.
    [[nodiscard]] const Core::ComputingTraitAdjustment* getAdjustmentTrait() const;

    /// \brief Get the adjustment data retrieved by the computating the adjustment trait.
    ///        The data is retrieved by the RequestExecutor flow step.
    /// \return The adjustment data. This may be nullptr if the flow step has failed/has not been performed.
    [[nodiscard]] const Core::AdjustmentData* getAdjustmentData() const;

    /// \brief Get the pre-adjustment a-priori percentiles.
    ///        The data is retrieved by the PADCollector flow step.
    /// \return The pre-adjustment a-priori percentiles. This may be nullptr if the flow step has failed/has not been performed.
    [[nodiscard]] const Core::PercentilesData* getPADAprioriPercentiles() const;

    /// \brief Get the pre-adjustment a-posteriori concentrations.
    ///        The data is retrieved by the PADCollector flow step.
    /// \return The pre-adjustment a-posteriori concentrations. This may be nullptr if the flow step has failed/has not been performed.
    [[nodiscard]] const Core::SinglePredictionData* getPADAposterioriConcentrations() const;

    /// \brief Get the last intake of the patient.
    ///        The data is retrieved by the AdjustmentTraitCreator flow step.
    /// \return The last intake. This may be nullptr if the flow step has failed/has not been performed or
    ///         if there is no last intake.
    [[nodiscard]] const Core::IntakeEvent* getLastIntake() const;

    /// \brief Get the XpertQueryResult object that contains the common information.
    /// \return The XpertQueryResult that has this XpertRequestResult.
    [[nodiscard]] const XpertQueryResult& getXpertQueryResult() const;

    /// \brief Get the parameters groups that depend on the computation type.
    ///        The data is retrieved by the RequestExecutor flow step.
    /// \return The parameters groups. This may be empty if the flow step has failed/has not been performed.
    ///         - In index 0, the typical patient.
    ///         - In index 1, the A priori values.
    ///         - In index 2, the A posteriori values. (Does not exist if the type of computation
    ///           of the adjustment trait is A priori.)
    [[nodiscard]] const std::vector<std::vector<Core::ParameterValue>>& getParameters() const;

    /// \brief Get the extrapolated statistics at steady state.
    ///        The data is retrieved by the RequestExecutor flow step.
    /// \return The extrapolated steady-state statistics.
    [[nodiscard]] const Core::CycleStats& getCycleStats() const;

    [[nodiscard]] const Configuration& getConfiguration() const;

    [[nodiscard]] const Justification& getJustification() const;

    [[nodiscard]] const std::vector<Duration>& getTimeAfterDoseDurations() const;

    // Setters

    /// \brief Define a new error message. This is used by the flow step to
    ///        explain what went wrong.
    /// \param _message Message to set.
    void setErrorMessage(const std::string& _message);

    /// \brief Set a new drug model. Used during the CovariateValidatiorAndModelSelector flow step.
    /// \param _drugModel New drug model pointer.
    void setDrugModel(const Core::DrugModel* _drugModel);

    /// \brief Set a new vector of covariate validation results.
    ///        The results are sorted by covariate name and by date if same covariate name occurs
    ///        several times. Used during the CovariateValidatiorAndModelSelector flow step.
    ///        The names are sorted according to the C++ comparison. This means that 'Z' < 'Â'.
    /// \param _covariateValidationResults Covariate validation results to store.
    void setCovariateResults(std::vector<CovariateValidationResult>&& _covariateValidationResults);

    /// \brief Set a new map of dose validation results.
    ///        Used during the DoseValidatior flow step.
    /// \param _doseValidationResults Dose validation results to store.
    void setDoseResults(std::map<const Core::DosageBounded*, DoseValidationResult>&& _doseValidationResults);

    /// \brief Set a new vector of dose date validation results.
    ///        Used during the DateValidatior flow step.
    /// \param _doseDateValidationResults Dose date validation results to store.
    void setDoseDateResults(std::vector<DoseDateValidationResult>&& _doseDateValidationResults);

    /// \brief Set a new vector of sample date validation results.
    ///        Used during the DateValidatior flow step.
    /// \param _sampleDateValidationResults Sample date validation results to
    ///        store.
    void setSampleDateResults(std::vector<SampleDateValidationResult>&& _sampleDateValidationResults);

    /// \brief Set a new vector of sample validation results.
    ///        Used during the SampleValidatior flow step.
    /// \param _sampleValidationResults Dose validation results to store.
    void setSampleResults(std::vector<SampleValidationResult>&& _sampleValidationResults);

    /// \brief Set the adjustment trait to use when creating the computing request for the core.
    ///        Used during the AdjustmentTraitCreator flow step.
    /// \param _adjustmentTrait Adjustment trait to store.
    void setAdjustmentTrait(const Core::ComputingTraitAdjustment& _adjustmentTrait);

    /// \brief Set the adjustment data that is retrieved after the adjustment trait is executed.
    ///        Used during the RequestExecutor flow step.
    /// \param _adjustmentData Adjustment data to store.
    void setAdjustmentData(std::unique_ptr<Core::AdjustmentData> _adjustmentData);

    void setTimeAfterDoseDurations(std::vector<Duration> _durations);

    /// \brief Set the a-priori percentiles for the pre-adjustment region.
    /// \param _aprioriPercentiles Percentiles to store.
    void setAPrioriPercentiles(std::unique_ptr<Core::PercentilesData> _aprioriPercentiles);

    /// \brief Set the a-posteriori concentrations for the pre-adjustment region.
    /// \param _aposterioriConcentrations Concentrations to store.
    void setAPosterioriConcentrations(std::unique_ptr<Core::SinglePredictionData> _aposterioriConcentrations);

    /// \brief Set the last intake of the patient.
    ///        Used during the AdjustmentTraitCreator flow step.
    /// \param _lastIntake Last intake to save.
    void setLastIntake(std::unique_ptr<Core::IntakeEvent> _lastIntake);

    /// \brief Emplace front a group of parameters.
    ///        Used during the RequestExecutor flow step.
    ///        When the adjustment trait is A priori, this method is called
    ///        two times: for the parameters A priori, then for the parameters of the
    ///        typical patient.
    ///        When the adjustment trait is A posteriori, this method is called three times:
    ///        for the parameters A posteriori, then for the parameters a priori and finally for
    ///        the parameters of the Typical patient.
    /// \param _paramters The parameters to saves.
    void addParameters(const std::vector<Core::ParameterValue>& _parameters);

    /// \brief Set the extrapolated statistics at steady state.
    ///        Used during the RequestExecutor flow step.
    /// \param _cycleStats The extrapolated statistics at steady state to save.
    void setCycleStats(Core::CycleStats _cycleStats);

    void setConfiguration(const std::unique_ptr<Configurations>& _configurations);

    void setJustification(const Justification& _justification);

    /// \brief Take ownership of a dynamically created CovariateDefinition.
    ///        Returns the raw pointer for use in non-owning contexts (e.g. CovariateValidationResult).
    /// \param _definition The CovariateDefinition to own.
    /// \return Raw pointer to the owned object.
    const Core::CovariateDefinition* addOwnedDefinition(std::unique_ptr<Core::CovariateDefinition> _definition);

    /// \brief Take ownership of a dynamically created PatientCovariate.
    ///        Returns the raw pointer for use in non-owning contexts (e.g. CovariateValidationResult).
    /// \param _covariate The PatientCovariate to own.
    /// \return Raw pointer to the owned object.
    const Core::PatientCovariate* addOwnedPatientCovariate(std::unique_ptr<Core::PatientCovariate> _covariate);

    // Others

    /// \brief Check if the XpertRequestResult should be processed by the next flow step.
    /// \return True if no problems were detected during the previous flow steps, otherwise false.
    [[nodiscard]] bool shouldContinueProcessing() const;

protected:
    /// \brief The XpertQueryResult which provides the admin data, the computation time,
    ///        the request index and the the output directory path. Owner of this
    ///        XpertRequestResult.
    const XpertQueryResult& m_xpertQueryResult;

    /// \brief Related requestXpert.
    std::unique_ptr<XpertRequestData> m_xpertRequest;

    /// \brief The drug treatment for the xpertRequest drug.
    std::unique_ptr<Core::DrugTreatment> m_drugTreatment;

    /// \brief Error message possibly set during a flow step.
    std::string m_errorMessage;

    /// \brief The drug model chosen during the CovariateValidatiorAndModelSelector flow step.
    const Core::DrugModel* m_drugModel;

    /// \brief Get the covariate validation results of the CovariateValidatorAndModelSelector
    ///        flow step.
    ///        One entry per patient covariate present and by definition missing
    ///        with respect to the selected drug model.
    std::vector<CovariateValidationResult> m_covariateValidationResults;

    /// \brief Validation result for each dose performed during DoseValidator flow step.
    ///        One entry per single dose found. The keys of the map are the same pointers
    ///        as the source in each DoseValidationResult.
    std::map<const Core::DosageBounded*, DoseValidationResult> m_doseValidationResults;

    /// \brief Validation result for each sample made during SampleValidator flow step.
    ///        One entry per sample found.
    std::vector<SampleValidationResult> m_sampleValidationResults;

    /// \brief Validation result for each dose date made during the DateValidator
    ///        flow step.
    ///        One entry per date found.
    std::vector<DoseDateValidationResult> m_doseDateValidationResults;

    /// \brief Validation result for each sample date made during the
    ///        DateValidator flow step.
    ///        One entry per date found.
    std::vector<SampleDateValidationResult> m_sampleDateValidationResults;

    /// \brief Adjustment trait used to make the adjustment request
    ///        The trait is made during the AdjustmentTraitCreator flow step.
    std::unique_ptr<Core::ComputingTraitAdjustment> m_adjustmentTrait;

    /// \brief Adjustment data retrieved by the computating the adjustment trait.
    ///        The data is retrieved by the RequestExecutor flow step.
    std::unique_ptr<Core::AdjustmentData> m_adjustmentData;

    /// \brief A-priori percentiles.
    ///        The data is retrieved by the PADCollector flow step.
    std::unique_ptr<Core::PercentilesData> m_aprioriPercentiles;

    /// \brief A-posteriori concentrations.
    ///        The data is retrieved by the PADCollector flow step.
    std::unique_ptr<Core::SinglePredictionData> m_aposterioriConcentrations;

    /// \brief The last intake of the patient.
    ///        The data is retrieved by the AdjustmentTraitCreator flow step.
    std::unique_ptr<Core::IntakeEvent> m_lastIntake;

    /// \brief The parameters groups that depend on the computation type.
    ///        The data is retrieved by the RequestExecutor flow step.
    ///        - In index 0, the typical patient.
    ///        - In index 1, the a priori values.
    ///        - In index 2, the a posteriori values. (Does not exist if the type of computation
    ///        of the base adjustment trait is a priori.)
    std::vector<std::vector<Core::ParameterValue>> m_parameters;

    /// \brief Extrapolated statistics at steady state.
    ///        The data is retrieved by the RequestExecutor flow step.
    Core::CycleStats m_cycleStats;

    Configuration m_configuration;

    Justification m_justification;

    std::vector<Duration> m_timeAfterDoses;

    /// \brief Ownership pool for dynamically created CovariateDefinitions.
    std::vector<std::unique_ptr<Core::CovariateDefinition>> m_ownedDefinitions;

    /// \brief Ownership pool for dynamically created PatientCovariates.
    std::vector<std::unique_ptr<Core::PatientCovariate>> m_ownedPatientCovariates;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // XPERTREQUESTRESULT_H
