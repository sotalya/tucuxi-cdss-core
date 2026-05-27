// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/flow/general/generalxpertflowstepprovider.h"
#include "tucucdss-core/result/xpertqueryresult.h" // Source file to be tested

/// \brief Gtest of requestexecutor.
///        Tests for RequestExecutor from the GeneralXpertFlowStepProvider.
///        The tests use the AdjustmentTraitCreator object and assume
///        that this object works as intended.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

// RequestExecutor::perform fixture
class RequestExecutorTest : public testing::Test
{
public:
    std::string queryStringNoAdjustmentTrait =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
                                                    <treatment>
                                                        <dosageHistory>
                                                        </dosageHistory>
                                                    </treatment>
                                                    <!-- Samples history -->
                                                    <samples>
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
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

    std::string queryStringNoDrugModel =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
                                                    <treatment>
                                                        <dosageHistory>
                                                        </dosageHistory>
                                                    </treatment>
                                                    <!-- Samples history -->
                                                    <samples>
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
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

    std::string queryStringExecutionFailed =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-07T13:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
                                                    <treatment>
                                                        <dosageHistory>
                                                        </dosageHistory>
                                                    </treatment>
                                                    <!-- Samples history -->
                                                    <samples>
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
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

    std::string queryStringSuccessAdjustment =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-07T13:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
                                                    <treatment>
                                                        <dosageHistory>
                                                        </dosageHistory>
                                                    </treatment>
                                                    <!-- Samples history -->
                                                    <samples>
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
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

    std::string queryStringSuccessStatistics =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-07T13:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
                                                    <treatment>
                                                    </treatment>
                                                    <!-- Samples history -->
                                                    <samples>
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
                                        <requests>
                                            <xpertRequest>
                                                <drugId>imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                <output>
                                                    <format>xml</format>
                                                    <language>en</language>
                                                </output>
                                                <adjustmentDate>2018-07-06T08:00:00</adjustmentDate>
                                                <options>
                                                    <loadingOption>noLoadingDose</loadingOption>
                                                    <restPeriodOption>noRestPeriod</restPeriodOption>
                                                    <targetExtractionOption>populationValues</targetExtractionOption>
                                                    <formulationAndRouteSelectionOption>allFormulationAndRoutes</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>)";

    std::string queryStringGetAposteriori =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-07T13:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
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
                                                    <!-- Samples history -->
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
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
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

    /// \brief Query with a dosage producing concentrations within the imatinib target range (750-1500 ug/l) and
    ///        the dontAdjustIfCurrentInRange option. 400mg daily with a sample at 1200 ug/l.
    std::string queryStringDontAdjustInRange =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-09T13:00:00</date>

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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-10T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>24:00:00</interval>
                                                                            <dose>
                                                                                <value>400</value>
                                                                                <unit>mg</unit>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>oralSolution</formulation>
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
                                                            <sampleDate>2018-07-08T06:00:30</sampleDate>
                                                            <concentrations>
                                                                <concentration>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <value>1.2</value>
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
                                                <options>
                                                    <adjustmentWithCurrentDosageOption>dontAdjustIfCurrentInRange</adjustmentWithCurrentDosageOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>)";

    /// \brief Query with a dosage producing concentrations above the target range and dontAdjustIfCurrentInRange.
    ///        400mg q12h with a high sample (2.5 mg/l = 2500 ug/l, above 1500).
    std::string queryStringDontAdjustOutOfRange =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-09T13:00:00</date>

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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-10T08:00:00</end>
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
                                                                                <formulation>oralSolution</formulation>
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
                                                            <sampleDate>2018-07-08T06:00:30</sampleDate>
                                                            <concentrations>
                                                                <concentration>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <value>2.5</value>
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
                                                <options>
                                                    <adjustmentWithCurrentDosageOption>dontAdjustIfCurrentInRange</adjustmentWithCurrentDosageOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>)";

    /// \brief Same dosage/sample as queryStringDontAdjustInRange but with alwaysAdjust. Should produce multiple
    ///        candidates even though the current dosage is in range.
    std::string queryStringAlwaysAdjustInRange =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-09T13:00:00</date>

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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-10T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>24:00:00</interval>
                                                                            <dose>
                                                                                <value>400</value>
                                                                                <unit>mg</unit>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>oralSolution</formulation>
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
                                                            <sampleDate>2018-07-08T06:00:30</sampleDate>
                                                            <concentrations>
                                                                <concentration>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <value>1.2</value>
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
                                                <options>
                                                    <adjustmentWithCurrentDosageOption>alwaysAdjust</adjustmentWithCurrentDosageOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>)";

    /// \brief Same dosage/sample as queryStringDontAdjustInRange (in-range, 400mg daily, 1.2 mg/l) but with no
    ///        adjustmentWithCurrentDosageOption element. The default AlwaysAdjust should apply, producing candidates.
    std::string queryStringNoOptionInRange =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-09T13:00:00</date>

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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-10T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>24:00:00</interval>
                                                                            <dose>
                                                                                <value>400</value>
                                                                                <unit>mg</unit>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>oralSolution</formulation>
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
                                                            <sampleDate>2018-07-08T06:00:30</sampleDate>
                                                            <concentrations>
                                                                <concentration>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <value>1.2</value>
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

    /// \brief Query with dontAdjustIfCurrentInRange but an empty dosage history and no samples (a priori). The
    ///        short-circuit cannot fire because there is no current dosage to evaluate; the engine should fall
    ///        through to the normal candidate search.
    std::string queryStringDontAdjustEmptyHistory =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-09T13:00:00</date>

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
                                                <options>
                                                    <adjustmentWithCurrentDosageOption>dontAdjustIfCurrentInRange</adjustmentWithCurrentDosageOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>)";

    std::string queryStringGetApriori =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-07T13:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>
                                                <!-- All the information regarding the drug -->
                                                <drug>
                                                    <drugId>imatinib</drugId>
                                                    <activePrinciple>something</activePrinciple>
                                                    <brandName>somebrand</brandName>
                                                    <atc>something</atc>
                                                    <!-- All the information regarding the treatment -->
                                                    <treatment>
                                                        <dosageHistory>
                                                        </dosageHistory>
                                                    </treatment>
                                                    <!-- Samples history -->
                                                    <samples>
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                            </drugs>
                                        </drugTreatment>
                                        <!-- List of the requests we want the server to take care of -->
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

/// \brief Test cases for the RequestExecutor::perform method.
///        Check that there is an error if the adjustment trait of an
///        XpertRequestResult is nullptr in RequestExecutor. The RequestExecutor
///        must set the error in the XpertRequestResult and shouldBeProcessed
///        must return false.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, AdjustmentTraitNullptrResquestExecutor)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoAdjustmentTrait, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No adjustment trait set.");
}

/// \brief Test cases for the RequestExecutor::perform method.
///        The test loads an xpertRequest without assigning a drug model to it.
///        The RequestExecutor must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, DrugModelNullptrResquestExecutor)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDrugModel, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()->perform(xpertRequestResult);
    xpertRequestResult.setDrugModel(nullptr);
    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No drug model set.");
}

/// \brief Test cases for the RequestExecutor::perform method.
///        This method modifies the trait of the XpertRequestResult to fail.
///        The returned adjustmentData must be nullptr, the XpertRequestResult
///        receives an error message and shouldContinuProcessing returns false.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, FailedResquestExecutor)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringExecutionFailed, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    // Prepare a new trait that is invalid by inverting start and end dates.
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()->perform(xpertRequestResult);
    unique_ptr<Core::ComputingTraitAdjustment> baseTrait =
            make_unique<Core::ComputingTraitAdjustment>(*xpertRequestResult.getAdjustmentTrait());

    unique_ptr<Core::ComputingTraitAdjustment> newInvalidTrait = nullptr;
    newInvalidTrait = make_unique<Core::ComputingTraitAdjustment>(
            "",
            baseTrait->getEnd(),
            baseTrait->getStart(),
            baseTrait->getNbPointsPerHour(),
            baseTrait->getComputingOption(),
            baseTrait->getAdjustmentTime(),
            Core::BestCandidatesOption::BestDosage,
            baseTrait->getLoadingOption(),
            baseTrait->getRestPeriodOption(),
            baseTrait->getSteadyStateTargetOption(),
            baseTrait->getTargetExtractionOption(),
            baseTrait->getFormulationAndRouteSelectionOption());

    xpertRequestResult.setAdjustmentTrait(*newInvalidTrait);

    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "Adjustment request execution failed.");
    EXPECT_EQ(xpertRequestResult.getAdjustmentData(), nullptr);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        This method checks that when a valid adjustment request is made,
///        the adjustmentData is not nullptr, houldContinuProcessing returns
///        true and their is an adjustment in the adjustmentData.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, CorrectAdjustmentDataSucceededResquestExecutor)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccessAdjustment, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_NE(xpertRequestResult.getAdjustmentData(), nullptr);
    EXPECT_EQ(xpertRequestResult.getAdjustmentData()->getAdjustments().empty(), false);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        This method checks that when a valid adjustment request is performed,
///        there are 9 statistics at steady state.
///        The method shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, StatisticsSucceededResquestExecutor)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccessStatistics, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getCycleStats().getStats().empty(), false);
    EXPECT_EQ(xpertRequestResult.getCycleStats().getStats()[0].size(), 9);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        This method checks that the typical, apriori and aposteriori
///        parameters are defined when the base trait of the XpertRequestResult
///        is aposteriori. The parameters groups are not empty and they have the
///        same size. The method shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, TypicalAprioriAposterioriParametersGetter)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringGetAposteriori, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getParameters().size(), 3);
    EXPECT_EQ(xpertRequestResult.getParameters()[0].empty(), false);
    EXPECT_EQ(xpertRequestResult.getParameters()[0].size(), xpertRequestResult.getParameters()[1].size());
    EXPECT_EQ(xpertRequestResult.getParameters()[1].size(), xpertRequestResult.getParameters()[2].size());
}

/// \brief Test cases for the RequestExecutor::perform method.
///        This method checks that the typical and apriori parameters are
///        defined when the base trait of the XpertRequestResult is apriori. The
///        parameters groups are not empty and they have the same size. The
///        method shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, TypicalAprioriParametersGetter)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringGetApriori, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getParameters().size(), 2);
    EXPECT_EQ(xpertRequestResult.getParameters()[0].empty(), false);
    EXPECT_EQ(xpertRequestResult.getParameters()[0].size(), xpertRequestResult.getParameters()[1].size());
    ;
}

/// \brief Test cases for the RequestExecutor::perform method.
///        When the dontAdjustIfCurrentInRange option is set and the
///        current dosage yields concentrations within the target range,
///        exactly one adjustment is returned (the current dosage itself).
///        shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, DontAdjustIfCurrentInRangeWhenInRange)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(
            queryStringDontAdjustInRange,
            TestUtils::originalImatinibModelString,
            xpertQueryResult);

    XpertRequestResult& xpertRequestResult =
            xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()
            ->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()
            ->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_NE(xpertRequestResult.getAdjustmentData(), nullptr);
    EXPECT_EQ(
            xpertRequestResult.getAdjustmentData()
                    ->getAdjustments()
                    .size(),
            1);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        When the dontAdjustIfCurrentInRange option is set but the current dosage yields concentrations outside the
///        target range, multiple adjustment candidates are returned (normal search). shouldContinueProcessing must
///        return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, DontAdjustIfCurrentInRangeWhenOutOfRange)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(
            queryStringDontAdjustOutOfRange,
            TestUtils::originalImatinibModelString,
            xpertQueryResult);

    XpertRequestResult& xpertRequestResult =
            xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()
            ->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()
            ->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_NE(xpertRequestResult.getAdjustmentData(), nullptr);
    EXPECT_GT(
            xpertRequestResult.getAdjustmentData()
                    ->getAdjustments()
                    .size(),
            1);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        When the alwaysAdjust option is set and the current dosage is within the target range, multiple adjustment
///        candidates are still returned (the adjustment search is not skipped). shouldContinueProcessing must return
///        true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, AlwaysAdjustWhenInRange)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(
            queryStringAlwaysAdjustInRange,
            TestUtils::originalImatinibModelString,
            xpertQueryResult);

    XpertRequestResult& xpertRequestResult =
            xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()
            ->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()
            ->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_NE(xpertRequestResult.getAdjustmentData(), nullptr);
    EXPECT_GT(
            xpertRequestResult.getAdjustmentData()
                    ->getAdjustments()
                    .size(),
            1);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        When the adjustmentWithCurrentDosageOption XML element is absent, the default AlwaysAdjust should apply.
///        Even though the current dosage is in the target range, multiple candidates are returned.
///        shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, DefaultOptionAbsentProducesMultipleCandidates)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(
            queryStringNoOptionInRange,
            TestUtils::originalImatinibModelString,
            xpertQueryResult);

    XpertRequestResult& xpertRequestResult =
            xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()
            ->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()
            ->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_NE(xpertRequestResult.getAdjustmentData(), nullptr);
    EXPECT_GT(
            xpertRequestResult.getAdjustmentData()
                    ->getAdjustments()
                    .size(),
            1);
}

/// \brief Test cases for the RequestExecutor::perform method.
///        When dontAdjustIfCurrentInRange is set but the dosage history is empty, there is no current dosage to
///        evaluate. The short-circuit cannot fire, so the engine falls through to the normal a priori candidate
///        search and returns multiple candidates. shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(RequestExecutorTest, DontAdjustWithEmptyDosageHistory)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(
            queryStringDontAdjustEmptyHistory,
            TestUtils::originalImatinibModelString,
            xpertQueryResult);

    XpertRequestResult& xpertRequestResult =
            xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getAdjustmentTraitCreator()
            ->perform(xpertRequestResult);
    TestUtils::flowStepProvider.getRequestExecutor()
            ->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_NE(xpertRequestResult.getAdjustmentData(), nullptr);
    EXPECT_GT(
            xpertRequestResult.getAdjustmentData()
                    ->getAdjustments()
                    .size(),
            1);
}

} // namespace Xpert
} // namespace Tucuxi
