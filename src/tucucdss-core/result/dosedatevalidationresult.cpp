// SPDX-License-Identifier: AGPL-3.0-or-later
#include "dosedatevalidationresult.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {


DoseDateValidationResult::DoseDateValidationResult(
        const Core::DosageBounded* _src, const std::string& _warning, const WarningLevel& _warningLevel)
    : AbstractValidationResult<Core::DosageBounded>(_src, _warning, _warningLevel)
{
}

} // namespace Xpert
} // namespace Tucuxi
