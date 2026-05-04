// SPDX-License-Identifier: AGPL-3.0-or-later
#include <map>

#include "covariatechecker.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelrepository.h"

#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"

namespace Tucuxi {
namespace Xpert {

void CovariateChecker::perform(XpertRequestResult& _xpertRequestResult)
{
    Tucuxi::Common::LoggerHelper logHelper;

    if (_xpertRequestResult.getTreatment() == nullptr) {
        _xpertRequestResult.setErrorMessage("No treatment set.");
        return;
    }

    const auto configuration = _xpertRequestResult.getConfiguration();

    // GET DRUG MODEL OF CONFIGURATION
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    Tucuxi::Core::IDrugModelRepository* drugModelRepository =
            pCmpMgr->getComponent<Tucuxi::Core::IDrugModelRepository>("DrugModelRepository");

    Tucuxi::Core::DrugModel* drugModel = drugModelRepository->getDrugModelById(configuration.getDrugModel());

    if (drugModel == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model found for id: " + configuration.getDrugModel());
        return;
    }

    // Setting model in XpertRequestResult
    _xpertRequestResult.setDrugModel(drugModel);

    // Start checking covariate
    std::vector<CovariateValidationResult> results;
    // Getting patient covariates
    const Core::PatientVariates& patientCovariates = _xpertRequestResult.getTreatment()->getCovariates();

    //Filter Patient covariate.
    std::vector allCovariatesToCheck = configuration.getCovariatesIds();
    std::map<std::string, const Core::PatientCovariate*> idToPatient;
    std::map<std::string, const Core::CovariateDefinition*> idToDefinitionFiltered;

    for (const std::unique_ptr<Core::CovariateDefinition>& covariateDefinition : drugModel->getCovariates()) {
        if (!covariateDefinition->isComputed()) {
            idToDefinitionFiltered[covariateDefinition->getId()] = covariateDefinition.get();
        }
    }

    for (const std::unique_ptr<Core::PatientCovariate>& patientCovariate : patientCovariates) {
        if (patientCovariate->getId() == "birthdate"
            || idToDefinitionFiltered.find(patientCovariate->getId()) != idToDefinitionFiltered.end()) {
            idToPatient[patientCovariate->getId()] = patientCovariate.get();
        }
        else {
            logHelper.warn("Covariate {} not in drug file --- ignoring it", patientCovariate->getId());
        }
    }

    {
        // The age should never be among the covariates directly specified by the
        // TQF --- it should be computed from the birth date in the format
        // requested by the drug model. If the age is specified, raise an error
        // to the user.
        // The age in the TQF could have any name, as it is the covariateType of
        // the drug model covariate that is defining that it is an age covariate.
        // To spot ages in the TQF file we check if there is a corresponding
        // drugmodel covariate with the same name that is an ageInX and, if so,
        // generate the error.
        static std::vector<Core::CovariateType> const INVALID_AGE_COVARIATE_TYPES = {
                Core::CovariateType::AgeInDays,
                Core::CovariateType::AgeInWeeks,
                Core::CovariateType::AgeInMonths,
                Core::CovariateType::AgeInYears,
        };
        for (const std::unique_ptr<Core::CovariateDefinition>& covariateDefinition : drugModel->getCovariates()) {
            if (std::find(
                        INVALID_AGE_COVARIATE_TYPES.begin(),
                        INVALID_AGE_COVARIATE_TYPES.end(),
                        covariateDefinition->getType())
                != INVALID_AGE_COVARIATE_TYPES.end()) {
                // We have found a covariate in the drug file that has the "age
                // type". We check that no patient covariate has the same name.
                auto const covId = covariateDefinition->getId();
                for (const std::unique_ptr<Core::PatientCovariate>& patientCovariate : patientCovariates) {
                    if (patientCovariate->getId() == covId) {
                        _xpertRequestResult.setErrorMessage("Age specified in the TQF --- must use birth date instead");
                        return;
                    }
                }
            }
        }
    }

    // Here we know there cannot be an "age" patient covariate

    // If we have the birth date in the TQF, convert its name to "age" (will be
    // dealt with down below, including the conversion to the required type).
    if (idToPatient.find("birthdate") != idToPatient.end()) {
        // Extract and replace birthdate key by age
        auto nodeHandler = idToPatient.extract("birthdate");
        nodeHandler.key() = "age";
        idToPatient.insert(std::move(nodeHandler));
    }

    for (auto& idToPatientIt : idToPatient) {
        std::string covariateId = idToPatientIt.first;
        const Core::CovariateDefinition* definition = nullptr;
        bool useToCompute = false;

        if (idToDefinitionFiltered.count(covariateId) > 0) {
            definition = idToDefinitionFiltered[covariateId];
            useToCompute = true;
        }
        else {
            // The only way to get here is having a "birthdate" patient covariate and so an "age" in idToPatient,
            // which happens if there is a "birthdate" patient covariate.
            // This is done due to type check later, we need to specify a "AgeInXX" for the age covar.
            //    if it's not done it will throw an exception.

            // The following is kept in case, but not used reachable in the current flow
            // if (covariateId != "age") {
            //     definition = _xpertRequestResult.addOwnedDefinition(std::make_unique<Core::CovariateDefinition>(
            //             covariateId,
            //             idToPatientIt.second->getValue(),
            //             nullptr,
            //             Core::CovariateType::Standard,
            //             idToPatientIt.second->getDataType(),
            //             Tucuxi::Common::TranslatableString(covariateId)));
            // }
            // else {
            definition = _xpertRequestResult.addOwnedDefinition(std::make_unique<Core::CovariateDefinition>(
                    covariateId,
                    idToPatientIt.second->getValue(),
                    nullptr,
                    Core::CovariateType::AgeInYears,
                    idToPatientIt.second->getDataType(),
                    Tucuxi::Common::TranslatableString(covariateId)));
            // }
        }

        if (useToCompute) {
            if (covariateId == "age") {
                if (idToPatientIt.second->getDataType() != Core::DataType::Date) {
                    throw std::invalid_argument("Invalid data type of birthdate.");
                }

                Common::DateTime computationTime = _xpertRequestResult.getXpertQueryResult().getComputationTime();

                double age = getAgeIn(
                        idToDefinitionFiltered[covariateId]->getType(),
                        idToPatientIt.second->getValueAsDate(),
                        computationTime);

                EvaluationResult ret = configuration.validateValue(covariateId, age);
                std::string warning = getWarningMessage(ret, covariateId, configuration);
                results.emplace_back(definition, idToPatientIt.second, warning, WarningLevel::CRITICAL);
            }
            else {
                if (idToPatientIt.second->getValue().empty()) {
                    auto it = idToDefinitionFiltered.find(covariateId);

                    if (it != idToDefinitionFiltered.end()) {
                        std::string warning = getWarningMessage(EvaluationResult::IsEmpty, covariateId, configuration);
                        std::string value = std::to_string(idToDefinitionFiltered.at(covariateId)->getValue());
                        Common::DateTime datetime;
                        const Core::PatientCovariate* defaultCovariate =
                                _xpertRequestResult.addOwnedPatientCovariate(std::make_unique<Core::PatientCovariate>(
                                        covariateId,
                                        value,
                                        idToDefinitionFiltered.at(covariateId)->getDataType(),
                                        idToDefinitionFiltered.at(covariateId)->getUnit(),
                                        DateTime::now()));
                        results.emplace_back(definition, defaultCovariate, warning, WarningLevel::CRITICAL);
                    }
                    // else {
                    // Temporary not used because tucuxi throw errors when unused covariate with no value is given.
                    // }
                }
                else {
                    EvaluationResult ret;
                    try {
                        if (idToPatientIt.second->getDataType() != Core::DataType::Bool) {
                            Core::Value value = Common::Utils::stringToValue(
                                    idToPatientIt.second->getValue(), idToPatientIt.second->getDataType());
                            value = Common::UnitManager::convertToUnit(
                                    value, idToPatientIt.second->getUnit(), definition->getUnit());

                            ret = configuration.validateValue(covariateId, value);
                        }
                        else {
                            ret = configuration.validateBoolValue(covariateId, idToPatientIt.second->getValue());
                        }
                    }
                    catch (std::invalid_argument const& e) {
                        throw std::invalid_argument(
                                std::string("issue in extracting the value for covariate ") + covariateId);
                    }
                    std::string warning = getWarningMessage(ret, covariateId, configuration);
                    results.emplace_back(definition, idToPatientIt.second, warning, WarningLevel::CRITICAL);
                }
            }
        }
        else {
            if (covariateId == "age") {
                if (idToPatientIt.second->getDataType() != Core::DataType::Date) {
                    throw std::invalid_argument("Invalid data type of birthdate.");
                }

                std::string warning;
                results.emplace_back(definition, idToPatientIt.second, warning, WarningLevel::NORMAL);
            }
            // In the current flow, the next lines can never be reached
            // else {
            //     string warning;
            //     results.emplace_back(definition, idToPatientIt.second, warning, WarningLevel::NORMAL);
            // }
        }
    }

    for (auto& idToDefinitionIt : idToDefinitionFiltered) {
        const std::string& covariateId = idToDefinitionIt.first;

        if (idToPatient.count(covariateId) == 0) {
            const Core::CovariateDefinition* definition = idToDefinitionIt.second;

            std::string warning = LanguageManager::getInstance().translate("no_covar_given");
            results.emplace_back(definition, nullptr, warning, WarningLevel::CRITICAL);
        }
    }
    _xpertRequestResult.setCovariateResults(std::move(results));
}

std::string CovariateChecker::getWarningMessage(
        EvaluationResult _ret, const std::string& _covariateId, const Configuration& _configuration)
{
    switch (_ret) {
    case EvaluationResult::NotInRange:
        return LanguageManager::getInstance().translate("error") + ": "
               + LanguageManager::getInstance().translate("invalid_range") + " "
               + _configuration.getValidRange(_covariateId);

    case EvaluationResult::InValidRange:
        return LanguageManager::getInstance().translate("warning") + ": "
               + LanguageManager::getInstance().translate("invalid_range") + " "
               + _configuration.getTypicalRange(_covariateId);

    case Tucuxi::Xpert::EvaluationResult::IsEmpty:
        return LanguageManager::getInstance().translate("error") + ": "
               + LanguageManager::getInstance().translate("empty_covar_value");

    case EvaluationResult::InTypicalRange:
        return "";

    default:
        return "Unknown validation result.";
    }
}

} // namespace Xpert
} // namespace Tucuxi
