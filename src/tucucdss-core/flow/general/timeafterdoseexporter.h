// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef TIMEAFTERDOSEEXPORTER_H
#define TIMEAFTERDOSEEXPORTER_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

class TimeAfterDoseExporter : public AbstractXpertFlowStep
{
public:
    void perform(XpertRequestResult& _xpertRequestResult) override;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // TIMEAFTERDOSEEXPORTER_H
