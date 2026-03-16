// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef DOSEDATEVALIDATIONRESULT_H
#define DOSEDATEVALIDATIONRESULT_H

#include "tucucore/dosage.h"

#include "tucucdss-core/result/abstractvalidationresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Validation result for a dosage date.
class DoseDateValidationResult : public AbstractValidationResult<Core::DosageBounded>
{
public:
    /// \brief Constructor.
    /// \param _src Pointer to the source object of the warning.
    /// \param _warning Associated warning message.
    /// \param _warningLevel Warning level of the message.
    DoseDateValidationResult(
            const Core::DosageBounded* _src, const std::string& _warning, const WarningLevel& _warningLevel);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // DOSEDATEVALIDATIONRESULT_H
