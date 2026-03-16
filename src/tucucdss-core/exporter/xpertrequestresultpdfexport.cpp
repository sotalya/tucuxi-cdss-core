// SPDX-License-Identifier: AGPL-3.0-or-later
#include "xpertrequestresultpdfexport.h"

#include "tucucdss-core/utils/xpertutils.h"
#include "wkhtmltox/pdf.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

XpertRequestResultPdfExport::XpertRequestResultPdfExport(unique_ptr<AbstractHtmlExport> _htmlExport)
    : m_htmlExport(move(_htmlExport))
{
}

void XpertRequestResultPdfExport::exportToFile(XpertRequestResult& _xpertRequestResult)
{
    // Get the pdf file name.
    std::string outputFileName = computeFileName(_xpertRequestResult);

    // Try to delete the pdf file if it already exists.
    // Wkhtmltox does not replace it automatically.
    ifstream pdfFileStream(outputFileName.c_str());
    if (pdfFileStream.good() && remove(outputFileName.c_str()) != 0) {
        _xpertRequestResult.setErrorMessage(
                "Output pdf file: " + outputFileName + " already exists and could not be replaced.");
        return;
    }

    // Create the file name of the intermediate html file.
    std::string intermediateHtmlFileName = computeFileName(_xpertRequestResult, true, false) + "_temp.html";

    // Generate the intermediate html file.
    m_htmlExport->exportToFile(intermediateHtmlFileName, _xpertRequestResult);
    if (!_xpertRequestResult.shouldContinueProcessing()) {
        _xpertRequestResult.setErrorMessage("Error during the generation of the html for pdf exportation.");

        // Try to delete the intermediate file (may be there is a permission problem).
        remove(intermediateHtmlFileName.c_str());

        return;
    }

    // Based on: https://github.com/wkhtmltopdf/wkhtmltopdf/blob/master/examples/pdf_c_api.c
    // Preparing the wkhtmltox objects.
    wkhtmltopdf_global_settings* gs;
    wkhtmltopdf_object_settings* os;
    wkhtmltopdf_converter* c;

    // Init wkhtmltopdf in graphics less mode.
    wkhtmltopdf_init(false);

    // Create a global settings object used to store options that are not
    // related to input objects.
    gs = wkhtmltopdf_create_global_settings();

    wkhtmltopdf_set_global_setting(gs, "out", outputFileName.c_str());
    wkhtmltopdf_set_global_setting(gs, "margin.top", "8");
    wkhtmltopdf_set_global_setting(gs, "margin.bottom", "8");
    wkhtmltopdf_set_global_setting(gs, "margin.left", "0");
    wkhtmltopdf_set_global_setting(gs, "margin.right", "0");

    // Create an input object settings object that is used to store settings
    // related to a input object, note that control of this object is parsed to
    // the converter later, which is then responsible for freeing it
    os = wkhtmltopdf_create_object_settings();

    // Set the file to convert as the intermediate html file.
    wkhtmltopdf_set_object_setting(os, "page", intermediateHtmlFileName.c_str());
    wkhtmltopdf_set_object_setting(os, "load.blockLocalFileAccess", "false");
    wkhtmltopdf_set_object_setting(os, "web.enableIntelligentShrinking", "false");

    // Create the actual converter object used to convert the pages.
    c = wkhtmltopdf_create_converter(gs);

    // Add the the settings object to the list of pages to convert.
    wkhtmltopdf_add_object(c, os, NULL);

    // Perform the conversion.
    if (!wkhtmltopdf_convert(c)) {
        _xpertRequestResult.setErrorMessage(
                "Error during the conversion of the html file into pdf. Http error code: "
                + std::to_string(wkhtmltopdf_http_error_code(c)));
    }

    // Destroy the converter object.
    wkhtmltopdf_destroy_converter(c);

    // We will no longer be needing wkhtmltopdf funcionality.
    wkhtmltopdf_deinit();

    // Try to remove the temp file.
    remove(intermediateHtmlFileName.c_str());
}

} // namespace Xpert
} // namespace Tucuxi
