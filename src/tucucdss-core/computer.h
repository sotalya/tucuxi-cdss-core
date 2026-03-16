// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef COMPUTER_H
#define COMPUTER_H

#include <memory>
#include <string>

#include "tucucdss-core/flow/abstract/abstractxpertflowstepprovider.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Enum whose values are used as return value of Computer.
enum class ComputingStatus
{

    ALL_REQUESTS_SUCCEEDED,  /**< For Each TuberXpert request could be fully processed until the report was printed. */
    SOME_REQUESTS_SUCCEEDED, /**< Some TuberXpert request failed at some stage of the process. */
    NO_REQUESTS_SUCCEEDED,   /**< No TuberXpert request could be fully processed. */
    IMPORT_ERROR             /**< The query file could not be loaded. */
};

/// \brief Groups the configuration parameters shared by computeFromFile() and computeFromString().
struct ComputingConfig
{
    std::string drugPath;       /**< Path to the folder containing the drug models. */
    std::string configFileName; /**< Path to the configuration XML file. */
    std::string outputPath;     /**< Path to the output directory. One file is created per successful request. */
    std::string templateName;   /**< Name of the HTML report template. */
    std::string templatePath;   /**< Path to the folder containing the HTML report templates. */
    std::string languagePath;   /**< Path to the folder containing the translation files. */
    std::string language;       /**< Language override (empty = use the query's default). */
    bool jsonDump = false;      /**< Enable JSON dump for debugging. */
};

/// \brief Given the required arguments, this class drives the flow of execution of TuberXpert.
class Computer
{
public:
    /// \brief Entry point of the TuberXpert command Line Interface. This method imports
    ///        the query from a file, loads the translation file, and processes each xpertRequest to finally
    ///        print the reports of the successfully processed requests.
    /// \param _config Computing configuration (paths, language, options).
    /// \param _inputFileName Path to the query file.
    /// \return  A computingStatus that depends on whether the query could be loaded and how much
    ///          requestXpert was successfully processed.
    [[nodiscard]] ComputingStatus computeFromFile(
            const ComputingConfig& _config, const std::string& _inputFileName) const;

    /// \brief This method imports the query from a string, loads the translation file,
    ///        and processes each xpertRequest to finally print the reports of the successfully
    ///        processed requests.
    /// \param _config Computing configuration (paths, language, options).
    /// \param _inputString Xml query string.
    /// \return A computingStatus that depends on whether the query could be loaded and how much
    ///          requestXpert was successfully processed.
    [[nodiscard]] ComputingStatus computeFromString(
            const ComputingConfig& _config, const std::string& _inputString) const;

protected:
    /// \brief For a given xpertRequest in _xpertRequestResult, this method executes the flow steps
    ///        provided by the given AbstractXpertFlowStepProvider.
    /// \param _xpertRequestResult Object containing the xpertRequest and treatment informations. This object will also
    ///                            be filled with the different validations of the system.
    /// \param _config Computing configuration (paths, language, options).
    /// \param _stepProvider Flow step provider responsible to give the each flow step for a given drug.
    void executeFlow(
            XpertRequestResult& _xpertRequestResult,
            const ComputingConfig& _config,
            const AbstractXpertFlowStepProvider& _stepProvider) const;

    /// \brief For a given drug identifier, get the XpertFlowStepProvider that best matches.
    /// \param _drugId Drug identifier to search for the best AbstractXpertFlowStepProvider.
    /// \return Unique pointer with the corresponding XpertFlowStepProvider.
    std::unique_ptr<AbstractXpertFlowStepProvider> getXpertFlowStepProvider(const std::string& _drugId) const;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // COMPUTER_H
