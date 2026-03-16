// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef REPORTPRINTER_H
#define REPORTPRINTER_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This step selects the exporter that matches the
///        desired output format of the XpertRequest.
///        In other words, it selects between the XML, HTML or PDF exporter
///        and send him the the XpertRequestResult to be exported.
///
///        This class chooses between:
///            - XpertRequestResultXmlExport
///            - XpertRequestResultHtmlExport
///            - XpertRequestResultPdfExport
///
class ReportPrinter : public AbstractXpertFlowStep
{
public:
    /// \brief Select the corresponding exporter and send him the XpertRequestResult.
    /// \param _xpertRequestResult XpertRequestResult to export.
    void perform(XpertRequestResult& _xpertRequestResult) override;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // REPORTPRINTER_H
