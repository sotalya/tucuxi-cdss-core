// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/exporter/xpertrequestresulthtmlexport.h"
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of XpertRequestResultHtmlExport.
///        Tests for XpertRequestResultHtmlExport class.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

class XpertRequestResultHtmlExportTest : public testing::Test
{
public:
    std::string queryStringBasic = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                     <query version="1.0"
                                         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                         xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                         <date>2018-07-11T13:45:30</date>

                                         <drugTreatment>
                                             <patient>
                                                 <covariates>
                                                 </covariates>
                                             </patient>
                                             <drugs>
                                                 <drug>
                                                     <drugId>imatinib</drugId>
                                                     <activePrinciple>something</activePrinciple>
                                                     <brandName>somebrand</brandName>
                                                     <atc>something</atc>
                                                     <treatment>
                                                         <dosageHistory>
                                                         </dosageHistory>
                                                     </treatment>
                                                     <samples>
                                                     </samples>
                                                     <targets>
                                                     </targets>
                                                 </drug>
                                             </drugs>
                                         </drugTreatment>
                                         <requests>
                                             <xpertRequest>
                                                 <drugId>imatinib</drugId>
                                                 <configId>imatinib.gotta2012</configId>
                                                 <output>
                                                     <format>html</format>
                                                     <language>en</language>
                                                 </output>
                                             </xpertRequest>
                                         </requests>
                                     </query>)";

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the XpertRequestResultHtmlExport::exportToFile method.
///        Check that the export fails gracefully when file cannot be opened
///        due to invalid path.
TEST_F(XpertRequestResultHtmlExportTest, FileOpenFailure)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringBasic, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    XpertRequestResultHtmlExport exporter;
    exporter.exportToFile("/nonexistent/path/file.html", xpertRequestResult);

    EXPECT_FALSE(xpertRequestResult.shouldContinueProcessing());
    EXPECT_TRUE(xpertRequestResult.getErrorMessage().find("could not be opened") != std::string::npos);
}

/// \brief Test cases for the XpertRequestResultHtmlExport class.
///        Check that the exporter inherits from AbstractHtmlExport.
TEST_F(XpertRequestResultHtmlExportTest, InheritsFromAbstractHtmlExport)
{
    XpertRequestResultHtmlExport exporter;
    AbstractHtmlExport* abstractPtr = &exporter;
    EXPECT_NE(abstractPtr, nullptr);
}

/// \brief Test cases for the XpertRequestResultHtmlExport class.
///        Check that the exporter inherits from AbstractXpertRequestResultExport.
TEST_F(XpertRequestResultHtmlExportTest, InheritsFromAbstractXpertRequestResultExport)
{
    XpertRequestResultHtmlExport exporter;
    AbstractXpertRequestResultExport* abstractPtr = &exporter;
    EXPECT_NE(abstractPtr, nullptr);
}

} // namespace Xpert
} // namespace Tucuxi
