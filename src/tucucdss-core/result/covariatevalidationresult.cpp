// SPDX-License-Identifier: AGPL-3.0-or-later
#include "covariatevalidationresult.h"

#include "tucucommon/utils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

CovariateValidationResult::CovariateValidationResult(
        const Core::CovariateDefinition* _definition,
        const Core::PatientCovariate* _patient,
        const std::string& _warning,
        WarningLevel _warningLevel)
    : AbstractValidationResult<Core::CovariateDefinition>(_definition, _warning, _warningLevel),
      m_patientCovariate(_patient)
{
}

std::string CovariateValidationResult::getValue() const
{
    // If the patient's covariate is defined.
    if (m_patientCovariate != nullptr) {
        return m_patientCovariate->getValue();
    }

    return Common::Utils::varToString(m_source->getValue());
}

Core::DataType CovariateValidationResult::getDataType() const
{
    // If the patient's covariate is defined.
    if (m_patientCovariate != nullptr) {
        return m_patientCovariate->getDataType();
    }

    return m_source->getDataType();
}

Common::TucuUnit CovariateValidationResult::getUnit() const
{
    // If the patient's covariate is defined.
    if (m_patientCovariate != nullptr) {
        return m_patientCovariate->getUnit();
    }

    return m_source->getUnit();
}

CovariateType CovariateValidationResult::getType() const
{
    // If the patient's covariate is defined.
    if (m_patientCovariate != nullptr) {
        return CovariateType::PATIENT;
    }

    return CovariateType::MODEL;
}


const Core::PatientCovariate* CovariateValidationResult::getPatientCovariate() const
{
    return m_patientCovariate;
}

} // namespace Xpert
} // namespace Tucuxi
