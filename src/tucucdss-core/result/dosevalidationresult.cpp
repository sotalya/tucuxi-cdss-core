// SPDX-License-Identifier: AGPL-3.0-or-later
#include "dosevalidationresult.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {


DoseValidationResult::DoseValidationResult(
        const Core::DosageBounded* _dose, const std::string& _warning, const WarningLevel& _warningLevel)
    : AbstractValidationResult<Core::DosageBounded>(_dose, _warning, _warningLevel)
{
}

} // namespace Xpert
} // namespace Tucuxi
