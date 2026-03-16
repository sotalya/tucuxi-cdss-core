// SPDX-License-Identifier: AGPL-3.0-or-later
#include <fstream>

#include "computer.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/overloadevaluator.h"

#include "tucucdss-core/flow/general/generalxpertflowstepprovider.h"
#include "tucucdss-core/language/languagemanager.h"
#include "tucucdss-core/query/configimport.h"
#include "tucucdss-core/query/xpertquerydata.h"
#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

ComputingStatus Computer::computeFromFile(const ComputingConfig& _config, const std::string& _inputFileName) const
{
    Common::LoggerHelper logHelper;

    // Read the file and extract its content.
    ifstream inputFile(_inputFileName);
    if (!inputFile.is_open()) {
        logHelper.error("Failed to open query file.");
        return ComputingStatus::IMPORT_ERROR;
    }

    std::string inputFileContent =
            std::string((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    // Extract from string.
    return computeFromString(_config, inputFileContent);
}

ComputingStatus Computer::computeFromString(const ComputingConfig& _config, const std::string& _inputString) const
{

    Common::LoggerHelper logHelper;

    // Drug models repository creation.
    Common::ComponentManager* pCmpMgr = Common::ComponentManager::getInstance();

    auto drugModelRepository = std::unique_ptr<Core::DrugModelRepository>(
            dynamic_cast<Core::DrugModelRepository*>(Core::DrugModelRepository::createComponent()));

    pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository.get());

    drugModelRepository->addFolderPath(_config.drugPath);

    /*********************************************************************************
     *                               Query importation                               *
     * *******************************************************************************/

    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, _inputString);

    if (importResult != XpertQueryImport::Status::Ok) {

        logHelper.error("Query import error, see details : {}", importer.getErrorMessage());
        return ComputingStatus::IMPORT_ERROR;
    }

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ComputationLimits computationLimits;
    ConfigImport configImporter;
    ConfigImport::Status importConfigStatus =
            configImporter.importFromFile(_config.configFileName, *configurations, computationLimits);

    if (importConfigStatus != ConfigImport::Status::Ok) {
        logHelper.error("Config import error, see details : {}", configImporter.getErrorMessage());
        return ComputingStatus::IMPORT_ERROR;
    }

    Tucuxi::Core::SingleOverloadEvaluator::getInstance()->setValues(
            computationLimits.getPredPointsNo(),
            computationLimits.getPctPointsNo(),
            computationLimits.getDosagePossibilitiesNo());

    XpertQueryResult xpertQueryResult(
            std::move(query), _config.outputPath, _config.templateName, _config.templatePath, _config.jsonDump);

    /*********************************************************************************
     *                             For each xpert request                            *
     * *******************************************************************************/

    unsigned nbUnfulfilledRequest = 0;
    for (XpertRequestResult& xpertRequestResult : xpertQueryResult.getXpertRequestResults()) {

        logHelper.info("---------------------------------------");
        logHelper.info(
                "Processing request number: "
                + std::to_string(
                        xpertQueryResult.incrementRequestIndexBeingProcessed() + 1)); // +1 because it starts from 0


        xpertRequestResult.setConfiguration(configurations);
        if (!xpertRequestResult.shouldContinueProcessing()) {
            logHelper.error(xpertRequestResult.getErrorMessage());
            ++nbUnfulfilledRequest;
            continue;
        }


        // Get the XpertFlowStepProvider for the drug of the request.
        unique_ptr<AbstractXpertFlowStepProvider> xpertFlowStepProvider =
                getXpertFlowStepProvider(xpertRequestResult.getXpertRequest().getDrugId());

        // Execute each step provided by the selected XpertFlowStepProvider.
        executeFlow(xpertRequestResult, _config, *xpertFlowStepProvider);
        if (!xpertRequestResult.shouldContinueProcessing()) {
            logHelper.error(xpertRequestResult.getErrorMessage());
            ++nbUnfulfilledRequest;
            continue;
        }

        // Here, the request has been fully processed without any problems.
    }

    pCmpMgr->unregisterComponent("DrugModelRepository");

    // If each request could be fully processed.
    if (nbUnfulfilledRequest == 0) {
        return ComputingStatus::ALL_REQUESTS_SUCCEEDED;
    }
    // If some requests failed.
    if (nbUnfulfilledRequest < xpertQueryResult.getXpertRequestResults().size()) {
        return ComputingStatus::SOME_REQUESTS_SUCCEEDED;
    }
    // Else, if they all failed.
    return ComputingStatus::NO_REQUESTS_SUCCEEDED;
}

void Computer::executeFlow(
        XpertRequestResult& _xpertRequestResult,
        const ComputingConfig& _config,
        const AbstractXpertFlowStepProvider& _stepProvider) const
{

    Common::LoggerHelper logHelper;

    /**************************************************************
     *                      Check extraction                      *
     * ************************************************************/
    logHelper.info("Checking extraction state...");

    if (!_xpertRequestResult.shouldContinueProcessing()) {
        return;
    }

    logHelper.info("Extraction succeed.");

    /**************************************************************
     *                Loading the translations file                *
     * ************************************************************/
    logHelper.info("Loading translation file...");

    // Getting language manager.
    LanguageManager& langMgr = LanguageManager::getInstance();

    try {
        std::string translationsFileName;
        if (_config.language != "") {
            translationsFileName = _config.languagePath + DIR_SEPARATOR + _config.language + ".xml";
        }
        else {
            translationsFileName = _config.languagePath + DIR_SEPARATOR
                                   + outputLangToString(_xpertRequestResult.getXpertRequest().getOutputLang()) + ".xml";
        }
        ifstream ifStream(translationsFileName);

        // If the opening of the translation file failed.
        if (ifStream.fail()) {
            throw runtime_error("Could not open the translation file " + translationsFileName);
        }

        // Try loading the translation file. It may throw a LanguageException.
        std::string xmlLanguageString((istreambuf_iterator<char>(ifStream)), (istreambuf_iterator<char>()));
        langMgr.loadTranslations(xmlLanguageString);

        logHelper.info("Successfully loaded " + translationsFileName);
    }
    catch (const runtime_error& e) {

        // Somehow, the loading of the translation file failed.
        // Leave this xpertRequest and try the next one.
        _xpertRequestResult.setErrorMessage(e.what());
        return;
    }

    /**************************************************************
     *        Drug model selection and covariates validation      *
     * ************************************************************/
    logHelper.info("Validating covariates and selecting drug model...");

    try {
        _stepProvider.getCovariateChecker()->perform(_xpertRequestResult);
    }
    catch (std::invalid_argument const& e) {
        logHelper.error(std::string("Error encountered while parsing the covariates: ") + e.what());
        return;
    }

    // _stepProvider.getCovariateValidatorAndModelSelector()->perform(_xpertRequestResult);

    // Check if the model selection was successful.
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        return;
    }

    logHelper.info(
            "Covariates validated and drug model selected: " + _xpertRequestResult.getDrugModel()->getDrugModelId());

    /**************************************************************
     *                        Doses validation                    *
     * ************************************************************/
    logHelper.info("Validating doses...");

    _stepProvider.getDoseValidator()->perform(_xpertRequestResult);

    // Check if the doses validation was successful.
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        return;
    }

    logHelper.info("Dosages successfully validated.");

    /**************************************************************
     *                       Samples validation                   *
     * ************************************************************/
    logHelper.info("Validating samples...");

    _stepProvider.getSampleValidator()->perform(_xpertRequestResult);

    // Check if the samples validation was successful.
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        logHelper.warn(_xpertRequestResult.getErrorMessage());
        // return;
    }
    else {
        logHelper.info("Samples successfully validated.");
    }


    /**************************************************************
     *                       Dates validation                     *
     * ************************************************************/
    logHelper.info("Validating dates...");

    _stepProvider.getDateValidator()->perform(_xpertRequestResult);

    // Check if the dates validation was successful.
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        logHelper.warn(_xpertRequestResult.getErrorMessage());
        // return;
    }
    else {
        logHelper.info("Dates successfully validated.");
    }


    /**************************************************************
     *                      Targets validation                    *
     * ************************************************************/
    logHelper.info("Validating targets...");

    _stepProvider.getTargetValidator()->perform(_xpertRequestResult);

    // Check if targets validation was successful.
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        logHelper.warn(_xpertRequestResult.getErrorMessage());
        // return;
    }
    else {
        logHelper.info("Targets successfully validated.");
    }



    /**************************************************************
     *                  Creating adjustment trait                 *
     * ************************************************************/
    logHelper.info("Creating adjustment trait...");

    _stepProvider.getAdjustmentTraitCreator()->perform(_xpertRequestResult);

    // Check if the adjustment trait creation was successful.
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        logHelper.warn(_xpertRequestResult.getErrorMessage());
        // return;
    }
    else {
        logHelper.info("Adjustment trait successfully created.");
    }


    /**************************************************************
     *                     Requests submission                    *
     * ************************************************************/
    logHelper.info("Submission of the adjustment request...");

    // Check if the submission of the adjustment request was successful.
    _stepProvider.getRequestExecutor()->perform(_xpertRequestResult);
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        logHelper.warn("No adjustment found");
        // return;
    }
    else {
        logHelper.info("Adjustment request submission succeed.");
    }

    /**************************************************************
     *                     TDA Calculation                        *
     * ************************************************************/
    logHelper.info("Calculation of the TDA..");
    // This step is in a try-catch block because this is where we catch potential
    // issues with doses.
    try {
        _stepProvider.getTDAExporter()->perform(_xpertRequestResult);
    }
    catch (const std::exception& e) {
        return;
    }

    /**************************************************************
     *                     Pre-adjustment data collection         *
     * ************************************************************/
    logHelper.info("Calculation of pre-adjustment data...");
    _stepProvider.getPADCollector()->perform(_xpertRequestResult);

    /**************************************************************
     *                     Justification Creation                 *
     * ************************************************************/
    logHelper.info("Creation of justification sentence..");
    _stepProvider.getJustificationCreator()->perform(_xpertRequestResult);

    /**************************************************************
     *                      Report generation                     *
     * ************************************************************/
    logHelper.info("Generating report...");

    // Check if the report generation was successful.
    _stepProvider.getReportPrinter()->perform(_xpertRequestResult);
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        logHelper.warn(_xpertRequestResult.getErrorMessage());
        // return;
    }
    else {
        logHelper.info("Report generation succeed.");
    }


    // At this point, the XpertRequestResult has not received
    // any errors during the execution of the flow steps.
}

unique_ptr<AbstractXpertFlowStepProvider> Computer::getXpertFlowStepProvider(const std::string& _drugId) const
{
    TMP_UNUSED_PARAMETER(_drugId);
    // The idea is the have this method return a XpertFlowStepProvider for each
    // drug. If a drug has no specialized XpertFlowStepProvider, the GeneralXpertFlowStepProvider
    // is returned. For example, like this:

    // if (_drugId == "imatinib") {
    //     _xpertFlowStepProvider = make_unique<XpertFlow::ImatinibXpertFlowStepProvider>();
    // } else if (_drugId == "rifampicin") {
    //     _xpertFlowStepProvider = make_unique<XpertFlow::RifampicinXpertFlowStepProvider>();
    // } else {
    //     _xpertFlowStepProvider = make_unique<XpertFlow::GeneralXpertFlowStepProvider>();
    // }


    // But for now, TuberXpert is implemented in a general way.
    // In future steps, this line should be removed for those commented above.
    return make_unique<GeneralXpertFlowStepProvider>();
}

} // namespace Xpert
} // namespace Tucuxi
