// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef XPERTREQUESTDATA_H
#define XPERTREQUESTDATA_H

#include <string>

#include "tucucommon/datetime.h"

#include "tucucore/computingservice/computingtrait.h"

namespace Tucuxi {
namespace Xpert {

/// \brief List of supported languages.
enum class OutputLang
{
    ENGLISH = 0, /**< For English language. */
    FRENCH       /**< For French language. */
};

/// \brief List of the supported formats.
enum class OutputFormat
{
    XML = 0, /**< For a report in xml. */
    HTML,    /**< For a report in html. */
    PDF      /**< For a report in pdf. */
};

/// \brief This enum authorizes or not a loading dose to reach quickly the steady state.
///        It is the same as in computingTrait.h with an added value "unspecified".
///        This last value simply means:
///        "I don't know what to fill, so check and follow the recommendations of the drug model".
enum class LoadingOption
{
    NoLoadingDose = 0,  /**< No loading dose is allowed. */
    LoadingDoseAllowed, /**< A loading dose may be offered if necessary. */
    Unspecified         /**< Use the recommendations of the drug model. */
};

///
/// \brief This enum authorizes or not a rest period to reach quickly the steady state.
///        It is the same as in computingTrait.h with an added value "unspecified".
///        This last value simply means:
///        "I don't know what to fill, so check and follow the recommendations of the drug model".
enum class RestPeriodOption
{
    NoRestPeriod = 0,  /**< No rest period is allowed. */
    RestPeriodAllowed, /**< A rest period may be offered if necessary. */
    Unspecified        /**< Use the recommendations of the drug model. */
};


/// \brief Class that represents the custom request of TuberXpert.
///        It is the representation of the "xpertRequest" element
///        from an xml query file.
class XpertRequestData
{
public:
    /// \brief XpertRequestData constructor.
    /// \param _drugId Identifier of the drug to adjust.
    /// \param _outputFormat Report output format.
    /// \param _outputLang Report output language.
    /// \param _adjustmentTime Time at which the adjustment takes place.
    /// \param _loadingOption Define whether a loading dose can be offered.
    /// \param _restPeriodOption Define whether a rest period can be offered.
    /// \param _targetExtractionOption Define the target extraction option.
    /// \param _formulationAndRouteSelectionOption Define the selection of the formulation and route option
    /// \param _configId Identifier of the configuration
    XpertRequestData(
            std::string _drugId,
            OutputFormat _outputFormat,
            OutputLang _outputLang,
            Common::DateTime _adjustmentTime,
            LoadingOption _loadingOption,
            RestPeriodOption _restPeriodOption,
            Core::TargetExtractionOption _targetExtractionOption,
            Core::FormulationAndRouteSelectionOption _formulationAndRouteSelectionOption,
            std::string _configId);


    // Getters

    /// \brief Get the identifier of the drug to adjust.
    /// \return The drug identifier.
    [[nodiscard]] std::string getDrugId() const;

    /// \brief Get the identifier of configuration.
    /// \return The configuration identifier
    [[nodiscard]] std::string getConfigId() const;

    /// \brief Get the output format of the report.
    /// \return The output format of the report.
    [[nodiscard]] OutputFormat getOutputFormat() const;

    /// \brief Get the output language of the report.
    /// \return The output language of the report.
    [[nodiscard]] OutputLang getOutputLang() const;

    /// \brief Get the time of the adjustment.
    /// \return The time of the adjustment.
    [[nodiscard]] Common::DateTime getAdjustmentTime() const;

    /// \brief Get the option about a potential loading dose.
    /// \return The loading dose option.
    [[nodiscard]] LoadingOption getLoadingOption() const;

    /// \brief Get the option about a potential rest period.
    /// \return The rest period option.
    [[nodiscard]] RestPeriodOption getRestPeriodOption() const;

    /// \brief Get the target extraction option.
    /// \return The target extraction option.
    [[nodiscard]] Core::TargetExtractionOption getTargetExtractionOption() const;

    /// \brief Get the formulation and route selection option.
    /// \return The formulation and route selection option.
    [[nodiscard]] Core::FormulationAndRouteSelectionOption getFormulationAndRouteSelectionOption() const;


    // [[nodiscard]] bool setConfiguration(const ConfigImport& importer);
protected:
    /// \brief Identifier of the drug that TuberXpert must use.
    std::string m_drugId;

    /// \brief Output format of the report.
    OutputFormat m_outputFormat;

    /// \brief Output language of the report.
    OutputLang m_outputLang;

    /// \brief Time of the adjustment.
    Common::DateTime m_adjustmentTime;

    /// \brief Is the loading dose applicable?
    LoadingOption m_loadingOption;

    /// \brief Is the rest period applicable?
    RestPeriodOption m_restPeriodOption;

    /// \brief Target extraction option.
    Core::TargetExtractionOption m_targetExtractionOption;

    /// \brief What formulation and route should be used to generate candidates.
    Core::FormulationAndRouteSelectionOption m_formulationAndRouteSelectionOption;

    /// \brief Identifier of the configuration
    std::string m_configId;

    // Configuration m_configuration;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // XPERTREQUESTDATA_H
