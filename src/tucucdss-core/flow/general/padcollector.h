// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef PADCOLLECTOR_H
#define PADCOLLECTOR_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Retrieve pre-adjustment data and store it in the output request result.
class PADCollector : public AbstractXpertFlowStep
{
public:
    /// \brief Compute pre-adjustment apriori percentiles and aposteriori concentrations.
    /// \param _xpertRequestResult XpertRequestResult containing the data required for the computations.
    void perform(XpertRequestResult& _xpertRequestResult) override;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // PADCOLLECTOR_H
