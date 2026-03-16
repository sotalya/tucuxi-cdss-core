// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef COVARIATECHECKER_H
#define COVARIATECHECKER_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"



namespace Tucuxi {
namespace Xpert {

class CovariateChecker : public AbstractXpertFlowStep
{
public:
    void perform(XpertRequestResult& _xpertRequestResult) override;

protected:
private:
    std::string getWarningMessage(
            EvaluationResult _ret, const std::string& _covariateId, const Configuration& _configuration);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // COVARIATECHECKER_H
