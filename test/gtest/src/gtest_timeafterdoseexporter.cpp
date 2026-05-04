// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/flow/general/timeafterdoseexporter.h"
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of TimeAfterDoseExporter.
///        Tests for TimeAfterDoseExporter from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

class TimeAfterDoseExporterTest : public testing::Test
{
public:
    std::string queryStringNoDosageHistory = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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

    std::string queryStringWithDosageAndSamples = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                     <query version="1.0"
                                         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                         xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                         <date>2018-07-11T13:45:30</date>

                                         <drugTreatment>
                                             <patient>
                                                 <covariates>
                                                     <covariate>
                                                         <covariateId>age</covariateId>
                                                         <value>50</value>
                                                         <unit>y</unit>
                                                     </covariate>
                                                     <covariate>
                                                         <covariateId>bodyweight</covariateId>
                                                         <value>70</value>
                                                         <unit>kg</unit>
                                                     </covariate>
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
                                                             <dosageTimeRange>
                                                                 <start>2018-07-06T08:00:00</start>
                                                                 <end>2018-07-08T08:00:00</end>
                                                                 <dosage>
                                                                     <dosageLoop>
                                                                         <lastingDosage>
                                                                             <interval>12:00:00</interval>
                                                                             <dose>
                                                                                 <value>400</value>
                                                                                 <unit>mg</unit>
                                                                                 <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                             </dose>
                                                                             <formulationAndRoute>
                                                                                 <formulation>parenteralSolution</formulation>
                                                                                 <administrationName>foo bar</administrationName>
                                                                                 <administrationRoute>oral</administrationRoute>
                                                                                 <absorptionModel>extravascular</absorptionModel>
                                                                             </formulationAndRoute>
                                                                         </lastingDosage>
                                                                     </dosageLoop>
                                                                 </dosage>
                                                             </dosageTimeRange>
                                                         </dosageHistory>
                                                     </treatment>
                                                     <samples>
                                                         <sample>
                                                             <sampleId>123456</sampleId>
                                                             <sampleDate>2018-07-07T06:00:30</sampleDate>
                                                             <concentrations>
                                                                 <concentration>
                                                                     <analyteId>imatinib</analyteId>
                                                                     <value>0.7</value>
                                                                     <unit>mg/l</unit>
                                                                 </concentration>
                                                             </concentrations>
                                                         </sample>
                                                         <sample>
                                                             <sampleId>123457</sampleId>
                                                             <sampleDate>2018-07-08T07:00:00</sampleDate>
                                                             <concentrations>
                                                                 <concentration>
                                                                     <analyteId>imatinib</analyteId>
                                                                     <value>10.6</value>
                                                                     <unit>mg/l</unit>
                                                                 </concentration>
                                                             </concentrations>
                                                         </sample>
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

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the TimeAfterDoseExporter class.
///        Check that the TimeAfterDoseExporter can be instantiated and inherits from AbstractXpertFlowStep.
TEST_F(TimeAfterDoseExporterTest, TimeAfterDoseExporterInstantiation)
{
    TimeAfterDoseExporter exporter;
    AbstractXpertFlowStep* stepPtr = &exporter;
    EXPECT_NE(stepPtr, nullptr);
}

/// \brief Test cases for the TimeAfterDoseExporter::perform method.
///        Check that the method returns early when dosage history is empty.
TEST_F(TimeAfterDoseExporterTest, EmptyDosageHistory)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDosageHistory, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    TimeAfterDoseExporter exporter;
    exporter.perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.shouldContinueProcessing());
    EXPECT_EQ(xpertRequestResult.getTimeAfterDoseDurations().size(), 0);
}

} // namespace Xpert
} // namespace Tucuxi
