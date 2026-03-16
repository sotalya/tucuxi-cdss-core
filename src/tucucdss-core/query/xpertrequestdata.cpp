// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include "xpertrequestdata.h"

namespace Tucuxi {
namespace Xpert {

XpertRequestData::XpertRequestData(
        std::string _drugId,
        OutputFormat _outputFormat,
        OutputLang _outputLang,
        Common::DateTime _adjustmentTime,
        LoadingOption _loadingOption,
        RestPeriodOption _restPeriodOption,
        Core::TargetExtractionOption _targetExtractionOption,
        Core::FormulationAndRouteSelectionOption _formulationAndRouteSelectionOption,
        std::string _configId)
    : m_drugId(std::move(_drugId)), m_outputFormat(_outputFormat), m_outputLang(_outputLang),
      m_adjustmentTime(_adjustmentTime), m_loadingOption(_loadingOption), m_restPeriodOption(_restPeriodOption),
      m_targetExtractionOption(_targetExtractionOption),
      m_formulationAndRouteSelectionOption(_formulationAndRouteSelectionOption), m_configId(std::move(_configId))
{
}

std::string XpertRequestData::getDrugId() const
{
    return m_drugId;
}

std::string XpertRequestData::getConfigId() const
{
    return m_configId;
}

OutputFormat XpertRequestData::getOutputFormat() const
{
    return m_outputFormat;
}

OutputLang XpertRequestData::getOutputLang() const
{
    return m_outputLang;
}

Common::DateTime XpertRequestData::getAdjustmentTime() const
{
    return m_adjustmentTime;
}

LoadingOption XpertRequestData::getLoadingOption() const
{
    return m_loadingOption;
}

RestPeriodOption XpertRequestData::getRestPeriodOption() const
{
    return m_restPeriodOption;
}

Core::TargetExtractionOption XpertRequestData::getTargetExtractionOption() const
{
    return m_targetExtractionOption;
}

Core::FormulationAndRouteSelectionOption XpertRequestData::getFormulationAndRouteSelectionOption() const
{
    return m_formulationAndRouteSelectionOption;
}

// bool XpertRequestData::setConfiguration(const ConfigImport& importer) {
//     m_configuration = importer.getConfiguration(m_configId);

//     return m_configuration.isValid();
// }

} // namespace Xpert
} // namespace Tucuxi
