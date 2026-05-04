// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>

#include "tucucommon/loggerhelper.h"
#include "tucucommon/utils.h"

#include "tucucdss-core/computer.h"
#include "tucuxi-core/libs/cxxopts/include/cxxopts.hpp"

/*********************************************************************************
 *                                 Result Codes                                  *
 * *******************************************************************************/

/// \brief CODE_EXECUTION_SUCCESS Exit code when everything went fine.
const int CODE_ALL_REQUESTS_SUCCEEDED = 0;

/// \brief CODE_SOME_REQUESTS_SUCCEEDED Exit code when some of the requests could not be fulfilled.
const int CODE_SOME_REQUESTS_SUCCEEDED = 1;

/// \brief CODE_NO_REQUESTS_SUCCEEDED Exit code when all the requests could not be fulfilled.
const int CODE_NO_REQUESTS_SUCCEEDED = 2;

/// \brief CODE_BAD_ARGUMENTS_ERROR Exit code when an error is present in the arguments.
const int CODE_BAD_ARGUMENTS_ERROR = 3;

/// \brief CODE_IMPORT_ERROR Exit code when the query could not be loaded.
const int CODE_IMPORT_ERROR = 4;

enum class ParseResult
{
    ContinueProcessing,
    ExitNormally,
    BadArguments,
};

/// \brief Parses the program arguments argc/argv into a ComputingConfig and an input file name.
/// \param _argc Program argument count.
/// \param _argv Program arguments.
/// \param _config ComputingConfig to populate (languagePath and templatePath should be pre-filled with defaults).
/// \param _inputFileName String value to store parsed input file path.
/// \return ParseResult describing whether processing should continue, exit normally, or fail with bad arguments.
ParseResult parse(
        int _argc,
        char* _argv[], // NOLINT(cppcoreguidelines-avoid-c-arrays)
        Tucuxi::Xpert::ComputingConfig& _config,
        std::string& _inputFileName)
{
    Tucuxi::Common::LoggerHelper logHelper;

    try {

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        cxxopts::Options options(_argv[0], " - Tucuxi-CDSS command line");
        options.positional_help("[optional args]").show_positional_help();

        options.allow_unrecognised_options().add_options()(
                "d,drugpath", "Path of the directory containing drug files", cxxopts::value<std::string>())(
                "i,input", "Path of the input request file", cxxopts::value<std::string>())(
                "c,config",
                "Path of the XML configuration file (default: ./config.xml)",
                cxxopts::value<std::string>())(
                "o,outputpath",
                "Path of the directory where the output report will be created",
                cxxopts::value<std::string>())(
                "l,languagepath",
                "(optional) Path of the directory containing translation files",
                cxxopts::value<std::string>())(
                "g,language",
                "(optional) Desired language for output, requires the language directory",
                cxxopts::value<std::string>())(
                "t,templatename", "Name of the HTML template", cxxopts::value<std::string>())(
                "p,templatepath",
                "Path of the directory containing HTML template files",
                cxxopts::value<std::string>())(
                "j,jsondump",
                "(optional) Enable json dump for debug (in output folder)",
                cxxopts::value<bool>()->default_value("false"))("help", "Print this help message");

        auto result = options.parse(_argc, _argv);

        if (result.count("help") > 0) {
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::ExitNormally;
        }

        _config.jsonDump = result["jsondump"].as<bool>();

        if (result.count("drugpath") > 0) {
            _config.drugPath = result["drugpath"].as<std::string>();
        }
        else {
            std::cout << "The drug model files directory is mandatory\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }
        if (!std::filesystem::exists(_config.drugPath)) {
            std::cout << "The drug model files directory: " << _config.drugPath << " is missing\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }

        if (result.count("input") > 0) {
            _inputFileName = result["input"].as<std::string>();
        }
        else {
            std::cout << "The input query file is mandatory\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }
        if (!std::filesystem::exists(_inputFileName)) {
            std::cout << "The specified input query file: " << _inputFileName << " is missing\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }

        if (result.count("config") > 0) {
            _config.configFileName = result["config"].as<std::string>();
        }
        else {
            std::cout << "Configuration file not specified --- "
                      << "set by default to ./config.xml\n\n";
            _config.configFileName = "./config.xml";
        }
        if (!std::filesystem::exists(_config.configFileName)) {
            std::cout << "The configuration file: " << _config.configFileName << " is missing\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }

        if (result.count("outputpath") > 0) {
            _config.outputPath = result["outputpath"].as<std::string>();
        }
        else {
            std::cout << "The output directory path is mandatory\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }
        if (!std::filesystem::exists(_config.outputPath)) {
            std::cout << "The output directory: " << _config.outputPath
                      << " is not existent, it will be created automatically"
                      << "\n\n";
            std::filesystem::create_directory(_config.outputPath);
        }

        if (result.count("templatepath") > 0) {
            _config.templatePath = result["templatepath"].as<std::string>();
        }
        else {
            std::cout << "The HTML template directory has not been specified --- "
                      << "defaulting to " << _config.templatePath << "\n\n";
        }
        if (!std::filesystem::exists(_config.templatePath)) {
            std::cout << "The  HTML template directory: " << _config.templatePath << " is missing\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }

        if (result.count("templatename") > 0) {
            _config.templateName = result["templatename"].as<std::string>();
        }
        else {
            std::cout << "The HTML template name is mandatory\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }
        std::filesystem::path templatePath =
                std::filesystem::path(_config.templatePath) / std::filesystem::path(_config.templateName);
        if (!std::filesystem::exists(templatePath)) {
            std::cout << "The selected HTML template: " << _config.templateName << " is missing\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }
        if (result.count("languagepath") > 0) {
            _config.languagePath = result["languagepath"].as<std::string>();
        }
        else {
            std::cout << "The language files directory has not been specified ---  defaulting to "
                      << _config.languagePath << "\n\n";
        }

        if (result.count("language") > 0) {
            std::string desiredLanguage = result["language"].as<std::string>();

            std::transform(
                    desiredLanguage.begin(), desiredLanguage.end(), desiredLanguage.begin(), [](unsigned char _c) {
                        return std::tolower(_c);
                    });

            if (desiredLanguage.empty()) {
                std::cout << "The desired language is empty\n\n" << options.help({"", "Group"}) << "\n";
                return ParseResult::BadArguments;
            }

            _config.language = desiredLanguage;
        }

        if (!std::filesystem::exists(_config.languagePath)) {
            std::cout << "The language files directory: " << _config.languagePath << " is missing\n\n";
            std::cout << options.help({"", "Group"}) << "\n";
            return ParseResult::BadArguments;
        }

        logHelper.info("Drugs directory : {}", _config.drugPath);
        logHelper.info("Input file : {}", _inputFileName);
        logHelper.info("Config file : {}", _config.configFileName);
        logHelper.info("Output directory : {}", _config.outputPath);
        logHelper.info("Template directory : {}", _config.templatePath);
        logHelper.info("Template name : {}", _config.templateName);
        logHelper.info("Language directory : {}", _config.languagePath);

        if (_config.language.empty()) {
            logHelper.info("Language : using tqf file setting.");
        }
        else {
            logHelper.info("Language : {}", _config.language);
        }

        if (_config.jsonDump) {
            logHelper.info("Json dump enabled in output directory");
        }

        return ParseResult::ContinueProcessing;
    }
    catch (const cxxopts::exceptions::exception& e) {
        logHelper.error("error parsing options: {}", e.what());

        return ParseResult::BadArguments;
    }
}


/// \brief The tucuxi-cdss-cli console application.
/// The tucuxi-cdss-cli console application offers a simple command line interface to
/// launch the computation.
int main(int argc, char** argv)
{
    // Parsing program arguments
    Tucuxi::Xpert::ComputingConfig config;
    config.languagePath = std::string(TUCU_SOURCE_DIR) + "/language";
    config.templatePath = std::string(TUCU_BUILD_DIR) + "/html_template";

    std::string inputFileName;
    ParseResult parseResult = parse(argc, argv, config, inputFileName);
    if (parseResult == ParseResult::ExitNormally) {
        return CODE_ALL_REQUESTS_SUCCEEDED;
    }
    if (parseResult == ParseResult::BadArguments) {
        return CODE_BAD_ARGUMENTS_ERROR;
    }

    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);
    // Init logger
    Tucuxi::Common::LoggerHelper::init(appFolder + "/tuberxpert.log");
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.info("********************************************************");
    logHelper.info("Tucuxi-cdss-cli console application is starting up...");

    // Computation start
    Tucuxi::Xpert::Computer xpertComputer;
    Tucuxi::Xpert::ComputingStatus result = xpertComputer.computeFromFile(config, inputFileName);

    logHelper.info("Tucuxi-cdss-cli console application is exiting...");
    logHelper.info("********************************************************");

    // Clean logger
    Tucuxi::Common::LoggerHelper::beforeExit();

    // Return code handling
    switch (result) {
    case Tucuxi::Xpert::ComputingStatus::IMPORT_ERROR:
        return CODE_IMPORT_ERROR;
    case Tucuxi::Xpert::ComputingStatus::ALL_REQUESTS_SUCCEEDED:
        return CODE_ALL_REQUESTS_SUCCEEDED;
    case Tucuxi::Xpert::ComputingStatus::SOME_REQUESTS_SUCCEEDED:
        return CODE_SOME_REQUESTS_SUCCEEDED;
    case Tucuxi::Xpert::ComputingStatus::NO_REQUESTS_SUCCEEDED:
        return CODE_NO_REQUESTS_SUCCEEDED;
    }
}
