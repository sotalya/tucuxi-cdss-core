// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/flow/general/reportprinter.h"
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of ReportPrinter.
///        Tests for ReportPrinter from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

class ReportPrinterTest : public testing::Test
{
public:
    std::string queryStringXmlOutput = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                     <format>xml</format>
                                                     <language>en</language>
                                                 </output>
                                             </xpertRequest>
                                         </requests>
                                     </query>)";

    std::string queryStringHtmlOutput = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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

/// \brief Test cases for the ReportPrinter class.
///        Check that the ReportPrinter can be instantiated and inherits from AbstractXpertFlowStep.
TEST_F(ReportPrinterTest, ReportPrinterInstantiation)
{
    ReportPrinter printer;
    AbstractXpertFlowStep* stepPtr = &printer;
    EXPECT_NE(stepPtr, nullptr);
}

} // namespace Xpert
} // namespace Tucuxi
