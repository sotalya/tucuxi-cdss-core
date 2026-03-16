// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef COVARIATEVALIDATIONRESULT_H
#define COVARIATEVALIDATIONRESULT_H

#include <string>

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"

#include "tucucdss-core/result/abstractvalidationresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Enum used to specify the source of a covariate.
enum class CovariateType
{
    PATIENT, /**< The covariate value comes from the query. It's the patient's value. */
    MODEL    /**< The covariate value comes from the drug model. This is the default value. */
};

/// \brief This class stores the validation result for a patient covariate.
///        This class is created during the CovariateChecker
///        phase. Its purpose is to tell what the computing core will receive:
///
///         - The value of the drugmodel.
///              or
///         - The patient value.
///
///         When the patient value is used, it is possible to set a
///         warning if the validation of the covariate definition is not respected.
class CovariateValidationResult : public AbstractValidationResult<Core::CovariateDefinition>
{
public:
    /// \brief Constructor.
    /// \param _definition Definition of the covariate in the drug model. Should never be nullptr.
    ///                    The definition must have at least the same lifetime as this object.
    /// \param _patient Patient-related covariate, if present. May be nullptr, if not defined.
    ///                 The patient-related covariate must have at least the same lifetime as this object.
    /// \param _warning Associated warning message.
    /// \param _warningLevel Warning level.
    CovariateValidationResult(
            const Core::CovariateDefinition* _definition,
            const Core::PatientCovariate* _patient,
            const std::string& _warning,
            WarningLevel _warningLevel);

    // Getters

    /// \brief Get the value of the covariate. If the patient-related covariate is defined,
    ///        it is used, otherwise it returns the value of the definition.
    /// \return The value.
    [[nodiscard]] std::string getValue() const;

    /// \brief Get the data type of the covariate value.
    /// \return The data type.
    [[nodiscard]] Core::DataType getDataType() const;

    /// \brief Get the unit of measurement of the value. If the patient is defined,
    ///        it uses its unit, otherwise it uses the definition.
    /// \return The measurement unit.
    [[nodiscard]] Common::TucuUnit getUnit() const;

    /// \brief Get where the value, the unit and the data type come from.
    /// \return CovariateType::Patient if it comes from the patient,
    ///         otherwise CovariateType::Model
    [[nodiscard]] CovariateType getType() const;


    /// \brief Get the pointer on the patient covariate that overrides the definition.
    ///        The pointer returned by this method must not be deleted.
    /// \return The pointer on the patient's covariate if it exists, otherwise nullptr.
    [[nodiscard]] const Core::PatientCovariate* getPatientCovariate() const;


protected:
    /// \brief The possible patient's covariate that overrides the definition.
    const Core::PatientCovariate* m_patientCovariate;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // COVARIATEVALIDATIONRESULT_H
