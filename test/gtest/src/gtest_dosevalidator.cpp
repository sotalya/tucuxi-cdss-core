// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "testutils.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/result/xpertrequestresult.h"
#include "tucucdss-core/utils/xpertutils.h"

/// \brief Gtest of DoseValidator.
///        Tests for DoseValidator from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

// DoseValidator::perform and DoseValidator::checkDoses fixture
class DoseValidatorTest : public testing::Test
{
public:
    std::string queryStringNoTreatment = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                </covariates>
                                            </patient>
                                            <!-- List of the drugs informations we have concerning the patient -->
                                            <drugs>

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

    std::string queryStringNoDrugModel = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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

    std::string queryStringEmpty = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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


    std::string queryStringUnderDosing = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-08T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>12:00:00</interval>
                                                                            <dose>
                                                                                <value>1</value>
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

    std::string queryStringOverDosing = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-08T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>12:00:00</interval>
                                                                            <dose>
                                                                                <value>10000000</value>
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

    std::string queryStringBadDoseUnit = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-08T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>12:00:00</interval>
                                                                            <dose>
                                                                                <value>400</value>
                                                                                <unit>month</unit>
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


    std::string queryStringFRNotSupported = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                                                <administrationRoute>nasal</administrationRoute>
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

    std::string queryStringCorrectValues = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-08T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>
                                                                            <interval>12:00:00</interval>
                                                                            <dose>
                                                                                <value>3000</value>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-09T08:00:00</start>
                                                                <end>2018-07-10T08:00:00</end>
                                                                <dosage>
                                                                    <dosageSteadyState>
                                                                        <lastDoseDate>2018-07-09T20:00:00</lastDoseDate>
                                                                        <lastingDosage>
                                                                            <interval>12:00:00</interval>
                                                                            <dose>
                                                                                <value>3</value>
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
                                                                    </dosageSteadyState>
                                                                </dosage>
                                                            </dosageTimeRange>
                                                            <dosageTimeRange>
                                                                <start>2018-07-11T08:00:00</start>
                                                                <end>2018-08-12T08:00:00</end>
                                                                <dosage>
                                                                    <dosageSequence>
                                                                        <dosageRepeat>
                                                                            <iterations>4</iterations>
                                                                            <dailyDosage>
                                                                                <time>10:00:00</time>
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
                                                                            </dailyDosage>
                                                                        </dosageRepeat>
                                                                        <dosageRepeat>
                                                                            <iterations>2</iterations>
                                                                            <weeklyDosage>
                                                                                <day>2</day>
                                                                                <time>11:00:00</time>
                                                                                <dose>
                                                                                    <value>0.39</value>
                                                                                    <unit>g</unit>
                                                                                    <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                                </dose>
                                                                                <formulationAndRoute>
                                                                                    <formulation>parenteralSolution</formulation>
                                                                                    <administrationName>foo bar</administrationName>
                                                                                    <administrationRoute>oral</administrationRoute>
                                                                                    <absorptionModel>extravascular</absorptionModel>
                                                                                </formulationAndRoute>
                                                                            </weeklyDosage>
                                                                        </dosageRepeat>
                                                                    </dosageSequence>
                                                                </dosage>
                                                            </dosageTimeRange>
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


    std::string queryStringSimpleDoseList = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-08T08:00:00</end>
                                                                <dosage>
                                                                    <simpleDoseList>
                                                                        <unit>mg</unit>
                                                                        <formulationAndRoute>
                                                                            <formulation>parenteralSolution</formulation>
                                                                            <administrationName>foo bar</administrationName>
                                                                            <administrationRoute>oral</administrationRoute>
                                                                            <absorptionModel>extravascular</absorptionModel>
                                                                        </formulationAndRoute>
                                                                        <doseList>
                                                                            <doseDateValue>
                                                                                <doseDate>2018-07-06T08:00:00</doseDate>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                                <value>400</value>
                                                                            </doseDateValue>
                                                                            <doseDateValue>
                                                                                <doseDate>2018-07-07T08:00:00</doseDate>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                                <value>1</value>
                                                                            </doseDateValue>
                                                                        </doseList>
                                                                    </simpleDoseList>
                                                                </dosage>
                                                            </dosageTimeRange>
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

    std::string queryStringSingleDoseAtTimeList = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-06T08:00:00</start>
                                                                <end>2018-07-08T08:00:00</end>
                                                                <dosage>
                                                                    <singleDoseAtTimeList>
                                                                        <singleDoseAtTime>
                                                                            <doseDate>2018-07-06T08:00:00</doseDate>
                                                                            <dose>
                                                                                <value>400</value>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                                <unit>mg</unit>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>parenteralSolution</formulation>
                                                                                <administrationName>foo bar</administrationName>
                                                                                <administrationRoute>oral</administrationRoute>
                                                                                <absorptionModel>extravascular</absorptionModel>
                                                                            </formulationAndRoute>
                                                                        </singleDoseAtTime>
                                                                        <singleDoseAtTime>
                                                                            <doseDate>2018-07-07T08:00:00</doseDate>
                                                                            <dose>
                                                                                <value>10000000</value>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                                <unit>mg</unit>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>parenteralSolution</formulation>
                                                                                <administrationName>foo bar</administrationName>
                                                                                <administrationRoute>oral</administrationRoute>
                                                                                <absorptionModel>extravascular</absorptionModel>
                                                                            </formulationAndRoute>
                                                                        </singleDoseAtTime>
                                                                    </singleDoseAtTimeList>
                                                                </dosage>
                                                            </dosageTimeRange>
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

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        Check that there is an error if the treatment of an XpertRequestResult is nullptr
///        in DoseValidator.
///        The DoseValidator must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and there should be 0 DoseValidationResult.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, NullptrTreatmentDoseValidator)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoTreatment, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No treatment set.");
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 0);
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads an xpertRequest without assigning a drug model to it.
///        The DoseValidator must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and there should be 0 DoseValidationResult.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, NullptrDrugModelDoseValidator)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDrugModel, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No drug model set.");
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 0);
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query that has no dose. After the DoseValidator flow step,
///        when calling getDoseValidationResults of the XpertRequestResult, the returned
///        map must be empty and the shouldContinueProcessing method should return true.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, NoDoseInQueryDoseValidationResultsGetter)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringEmpty, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 0);
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query that has a dose. The dose is too low compared to the
///        recommended doses of the imatinib model. After the DoseValidator flow step,
///        when calling getDoseValidationResults of the XpertRequestResult, the returned
///        map must not be empty and the unique DoseValidationResult it contains
///        should have a warning message.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, UnderDosingDoseValidationResultGetter)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringUnderDosing, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 1);
    EXPECT_EQ(
            xpertRequestResult.getDoseValidationResults().begin()->second.getWarning(),
            "Minimum recommended dosage reached (100.00 mg)");
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query that has a dose. The dose is too high compared to the
///        recommended doses of the imatinib model. After the DoseValidator flow step,
///        when calling getDoseValidationResults of the XpertRequestResult, the returned
///        map must not be empty and the unique DoseValidationResult it contains
///        should have a warning message.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, OverDosingDoseValidationResultGetter)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringOverDosing, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 1);
    EXPECT_EQ(
            xpertRequestResult.getDoseValidationResults().begin()->second.getWarning(),
            "Maximum recommended dosage reached (800.00 mg)");
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query that has a dose. The dose has non-covertible unit.
///        After the DoseValidator flow step, when calling getDoseValidationResults of the XpertRequestResult,
///        the returned map must be empty, the shouldContinueProcessing method should return false
///        and there is an error message.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, BadDoseUnitDoseValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringBadDoseUnit, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 0);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "Patient dosage error found, details: Error in unit conversion");
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query that has a dose. The dose has a nasal route that is not compatible with the
///        imatinib model. After the DoseValidator flow step, when calling getDoseValidationResults of the XpertRequestResult,
///        the returned map must be empty, the shouldContinueProcessing method should return false and there is an error message.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, FormulationAndRouteNotSupportedByModelDoseValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringFRNotSupported, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 0);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "Patient dosage error found, details: No corresponding full formulation and route found for a dosage.");
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query that has three dosage time ranges. The dosage time ranges mix all the possible
///        dosge types. After the DoseValidator flow step,
///        when calling getDoseValidationResults of the XpertRequestResult, the returned
///        map must have the same size than the number of doses and the DoseValidationResults it contains
///        should have a correct dose, a correct unit and a correct warning message.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, AllDosageTypeAndMultipleTimeRangeDoseValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringCorrectValues, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 4);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");

    // All doses are diffrent and we are going to use this to get the correct asserts.
    // This is because map are not guaranteed in the same order. This is not really elegant but it makes
    // the trick.
    bool first = false;
    bool second = false;
    bool third = false;
    bool fourth = false;
    for (auto doseIt = xpertRequestResult.getDoseValidationResults().begin();
         doseIt != xpertRequestResult.getDoseValidationResults().end();
         ++doseIt) {

        if (Core::SingleDose const* mapKeyDose = dynamic_cast<Core::SingleDose const*>(doseIt->first)) {

            // Checking that the map key is equal to the pointer in the the
            // DoseValidationResult.
            // Must be true for all the doses.
            EXPECT_EQ(doseIt->second.getSource(), mapKeyDose);

            // Now, check the specific elements.
            switch (int(mapKeyDose->getDose())) {
            case 3000:
                EXPECT_EQ(doubleToString(mapKeyDose->getDose()), "3000.00");
                EXPECT_EQ(mapKeyDose->getDoseUnit().toString(), "mg");
                EXPECT_EQ(doseIt->second.getWarning(), "Maximum recommended dosage reached (800.00 mg)");
                first = true;
                break;

            case 3:
                EXPECT_EQ(doubleToString(mapKeyDose->getDose()), "3.00");
                EXPECT_EQ(mapKeyDose->getDoseUnit().toString(), "mg");
                EXPECT_EQ(doseIt->second.getWarning(), "Minimum recommended dosage reached (100.00 mg)");
                second = true;
                break;

            case 400:
                EXPECT_EQ(doubleToString(mapKeyDose->getDose()), "400.00");
                EXPECT_EQ(mapKeyDose->getDoseUnit().toString(), "mg");
                EXPECT_EQ(doseIt->second.getWarning(), "");
                third = true;
                break;

            case 0:
                EXPECT_EQ(doubleToString(mapKeyDose->getDose()), "0.39");
                EXPECT_EQ(mapKeyDose->getDoseUnit().toString(), "g");
                EXPECT_EQ(doseIt->second.getWarning(), "");
                fourth = true;
                break;
            }
        }
    }

    EXPECT_EQ(first && second && third && fourth, true);
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query with a SimpleDoseList dosage type. The last dose in the list
///        is 1 mg (under-dosing). After the DoseValidator flow step, the returned
///        DoseValidationResult should have a minimum dosage warning.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, SimpleDoseListDoseValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSimpleDoseList, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 1);
    EXPECT_EQ(
            xpertRequestResult.getDoseValidationResults().begin()->second.getWarning(),
            "Minimum recommended dosage reached (100.00 mg)");
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query with a SingleDoseAtTimeList dosage type. The last dose in
///        the list is 10000000 mg (over-dosing). After the DoseValidator flow step, the
///        returned DoseValidationResult should have a maximum dosage warning.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, SingleDoseAtTimeListDoseValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSingleDoseAtTimeList, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 1);
    EXPECT_EQ(
            xpertRequestResult.getDoseValidationResults().begin()->second.getWarning(),
            "Maximum recommended dosage reached (800.00 mg)");
}

/// \brief Test cases for the DoseValidator::perform and DoseValidator::checkDoses method.
///        The test loads a query with an empty dosage history, then programmatically adds
///        a ParallelDosageSequence (not supported by XML import) with a LastingDose inside.
///        This covers checkDoses(ParallelDosageSequence) which delegates to checkDosageBoundedList.
/// \param none but uses the related fixture
TEST_F(DoseValidatorTest, ParallelDosageSequenceDoseValidator)
{
    // Prepare the XpertRequestResult with empty dosage history
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringEmpty, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Programmatically add a ParallelDosageSequence to the treatment.
    // ParallelDosageSequence is not supported in XML import, so we build it in code.
    Core::FormulationAndRoute fr(Core::Formulation::ParenteralSolution, Core::AdministrationRoute::Oral, "foo bar");
    Core::LastingDose lastingDose(
            1.0,
            Common::TucuUnit("mg"),
            fr,
            Common::Duration(std::chrono::minutes(60)),
            Common::Duration(std::chrono::hours(12)));

    Core::ParallelDosageSequence parallelSeq(lastingDose, Common::Duration(std::chrono::hours(0)));

    Common::DateTime startDate("2018-07-06T08:00:00", TestUtils::date_format);
    Common::DateTime endDate("2018-07-08T08:00:00", TestUtils::date_format);

    xpertRequestResult.getModifiableTreatment()->addDosageTimeRange(
            std::make_unique<Core::DosageTimeRange>(startDate, endDate, parallelSeq));

    // Execute
    TestUtils::flowStepProvider.getDoseValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getDoseValidationResults().size(), 1);
    EXPECT_EQ(
            xpertRequestResult.getDoseValidationResults().begin()->second.getWarning(),
            "Minimum recommended dosage reached (100.00 mg)");
}

} // namespace Xpert
} // namespace Tucuxi
