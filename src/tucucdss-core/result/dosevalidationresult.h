// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef DOSEVALIDATIONRESULT_H
#define DOSEVALIDATIONRESULT_H

#include "tucucore/dosage.h"

#include "tucucdss-core/result/abstractvalidationresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class stores the validation result for a dose of a patient.
///        It contains a pointer to the single dose and possibly
///        a warning message.
///
///        The warning message indicates if the dose is too low or too high
///        compared to the doses recommended by the drug model.
class DoseValidationResult : public AbstractValidationResult<Core::DosageBounded>
{
public:
    /// \brief Constructor.
    /// \param _dose Patient dose concerned by this validation result.
    ///              The dose must have at least the same lifetime as this object
    /// \param _warning Associated warning message.
    DoseValidationResult(
            const Core::DosageBounded* _dose, const std::string& _warning, const WarningLevel& _warningLevel);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // DOSEVALIDATIONRESULT_H
