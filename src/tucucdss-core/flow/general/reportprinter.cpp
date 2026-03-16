// SPDX-License-Identifier: AGPL-3.0-or-later
#include <memory>

#include "reportprinter.h"

#include "tucucdss-core/exporter/abstractxpertrequestresultexport.h"
#include "tucucdss-core/exporter/xpertrequestresulthtmlexport.h"
#include "tucucdss-core/exporter/xpertrequestresultxmlexport.h"
#include "tucucdss-core/query/xpertrequestdata.h"
#ifndef CONFIG_WITHPDF
#include "tucucdss-core/exporter/xpertrequestresultpdfexport.h"
#endif // CONFIG_WITHPDF

using namespace std;

namespace Tucuxi {
namespace Xpert {

void ReportPrinter::perform(XpertRequestResult& _xpertRequestResult)
{
    // Extract the request format.
    OutputFormat desiredOutputFormat = _xpertRequestResult.getXpertRequest().getOutputFormat();

    unique_ptr<AbstractXpertRequestResultExport> exporter = nullptr;

    // Select the cooresponding exporter.
    switch (desiredOutputFormat) {
    case OutputFormat::XML:
        exporter = make_unique<XpertRequestResultXmlExport>();
        break;
    case OutputFormat::HTML:
        exporter = make_unique<XpertRequestResultHtmlExport>();
        break;
#ifndef CONFIG_WITHPDF
    case OutputFormat::PDF:
        exporter = make_unique<XpertRequestResultPdfExport>(make_unique<XpertRequestResultHtmlExport>());
        break;
#endif // CONFIG_WITHPDF
    }

    // Launch export.
    exporter->exportToFile(_xpertRequestResult);
}

} // namespace Xpert
} // namespace Tucuxi
