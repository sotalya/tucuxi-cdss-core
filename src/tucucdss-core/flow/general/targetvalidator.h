// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef TARGETVALIDATOR_H
#define TARGETVALIDATOR_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class is used to check the consistency of the patient's targets.
///        - Two patient targets cannot have the same target type for the same active moitety.
///        - Each patient target active moiety must exist in the drug model.
class TargetValidator : public AbstractXpertFlowStep
{
public:
    /// \brief The getTargetValidation method simply checks that two targets don't overlap.
    ///        In other words, it checks that the targets that have the same active moiety
    ///        don't have the same target type. In addition, it checks that each patient's
    ///        target is related to an active moiety that exists in the drug model.
    ///
    ///        If any of these checks fail, the XpertRequestResult gets an error and
    ///        should not be processed further.
    /// \param _xpertRequestResult XpertResult object containing the targets and the associated drug model.
    void perform(XpertRequestResult& _xpertRequestResult) override;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // TARGETVALIDATOR_H
