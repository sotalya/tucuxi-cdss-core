// SPDX-License-Identifier: AGPL-3.0-or-later
#include <limits>
#include <map>
#include <vector>

#include "covariatevalidatorandmodelselector.h"

#include "tucucommon/unit.h"
#include "tucucommon/utils.h"

#include "tucucore/covariateevent.h"
#include "tucucore/covariateextractor.h"
#include "tucucore/definitions.h"
#include "tucucore/drugdomainconstraintsevaluator.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"

#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void CovariateValidatorAndModelSelector::perform(XpertRequestResult& _xpertRequestResult)
{
    // Fix the computation time.
    m_computationTime = _xpertRequestResult.getXpertQueryResult().getComputationTime();

    Tucuxi::Common::LoggerHelper logHelper;

    // Check if there is a treatment.
    if (_xpertRequestResult.getTreatment() == nullptr) {
        _xpertRequestResult.setErrorMessage("No treatment set.");
        return;
    }

    // Check the compatibility of the formulations and routes between the doses of the patient.
    bool areFormulationsAndRoutesEqual =
            checkFormulationsAndRoutesCompatibility(_xpertRequestResult.getTreatment()->getDosageHistory());
    if (!areFormulationsAndRoutesEqual) {
        _xpertRequestResult.setErrorMessage("All formulations and routes must be equal.");
        return;
    }

    // Get the drug identifier of the xpertReqest.
    std::string drugId = _xpertRequestResult.getXpertRequest().getDrugId();

    // Get the drug model repository.
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    Tucuxi::Core::IDrugModelRepository* drugModelRepository =
            pCmpMgr->getComponent<Tucuxi::Core::IDrugModelRepository>("DrugModelRepository");

    // Get the drug models that match the drug identifier.
    vector<Core::DrugModel*> drugModels = drugModelRepository->getDrugModelsByDrugId(drugId);

    if (drugModels.empty()) {
        _xpertRequestResult.setErrorMessage(
                "The drug files directory does not contain a drug model for the given drug: " + drugId);
        return;
    }

    // Remember the best drug model.
    unsigned lowestKnownScore = numeric_limits<unsigned>::max(); // Score of the best model known.
    const Core::DrugModel* bestModel = nullptr;                  // Pointer on the best model known.
    vector<CovariateValidationResult>
            covariateValidationResultsOfBestModel; // Covariates validation result for the best model known.

    Core::TreatmentDrugModelCompatibilityChecker drugModelTreatmentCompatiblityChecker;

    // Evaluate each drug model for the given drug identifier.
    for (const Core::DrugModel* drugModel : drugModels) {

        // Does the drug model supports the formulation and route of the treatment?
        if (!drugModelTreatmentCompatiblityChecker.checkCompatibility(_xpertRequestResult.getTreatment(), drugModel)) {
            logHelper.warn(drugModel->getDrugModelId() + " incompatible: Formulations and routes are not matching.");
            continue;
        }

        // Are the drug model constraints respected?
        Common::DateTime start = getOldestCovariateDateTime(_xpertRequestResult.getTreatment()->getCovariates());
        Common::DateTime end = m_computationTime;
        vector<Core::DrugDomainConstraintsEvaluator::EvaluationResult> results;
        Core::DrugDomainConstraintsEvaluator constraintEvaluator;
        Core::DrugDomainConstraintsEvaluator::Result constraintsResult =
                constraintEvaluator.evaluate(*drugModel, *_xpertRequestResult.getTreatment(), start, end, results);

        // If the constraints are not compatible.
        if (constraintsResult == Core::DrugDomainConstraintsEvaluator::Result::Incompatible) {
            logHelper.warn(drugModel->getDrugModelId() + " incompatible: constraints not respected.");
            continue;
        }

        // If the covariates extraction failed.
        if (constraintsResult == Core::DrugDomainConstraintsEvaluator::Result::ComputationError) {
            _xpertRequestResult.setErrorMessage(
                    "Covariates extraction failed for drug model: " + drugModel->getDrugModelId()
                    + ". It may be caused by covariates that could not be converted.");
            return;
        }

        try {
            // Compute the score for this drug model.
            vector<CovariateValidationResult> covariateResults;
            unsigned score = computeScore(
                    _xpertRequestResult.getTreatment()->getCovariates(),
                    drugModel->getCovariates(),
                    _xpertRequestResult.getXpertRequest().getOutputLang(),
                    covariateResults);

            // Compare the score with the score of the best drug model.
            // Better or same but with more covariate definitions.
            if (score < lowestKnownScore
                || ((score == lowestKnownScore) && (bestModel != nullptr)
                    && (bestModel->getCovariates().size() < drugModel->getCovariates().size()))) {

                // The current model become the best model known.
                lowestKnownScore = score;
                bestModel = drugModel;
                covariateValidationResultsOfBestModel = covariateResults;
            }
        }
        catch (const invalid_argument& e) {
            // We catch operation error, then stop drug model search.
            // (Covariate type compatibility already checked when checking contraints).
            // We don't move on to the next model because as soon as the problem is fixed,
            // it can be this model that is the best.
            _xpertRequestResult.setErrorMessage(
                    "Patient covariate error found when handling model " + drugModel->getDrugModelId()
                    + ", details: " + std::string(e.what()));
            return;
        }
    }

    // If a model is found.
    if (lowestKnownScore != numeric_limits<unsigned>::max()) {

        // Check the language compatibility.
        if (checkCovariateDefinitionsSupportedLanguage(
                    bestModel->getCovariates(), _xpertRequestResult.getXpertRequest().getOutputLang())) {

            _xpertRequestResult.setCovariateResults(std::move(covariateValidationResultsOfBestModel));
            _xpertRequestResult.setDrugModel(bestModel);
            // The model does not support the language.
        }
        else {

            _xpertRequestResult.setErrorMessage(
                    "Best drug model found but covariate definitions dont't support requested language.");
        }
        // No model found.
    }
    else {
        _xpertRequestResult.setErrorMessage("No valid drug model found.");
    }
}

bool CovariateValidatorAndModelSelector::checkFormulationsAndRoutesCompatibility(
        const Core::DosageHistory& _dosageHistory) const
{
    // Check that all formulations and routes are equal.
    vector<Core::FormulationAndRoute> queryFormulationsAndRoutes = _dosageHistory.getFormulationAndRouteList();
    for (size_t i = 0; i < queryFormulationsAndRoutes.size(); ++i) {

        // With the exception of the first formulation and route, if not equal to the previous one, they are not all identical.
        if (i == 0 || queryFormulationsAndRoutes[i] == queryFormulationsAndRoutes[i - 1]) {
            continue;
        }
        return false;
    }

    return true;
}

Common::DateTime CovariateValidatorAndModelSelector::getOldestCovariateDateTime(
        const Core::PatientVariates& _patientCovariates) const
{
    Common::DateTime oldestDateTimeKnown = m_computationTime;

    // For each patient's covariate.
    for (const unique_ptr<Core::PatientCovariate>& patientCovariate : _patientCovariates) {
        if (patientCovariate->getEventTime() < oldestDateTimeKnown) {
            oldestDateTimeKnown = patientCovariate->getEventTime();
        }
    }

    return oldestDateTimeKnown;
}

unsigned CovariateValidatorAndModelSelector::computeScore(
        const Core::PatientVariates& _patientCovariates,
        const Core::CovariateDefinitions& _modelDefinitions,
        OutputLang _lang,
        vector<CovariateValidationResult>& _results) const
{
    unsigned score = 0;

    // Map covariate id to patient covariate.
    map<std::string, vector<const Core::PatientCovariate*>> idToPatient;

    // Map covariate id to covariate definition.
    map<std::string, const Core::CovariateDefinition*> idToDefinitionFiltered;

    // Inserting definition in idToDefinition.
    for (const unique_ptr<Core::CovariateDefinition>& covariateDefinition : _modelDefinitions) {

        // Filter computed definition.
        if (!covariateDefinition->isComputed()) {
            idToDefinitionFiltered[covariateDefinition->getId()] = covariateDefinition.get();
        }
    }

    // Retrieving only needed patient covariate
    for (const unique_ptr<Core::PatientCovariate>& patientCovariate : _patientCovariates) {

        // If a covariate definition with the same id exist
        if (idToDefinitionFiltered.count(patientCovariate->getId()) == 1
            || (patientCovariate->getId() == "birthdate" && idToDefinitionFiltered.count("age") == 1)) {
            idToPatient[patientCovariate->getId()].emplace_back(patientCovariate.get());
        }
    }

    // Compute the score.
    // For each covariate definition
    for (auto idToDefinitionIt = idToDefinitionFiltered.begin(); idToDefinitionIt != idToDefinitionFiltered.end();
         ++idToDefinitionIt) {

        std::string covariateId = idToDefinitionIt->first;
        const Core::CovariateDefinition* definition = idToDefinitionIt->second;

        // If the covariate definition is "age".
        if (covariateId == "age") {

            // If the patient has no covariate for this definition.
            if (idToPatient.count("birthdate") == 0) {
                _results.emplace_back(definition, nullptr, "", WarningLevel::NORMAL);
                ++score;
                // There are some covariates for this definition.
            }
            else {
                const auto& birthdateCovariates = idToPatient["birthdate"];
                if (birthdateCovariates.size() > 1) {
                    throw invalid_argument("Multiple birthdate not allowed.");
                }
                const Core::PatientCovariate* birthdateCovariate = birthdateCovariates[0];
                if (birthdateCovariate->getDataType() != Core::DataType::Date) {
                    throw invalid_argument("Invalid data type of birthdate.");
                }

                // Get the age out of patient birthdate covariate.
                double age = getAgeIn(
                        idToDefinitionFiltered["age"]->getType(),
                        birthdateCovariate->getValueAsDate(),
                        m_computationTime);

                // Check the validation.
                if (!checkOperation(age, definition, birthdateCovariate, _lang, _results)) {
                    ++score;
                }
            }

            // If covariate definition is not "age"
        }
        else {

            // If the patient has no covariate for this definition.
            if (idToPatient.count(definition->getId()) == 0) {
                _results.emplace_back(definition, nullptr, "", WarningLevel::NORMAL);
                ++score;
                // There are some covariates for this definition.
            }
            else {
                bool alreadyScored = false;

                // Check each patient's covariate for this definition.
                for (const Core::PatientCovariate* patientCovariate : idToPatient[definition->getId()]) {

                    // Get the value and convert.
                    Core::Value value =
                            Common::Utils::stringToValue(patientCovariate->getValue(), patientCovariate->getDataType());
                    value = Common::UnitManager::convertToUnit(
                            value, patientCovariate->getUnit(), definition->getUnit());

                    if (!checkOperation(value, definition, patientCovariate, _lang, _results) && !alreadyScored) {
                        ++score;
                        alreadyScored = true;
                    }
                }
            }
        }
    }

    return score;
}

bool CovariateValidatorAndModelSelector::checkOperation(
        double _patientConvertedValue,
        const Core::CovariateDefinition* _definition,
        const Core::PatientCovariate* _patient,
        OutputLang _lang,
        vector<CovariateValidationResult>& _results) const
{
    // Get the validation operation.
    Core::Operation* op = _definition->getValidation();
    if (op == nullptr) {
        throw invalid_argument("No validation operation for covariate " + _definition->getId());
    }
    Core::OperationInputList defInputList = op->getInputs();
    Core::OperationInputList inputList;

    // Add the covariate value in the operation input.
    inputList.push_back(Core::OperationInput(defInputList.front().getName(), _patientConvertedValue));

    double result = 0;

    // Evaluate and check if operation could be completed.
    if (!op->evaluate(inputList, result)) {
        throw invalid_argument("Evaluation failed for covariate " + _patient->getId());
    }

    // Get the warning message if the validation is not respected.
    std::string warning =
            (result == 0 ? getStringWithEnglishFallback(_definition->getValidationErrorMessage(), _lang) : "");

    // Add the validation result in the results.
    _results.emplace_back(_definition, _patient, warning, WarningLevel::NORMAL);

    // If validation operation is respected.
    return result == 1;
}

bool CovariateValidatorAndModelSelector::checkCovariateDefinitionsSupportedLanguage(
        const Core::CovariateDefinitions& _modelDefinitions, OutputLang _lang) const
{
    // For each covariate definition
    for (const unique_ptr<Core::CovariateDefinition>& covariateDefinition : _modelDefinitions) {

        // Checking name translation.
        if (covariateDefinition->getName().getString(outputLangToString(OutputLang::ENGLISH)).empty()
            && covariateDefinition->getName().getString(outputLangToString(_lang)).empty()) {
            return false;
        }

        // Checking description translation.
        if (covariateDefinition->getDescription().getString(outputLangToString(OutputLang::ENGLISH)).empty()
            && covariateDefinition->getDescription().getString(outputLangToString(_lang)).empty()) {
            return false;
        }

        // Checking validation error message translation.
        if (covariateDefinition->getValidationErrorMessage().getString(outputLangToString(OutputLang::ENGLISH)).empty()
            && covariateDefinition->getValidationErrorMessage().getString(outputLangToString(_lang)).empty()) {
            return false;
        }
    }

    return true;
}

} // namespace Xpert
} // namespace Tucuxi
