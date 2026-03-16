// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "configuration.h"

#include "tucucdss-core/language/languagemanager.h"

namespace Tucuxi {
namespace Xpert {

void Configurations::addConfiguration(const Configuration& _config)
{
    m_configurations.push_back(_config);
}

Configuration Configurations::getConfiguration(const std::string& _configName) const
{
    for (const auto& config : m_configurations) {

        if (config.getName() == _configName) {

            return config;
        }
    }
    return Configuration();
}



Configuration::Configuration(
        std::string _configName,
        std::string _drugId,
        std::string _drugModelId,
        const std::vector<ConfigCovariate>& _covariates)
    : m_configName(std::move(_configName)), m_drugId(std::move(_drugId)), m_drugModelId(std::move(_drugModelId)),
      m_covariates(_covariates)
{
}

std::string Configuration::getName() const
{
    return m_configName;
}

std::string Configuration::getDrug() const
{
    return m_drugId;
}

std::string Configuration::getDrugModel() const
{
    return m_drugModelId;
}

std::vector<std::string> Configuration::getCovariatesIds() const
{
    std::vector<std::string> covariateIds;
    covariateIds.reserve(m_covariates.size());

    for (const auto& covariate : m_covariates) {
        covariateIds.push_back(covariate.getCovariateId());
    }
    return covariateIds;
}

EvaluationResult Configuration::validateBoolValue(const std::string& _covariateName, std::string const& _val) const
{
    for (const auto& covariate : m_covariates) {
        if (covariate.getCovariateId() == _covariateName) {
            return ((_val == "0") || (_val == "1") || (_val == "true") || (_val == "false"))
                           ? EvaluationResult::InTypicalRange
                           : EvaluationResult::NotInRange;
        }
    }
    return EvaluationResult::CovariateNotExist;
}

EvaluationResult Configuration::validateValue(const std::string& _covariateName, double _val) const
{
    for (const auto& covariate : m_covariates) {
        if (covariate.getCovariateId() == _covariateName) {
            return covariate.evaluateValue(_val);
        }
    }
    return EvaluationResult::CovariateNotExist;
}

bool Configuration::isValid() const
{
    return !m_configName.empty();
}


std::string Configuration::getValidRange(const std::string& _covariateName) const
{
    for (const auto& covariate : m_covariates) {
        if (covariate.getCovariateId() == _covariateName) {
            return covariate.getValidRangeDescription();
        }
    }
    return "";
}

std::string Configuration::getTypicalRange(const std::string& _covariateName) const
{
    for (const auto& covariate : m_covariates) {
        if (covariate.getCovariateId() == _covariateName) {
            return covariate.getTypicalRangeDescription();
        }
    }
    return "";
}

ConfigCovariate::ConfigCovariate(
        std::string _covariateId,
        std::string _covariateType,
        const std::vector<Constraint>& _validRange,
        const std::vector<Constraint>& _typicalRange)
    : m_covariateId(std::move(_covariateId)), m_covariateType(std::move(_covariateType)), m_validRange(_validRange),
      m_typicalRange(_typicalRange)
{
}

EvaluationResult ConfigCovariate::evaluateValue(double _val) const
{
    if (!m_typicalRange.empty() && isInRange(_val, m_typicalRange)) {
        return EvaluationResult::InTypicalRange;
    }
    if (!m_validRange.empty() && isInRange(_val, m_validRange)) {
        return EvaluationResult::InValidRange;
    }
    return EvaluationResult::NotInRange;
}

bool ConfigCovariate::isInRange(double _val, const std::vector<Constraint>& _range) const
{
    return std::all_of(
            _range.begin(), _range.end(), [&](const Constraint& _constraint) { return _constraint.evaluate(_val); });
}

std::string ConfigCovariate::getCovariateId() const
{
    return m_covariateId;
}

std::string ConfigCovariate::getRangeDescription(const std::vector<Constraint>& _range) const
{
    if (_range.empty()) {
        return "";
    }

    std::string description;

    for (const auto& constraint : _range) {
        description += constraint.getDescription() + " " + LanguageManager::getInstance().translate("and") + " ";
    }

    description = description.substr(0, description.size() - 4);

    return description;
}

std::string ConfigCovariate::getValidRangeDescription() const
{
    return getRangeDescription(m_validRange);
}

std::string ConfigCovariate::getTypicalRangeDescription() const
{
    return getRangeDescription(m_typicalRange);
}

Constraint::Constraint(double _v, const std::string& _operatorStr) : m_value(_v), m_operatorStr(_operatorStr)
{
    if (_operatorStr == "gt") {
        m_op = std::greater<double>();
    }
    else if (_operatorStr == "lt") {
        m_op = std::less<double>();
    }
    else if (_operatorStr == "geq") {
        m_op = std::greater_equal<double>();
    }
    else if (_operatorStr == "leq") {
        m_op = std::less_equal<double>();
    }
    else {
        throw std::invalid_argument("Unknown operator");
    }
}

bool Constraint::evaluate(double _val) const
{
    return m_op(_val, m_value);
}

std::string Constraint::getDescription() const
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << m_value;
    std::string opStr = oss.str();

    if (m_operatorStr == "gt") {
        return opStr + " (" + LanguageManager::getInstance().translate("exclude_range") + ")";
    }
    if (m_operatorStr == "lt") {
        return opStr + " (" + LanguageManager::getInstance().translate("exclude_range") + ")";
    }
    if (m_operatorStr == "geq") {
        return opStr + " (" + LanguageManager::getInstance().translate("include_range") + ")";
    }
    if (m_operatorStr == "leq") {
        return opStr + " (" + LanguageManager::getInstance().translate("include_range") + ")";
    }

    return "";
}

} // namespace Xpert
} // namespace Tucuxi
