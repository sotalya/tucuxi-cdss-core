// SPDX-License-Identifier: AGPL-3.0-or-later
#include "sampledatevalidationresult.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {


SampleDateValidationResult::SampleDateValidationResult(
        const Core::Sample* _src, const std::string& _warning, const WarningLevel& _warningLevel)
    : AbstractValidationResult<Core::Sample>(_src, _warning, _warningLevel)
{
}

} // namespace Xpert
} // namespace Tucuxi
