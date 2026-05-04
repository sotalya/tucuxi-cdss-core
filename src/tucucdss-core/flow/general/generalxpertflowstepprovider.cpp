// SPDX-License-Identifier: AGPL-3.0-or-later
#include "generalxpertflowstepprovider.h"

#include "tucucdss-core/flow/general/adjustmenttraitcreator.h"
#include "tucucdss-core/flow/general/covariatechecker.h"
#include "tucucdss-core/flow/general/covariatevalidatorandmodelselector.h"
#include "tucucdss-core/flow/general/datevalidator.h"
#include "tucucdss-core/flow/general/dosevalidator.h"
#include "tucucdss-core/flow/general/justificationcreator.h"
#include "tucucdss-core/flow/general/padcollector.h"
#include "tucucdss-core/flow/general/reportprinter.h"
#include "tucucdss-core/flow/general/requestexecutor.h"
#include "tucucdss-core/flow/general/samplevalidator.h"
#include "tucucdss-core/flow/general/targetvalidator.h"
#include "tucucdss-core/flow/general/timeafterdoseexporter.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

GeneralXpertFlowStepProvider::GeneralXpertFlowStepProvider()
{
    m_covariateValidatorAndModelSelector = make_unique<CovariateValidatorAndModelSelector>();
    m_doseValidator = make_unique<DoseValidator>();
    m_sampleValidator = make_unique<SampleValidator>();
    m_dateValidator = make_unique<DateValidator>();
    m_targetValidator = make_unique<TargetValidator>();
    m_adjustmentTraitCreator = make_unique<AdjustmentTraitCreator>();
    m_requestExecutor = make_unique<RequestExecutor>();
    m_reportPrinter = make_unique<ReportPrinter>();
    m_covariateChecker = make_unique<CovariateChecker>();
    m_justificationCreator = make_unique<JustificationCreator>();
    m_tdaExporter = make_unique<TimeAfterDoseExporter>();
    m_padCollector = make_unique<PADCollector>();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getCovariateValidatorAndModelSelector() const
{
    return m_covariateValidatorAndModelSelector.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getCovariateChecker() const
{
    return m_covariateChecker.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getJustificationCreator() const
{
    return m_justificationCreator.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getTADExporter() const
{
    return m_tdaExporter.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getDoseValidator() const
{
    return m_doseValidator.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getSampleValidator() const
{
    return m_sampleValidator.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getDateValidator() const
{
    return m_dateValidator.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getTargetValidator() const
{
    return m_targetValidator.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getAdjustmentTraitCreator() const
{
    return m_adjustmentTraitCreator.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getRequestExecutor() const
{
    return m_requestExecutor.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getReportPrinter() const
{
    return m_reportPrinter.get();
}

AbstractXpertFlowStep* GeneralXpertFlowStepProvider::getPADCollector() const
{
    return m_padCollector.get();
}

} // namespace Xpert
} // namespace Tucuxi
