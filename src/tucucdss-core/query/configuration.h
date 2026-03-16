// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace Tucuxi {
namespace Xpert {

enum class EvaluationResult
{
    NotInRange = 0,
    InValidRange = 1,
    InTypicalRange = 2,
    IsEmpty = 3,
    CovariateNotExist = -1
};

class Constraint
{
public:
    ///
    /// \brief Constraint
    /// Constructs a Constraint object with a given value and comparison operation string.
    ///     Comparaison operation could be only gt, lt, geq or leq.
    /// \param v The value of the constraint.
    /// \param operatorStr A string representing the comparison operation.
    ///
    Constraint(double _v, const std::string& _operatorStr);

    ///
    /// \brief evaluate
    /// Evaluates the constraint against a given value.
    /// \param val The value to be evaluated.
    /// \return True if the evaluation is successful, otherwise false.
    ///
    bool evaluate(double _val) const;

    ///
    /// \brief getDescription
    /// Provides a string description of the constraint.
    /// \return A string that describes the constraint.
    ///
    std::string getDescription() const;

private:
    double m_value;
    std::string m_operatorStr;
    std::function<bool(double, double)> m_op; // Function to perform the comparison operation
};

class ConfigCovariate
{
public:
    ///
    /// \brief ConfigCovariate
    /// Constructs a ConfigCovariate object with the given parameters.
    /// \param _covariateId The ID of the covariate.
    /// \param _covariateType The type of the covariate.
    /// \param _validRange A vector of Constraint objects representing the valid range.
    /// \param _typicalRange A vector of Constraint objects representing the typical range.
    ///
    ConfigCovariate(
            std::string _covariateId,
            std::string _covariateType,
            const std::vector<Constraint>& _validRange,
            const std::vector<Constraint>& _typicalRange);

    ///
    /// \brief evaluateValue
    /// Evaluates if a given value falls within the valid range or typical range of the covariate.
    /// \param _val The value to be evaluated.
    /// \return An EvaluationResult enum representing the evaluation result. Returns NotInRange if not in range,
    ///         InValidRange if in valid range, and InTypicalRange if in typical range.
    ///
    EvaluationResult evaluateValue(double _val) const;

    ///
    /// \brief getCovariateId
    /// Retrieves the ID of the covariate.
    /// \return A string representing the covariate ID.
    ///
    std::string getCovariateId() const;

    ///
    /// \brief getValidRangeDescription
    /// Provides a string description of the valid range constraints for the covariate.
    /// \return A string that describes the valid range constraints.
    ///
    std::string getValidRangeDescription() const;

    ///
    /// \brief getTypicalRangeDescription
    /// Provides a string description of the typical range constraints for the covariate.
    /// \return A string that describes the typical range constraints.
    ///
    std::string getTypicalRangeDescription() const;

private:
    std::string m_covariateId;
    std::string m_covariateType;
    std::vector<Constraint> m_validRange;
    std::vector<Constraint> m_typicalRange;

    ///
    /// \brief isInRange
    /// Checks if a value is within a specified range of constraints.
    /// \param val The value to be checked.
    /// \param range A vector of constraints representing the range.
    /// \return True if the value is within the range, otherwise false.
    ///
    bool isInRange(double _val, const std::vector<Constraint>& _range) const;

    ///
    /// \brief getRangeDescription
    /// Provides a string description of a specified range of constraints.
    /// \param _range A vector of constraints representing the range.
    /// \return A string that describes the constraints in the range.
    ///
    std::string getRangeDescription(const std::vector<Constraint>& _range) const;
};

class Configuration
{
public:
    Configuration() {}

    ///
    /// \brief Configuration
    /// Constructs a Configuration object with the given parameters.
    /// \param _configName The name of the configuration.
    /// \param _drugId The ID of the drug associated with the configuration.
    /// \param _drugModelId The ID of the drug model associated with the configuration.
    /// \param _covariates A vector of ConfigCovariate objects associated with the configuration.
    ///
    Configuration(
            std::string _configName,
            std::string _drugId,
            std::string _drugModelId,
            const std::vector<ConfigCovariate>& _covariates);

    ///
    /// \brief getName
    /// Retrieves the name of the configuration.
    /// \return A string representing the configuration name.
    ///
    std::string getName() const;

    ///
    /// \brief getDrug
    /// Retrieves the ID of the drug associated with the configuration.
    /// \return A string representing the drug ID.
    ///
    std::string getDrug() const;

    ///
    /// \brief getDrugModel
    /// Retrieves the ID of the drug model associated with the configuration.
    /// \return A string representing the drug model ID.
    ///
    std::string getDrugModel() const;

    ///
    /// \brief getCovariatesIds
    /// Retrieves the IDs of all covariates associated with the configuration.
    /// \return A vector of strings representing the IDs of the covariates.
    ///
    std::vector<std::string> getCovariatesIds() const;

    ///
    /// \brief validateBoolValue
    /// Validates that a given covariate exists among the covariate set.
    /// \param _covariateName The name of the covariate.
    /// \param _val The value to be validated, in string form (only "0", "1", "true", "false" are accepted.
    /// \return An EvalutationResult enum representing the validation result. Returns CovariateNotExist if the covariate name is not found.
    ///
    EvaluationResult validateBoolValue(const std::string& _covariateName, std::string const& _val) const;

    ///
    /// \brief validateValue
    /// Validates a value against the constraints of a specified covariate.
    /// \param _covariateName The name of the covariate.
    /// \param _val The value to be validated.
    /// \return An EvalutationResult enum representing the validation result. Returns CovariateNotExist if the covariate name is not found.
    ///
    EvaluationResult validateValue(const std::string& _covariateName, double _val) const;

    ///
    /// \brief isValid
    /// Return true if the configuration contain value, false if the config is empty.
    /// \return A boolean representing if the configuration is valid. True if valid else False.
    ///
    bool isValid() const;

    ///
    /// \brief getValidRange
    /// Retrieves a string description of the valid range for a specified covariate.
    /// \param _covariateName The name of the covariate.
    /// \return A string representing the valid range of the specified covariate.
    ///
    std::string getValidRange(const std::string& _covariateName) const;

    ///
    /// \brief getTypicalRange
    /// Retrieves a string description of the typical range for a specified covariate.
    /// \param _covariateName The name of the covariate.
    /// \return A string representing the typical range of the specified covariate.
    ///
    std::string getTypicalRange(const std::string& _covariateName) const;

private:
    std::string m_configName;
    std::string m_drugId;
    std::string m_drugModelId;
    std::vector<ConfigCovariate> m_covariates;
};

class Configurations
{
public:
    Configurations() = default;
    virtual ~Configurations() = default;

    ///
    /// \brief addConfiguration
    /// Adds a new configuration to the collection of configurations.
    /// \param _config The Configuration object to be added.
    ///
    void addConfiguration(const Configuration& _config);

    ///
    /// \brief getConfiguration
    /// Retrieves a configuration by its name.
    /// \param _configName The name of the configuration to be retrieved.
    /// \return The Configuration object associated with the specified name.
    ///
    Configuration getConfiguration(const std::string& _configName) const;

private:
    std::vector<Configuration> m_configurations;
};


class ComputationLimits
{
    static size_t const PRED_POINTS_NO_DEFAULT = 10000;
    static size_t const PCT_POINTS_NO_DEFAULT = 2000;
    static size_t const DOSAGE_POSSIBILITIES_NO_DEFAULT = 10000;

public:
    ComputationLimits()
        : m_predPointsNo(PRED_POINTS_NO_DEFAULT), m_pctPointsNo(PCT_POINTS_NO_DEFAULT),
          m_dosagePossibilitiesNo(DOSAGE_POSSIBILITIES_NO_DEFAULT)
    {
    }

    ///
    /// \brief getPredPointsNo
    /// Retrieves the number of prediction points to be used.
    /// \return Number of prediction points to be used.
    ///
    size_t getPredPointsNo() const
    {
        return m_predPointsNo;
    };

    ///
    /// \brief getPctPointsNo
    /// Retrieves the number of percentiles points to be used.
    /// \return Number of percentiles points to be used.
    ///
    size_t getPctPointsNo() const
    {
        return m_pctPointsNo;
    };

    ///
    /// \brief getDosagePossibilitiesNo
    /// Retrieves the number of dosage possibilities to consider.
    /// \return Number of dosage possibilities to consider.
    ///
    size_t getDosagePossibilitiesNo() const
    {
        return m_dosagePossibilitiesNo;
    };

    ///
    /// \brief setPredPointsNo
    /// Set the number of prediction points to be used.
    /// \params _predPointsNo Number of prediction points to be used.
    ///
    void setPredPointsNo(size_t const _predPointsNo)
    {
        m_predPointsNo = _predPointsNo;
    };

    ///
    /// \brief setPctPointsNo
    /// Set the number of percentiles points to be used.
    /// \params _pctPointsNo Number of percentiles points to be used.
    ///
    void setPctPointsNo(size_t const _pctPointsNo)
    {
        m_pctPointsNo = _pctPointsNo;
    };

    ///
    /// \brief setDosagePossibilitiesNo
    /// Set the number of dosage possibilities to consider.
    /// \params _dosagePossibilitiesNo Number of dosage possibilities to consider.
    ///
    void setDosagePossibilitiesNo(size_t const _dosagePossibilitiesNo)
    {
        m_dosagePossibilitiesNo = _dosagePossibilitiesNo;
    };

private:
    size_t m_predPointsNo;
    size_t m_pctPointsNo;
    size_t m_dosagePossibilitiesNo;
};


} // namespace Xpert
} // namespace Tucuxi

#endif // CONFIGURATION_H
