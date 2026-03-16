// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef SAMPLEDATEVALIDATIONRESULT_H
#define SAMPLEDATEVALIDATIONRESULT_H

#include "tucucore/drugtreatment/sample.h"

#include "tucucdss-core/result/abstractvalidationresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Validation result for a sample date.
class SampleDateValidationResult : public AbstractValidationResult<Core::Sample>
{
public:
    /// \brief Constructor.
    /// \param _src Pointer to the source object of the warning.
    /// \param _warning Associated warning message.
    /// \param _warningLevel Warning level of the message.
    SampleDateValidationResult(
            const Core::Sample* _src, const std::string& _warning, const WarningLevel& _warningLevel);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // SAMPLEDATEVALIDATIONRESULT_H
