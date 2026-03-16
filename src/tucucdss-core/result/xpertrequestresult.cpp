// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include "xpertrequestresult.h"

#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

XpertRequestResult::XpertRequestResult(
        const XpertQueryResult& _xpertQueryResult,
        unique_ptr<XpertRequestData> _xpertRequest,
        unique_ptr<Core::DrugTreatment> _drugTreatment,
        std::string _errorMessage)
    : m_xpertQueryResult(_xpertQueryResult), m_xpertRequest(std::move(_xpertRequest)),
      m_drugTreatment(std::move(_drugTreatment)), m_errorMessage(std::move(_errorMessage)), m_drugModel(nullptr),
      m_adjustmentTrait(nullptr), m_adjustmentData(nullptr), m_lastIntake(nullptr)
{
}

const XpertRequestData& XpertRequestResult::getXpertRequest() const
{
    return *m_xpertRequest;
}

const Core::DrugTreatment* XpertRequestResult::getTreatment() const
{
    return m_drugTreatment.get();
}

Core::DrugTreatment* XpertRequestResult::getModifiableTreatment() const
{
    return m_drugTreatment.get();
}

const Configuration& XpertRequestResult::getConfiguration() const
{
    return m_configuration;
}

const Justification& XpertRequestResult::getJustification() const
{
    return m_justification;
}

const std::vector<Duration>& XpertRequestResult::getTimeAfterDoseDurations() const
{
    return m_timeAfterDoses;
}

std::string XpertRequestResult::getErrorMessage() const
{
    return m_errorMessage;
}

const Core::DrugModel* XpertRequestResult::getDrugModel() const
{
    return m_drugModel;
}

const vector<CovariateValidationResult>& XpertRequestResult::getCovariateValidationResults() const
{
    return m_covariateValidationResults;
}

const map<const Core::DosageBounded*, DoseValidationResult>& XpertRequestResult::getDoseValidationResults() const
{
    return m_doseValidationResults;
}

const vector<SampleValidationResult>& XpertRequestResult::getSampleValidationResults() const
{
    return m_sampleValidationResults;
}

const vector<DoseDateValidationResult>& XpertRequestResult::getDoseDateValidationResults() const
{
    return m_doseDateValidationResults;
}

const vector<SampleDateValidationResult>& XpertRequestResult::getSampleDateValidationResults() const
{
    return m_sampleDateValidationResults;
}

const Core::ComputingTraitAdjustment* XpertRequestResult::getAdjustmentTrait() const
{
    return m_adjustmentTrait.get();
}

const Core::AdjustmentData* XpertRequestResult::getAdjustmentData() const
{
    return m_adjustmentData.get();
}

const Core::PercentilesData* XpertRequestResult::getPADAprioriPercentiles() const
{
    return m_aprioriPercentiles.get();
}

const Core::SinglePredictionData* XpertRequestResult::getPADAposterioriConcentrations() const
{
    return m_aposterioriConcentrations.get();
}

const Core::IntakeEvent* XpertRequestResult::getLastIntake() const
{
    return m_lastIntake.get();
}

const XpertQueryResult& XpertRequestResult::getXpertQueryResult() const
{
    return m_xpertQueryResult;
}

const vector<vector<Core::ParameterValue>>& XpertRequestResult::getParameters() const
{
    return m_parameters;
}

const Core::CycleStats& XpertRequestResult::getCycleStats() const
{
    return m_cycleStats;
}

void XpertRequestResult::setErrorMessage(const std::string& _message)
{
    m_errorMessage = _message;
}

void XpertRequestResult::setDrugModel(const Core::DrugModel* _drugModel)
{
    m_drugModel = _drugModel;
}

void XpertRequestResult::setTimeAfterDoseDurations(std::vector<Duration> _durations)
{
    m_timeAfterDoses = std::move(_durations);
}

void XpertRequestResult::setCovariateResults(vector<CovariateValidationResult>&& _covariateValidationResults)
{
    m_covariateValidationResults = _covariateValidationResults;

    // Sort the covariate validation results by name and date.
    OutputLang lang = m_xpertRequest->getOutputLang();
    sort(m_covariateValidationResults.begin(),
         m_covariateValidationResults.end(),
         [lang](const CovariateValidationResult& _covValRes1, const CovariateValidationResult& _covValRes2) {
             // Try to sort by name.
             std::string covariateName1 = getStringWithEnglishFallback(_covValRes1.getSource()->getName(), lang);
             std::string covariateName2 = getStringWithEnglishFallback(_covValRes2.getSource()->getName(), lang);

             if (covariateName1 != covariateName2) {
                 return covariateName1 < covariateName2;
             }
             // Otherwhise try by date.
             // Normally this if is always true. The only time there is the
             // same name is when the patient has multiple measures.
             // So, this is just a fool guard.
             if (_covValRes1.getPatientCovariate() != nullptr && _covValRes2.getPatientCovariate() != nullptr) {
                 return _covValRes1.getPatientCovariate()->getEventTime()
                        < _covValRes2.getPatientCovariate()->getEventTime();
             }

             // Should never happen.
             return true;
         });
}

void XpertRequestResult::setConfiguration(const std::unique_ptr<Configurations>& _configurations)
{
    m_configuration = _configurations->getConfiguration(m_xpertRequest->getConfigId());

    if (!m_configuration.isValid()) {
        setErrorMessage("Configuration is not valid");
    }
}

void XpertRequestResult::setJustification(const Justification& _justification)
{
    m_justification = _justification;
}

void XpertRequestResult::setDoseResults(map<const Core::DosageBounded*, DoseValidationResult>&& _doseValidationResults)
{
    m_doseValidationResults = _doseValidationResults;
}

void XpertRequestResult::setSampleResults(vector<SampleValidationResult>&& _sampleValidationResults)
{
    m_sampleValidationResults = _sampleValidationResults;

    // Sort the sample results by date and by analyteId.
    sort(m_sampleValidationResults.begin(),
         m_sampleValidationResults.end(),
         [](const SampleValidationResult& _svr1, const SampleValidationResult& _svr2) {
             // Try by date
             if (_svr1.getSource()->getDate() != _svr2.getSource()->getDate()) {
                 return _svr1.getSource()->getDate() < _svr2.getSource()->getDate();
                 // Else try by analyteID.
             }
             return _svr1.getSource()->getAnalyteID() < _svr2.getSource()->getAnalyteID();
         });
}

void XpertRequestResult::setDoseDateResults(std::vector<DoseDateValidationResult>&& _doseDateValidationResults)
{
    m_doseDateValidationResults = _doseDateValidationResults;
}

void XpertRequestResult::setSampleDateResults(std::vector<SampleDateValidationResult>&& _sampleDateValidationResults)
{
    m_sampleDateValidationResults = _sampleDateValidationResults;
}

void XpertRequestResult::setAdjustmentTrait(const Core::ComputingTraitAdjustment& _adjustmentTrait)
{
    m_adjustmentTrait = make_unique<Core::ComputingTraitAdjustment>(_adjustmentTrait);
}

void XpertRequestResult::setAdjustmentData(unique_ptr<Core::AdjustmentData> _adjustmentData)
{
    m_adjustmentData = std::move(_adjustmentData);
}

void XpertRequestResult::setAPrioriPercentiles(unique_ptr<Core::PercentilesData> _aprioriPercentiles)
{
    m_aprioriPercentiles = std::move(_aprioriPercentiles);
}

void XpertRequestResult::setAPosterioriConcentrations(
        std::unique_ptr<Core::SinglePredictionData> _aposterioriConcentrations)
{
    m_aposterioriConcentrations = std::move(_aposterioriConcentrations);
}

void XpertRequestResult::setLastIntake(unique_ptr<Core::IntakeEvent> _lastIntake)
{
    m_lastIntake = std::move(_lastIntake);
}

void XpertRequestResult::addParameters(const vector<Core::ParameterValue>& _parameters)
{
    auto it = m_parameters.begin();
    m_parameters.emplace(it, _parameters);
}

void XpertRequestResult::setCycleStats(Core::CycleStats _cycleStats)
{
    m_cycleStats = std::move(_cycleStats);
}

bool XpertRequestResult::shouldContinueProcessing() const
{
    return m_errorMessage.empty();
}

const Core::CovariateDefinition* XpertRequestResult::addOwnedDefinition(
        std::unique_ptr<Core::CovariateDefinition> _definition)
{
    const Core::CovariateDefinition* raw = _definition.get();
    m_ownedDefinitions.push_back(std::move(_definition));
    return raw;
}

const Core::PatientCovariate* XpertRequestResult::addOwnedPatientCovariate(
        std::unique_ptr<Core::PatientCovariate> _covariate)
{
    const Core::PatientCovariate* raw = _covariate.get();
    m_ownedPatientCovariates.push_back(std::move(_covariate));
    return raw;
}

} // namespace Xpert
} // namespace Tucuxi
