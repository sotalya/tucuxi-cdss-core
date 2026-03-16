// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/query/xpertrequestdata.h"

/// \brief Gtest of XpertQueryImport.
///        Tests for XpertQueryImport from the GeneralXpertFlowStepProvider.
///        The tests use the AdjustmentTraitCreator object and assume
///        that this object works as intended.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

// XpertQueryImport fixture
class XpertQueryImportTest : public testing::Test
{
public:
    std::string xmlStringAllValuesCompleteAdmin = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                   <query version="1.0"
                                       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                       xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                       <date>2018-07-11T13:45:30</date>

                                       <admin>
                                           <mandator>
                                               <person>
                                                   <id>asdf</id>
                                                   <title>Dr.</title>
                                                   <firstName>John</firstName>
                                                   <lastName>Doe</lastName>
                                                   <address>
                                                       <street>Av. de l'Ours 2</street>
                                                       <postalCode>1010</postalCode>
                                                       <city>Lausanne</city>
                                                       <state>Vaud</state>
                                                       <country>Suisse</country>
                                                   </address>
                                                   <phone>
                                                       <number>0213140002</number>
                                                       <type>professional</type>
                                                   </phone>
                                                   <email>
                                                       <address>john.doe@chuv.com</address>
                                                       <type>professional</type>
                                                   </email>
                                               </person>
                                               <institute>
                                                   <id>456789</id>
                                                   <name>CHUV</name>
                                                   <address>
                                                       <street>Av. de l'Ours 1</street>
                                                       <postalCode>1010</postalCode>
                                                       <city>Lausanne</city>
                                                       <state>Vaud</state>
                                                       <country>Suisse</country>
                                                   </address>
                                                   <phone>
                                                       <number>0213140001</number>
                                                       <type>professional</type>
                                                   </phone>
                                                   <email>
                                                       <address>info@chuv.com</address>
                                                       <type>professional</type>
                                                   </email>
                                               </institute>
                                           </mandator>
                                           <patient>
                                               <person>
                                                   <id>123456</id>
                                                   <firstName>Alice</firstName>
                                                   <lastName>Aupaysdesmerveilles</lastName>
                                                   <address>
                                                       <street>Av. d'Ouchy 27</street>
                                                       <postalCode>1006</postalCode>
                                                       <city>Lausanne</city>
                                                       <state>Vaud</state>
                                                       <country>Suisse</country>
                                                   </address>
                                                   <phone>
                                                       <number>0216170002</number>
                                                       <type>professional</type>
                                                   </phone>
                                                   <email>
                                                       <address>alice.apdm@gmail.com</address>
                                                       <type>private</type>
                                                   </email>
                                               </person>
                                               <institute>
                                                   <id>1234</id>
                                                   <name>EHNV</name>
                                                   <address>
                                                       <street>Street name 2</street>
                                                       <postalCode>1400</postalCode>
                                                       <city>Yverdon-les-Bains</city>
                                                       <state>Vaud</state>
                                                       <country>Suisse</country>
                                                   </address>
                                                   <phone>
                                                       <number>0123456789</number>
                                                       <type>professional</type>
                                                   </phone>
                                                   <email>
                                                       <address>info@ehnv.com</address>
                                                       <type>professional</type>
                                                   </email>
                                               </institute>
                                           </patient>
                                           <clinicalDatas>
                                               <clinicalData key="goodNote"> nice </clinicalDataEntry>
                                               <clinicalData key="badNote"> <yet>random note</yet> </clinicalDataEntry>
                                           </clinicalDatas>
                                       </admin>

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
                                                   <drugId>rifampicin</drugId>
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
                                               <drugId>rifampicin</drugId>
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

    std::string xmlStringNoAdminElement = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringEmptyAdminElement = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin></admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingValuesPerson = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin>
                                            <mandator>
                                                <person>
                                                    <firstName>John</firstName>
                                                    <lastName>Doe</lastName>
                                                </person>
                                            </mandator>
                                            <patient>
                                                <person>
                                                    <firstName>Alice</firstName>
                                                    <lastName>Aupaysdesmerveilles</lastName>
                                                </person>
                                            </patient>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingValuesInstitute = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin>
                                            <mandator>
                                                <person>
                                                    <firstName>John</firstName>
                                                    <lastName>Doe</lastName>
                                                </person>
                                                <institute>
                                                    <name>CHUV</name>
                                                </institute>
                                            </mandator>
                                            <patient>
                                                <person>
                                                    <firstName>Alice</firstName>
                                                    <lastName>Aupaysdesmerveilles</lastName>
                                                </person>
                                                <institute>
                                                    <name>EHNV</name>
                                                </institute>
                                            </patient>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingValuesAddress = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->
                                        <language>en</language>

                                        <admin>
                                            <mandator>
                                                <person>
                                                    <firstName>John</firstName>
                                                    <lastName>Doe</lastName>
                                                    <address>
                                                        <street>Av. de l'Ours 2</street>
                                                        <postalCode>1010</postalCode>
                                                        <city>Lausanne</city>
                                                    </address>
                                                    <phone>
                                                        <number>0213140002</number>
                                                    </phone>
                                                    <email>
                                                        <address>john.doe@chuv.com</address>
                                                    </email>
                                                </person>
                                                <institute>
                                                    <name>CHUV</name>
                                                    <address>
                                                        <street>Av. de l'Ours 1</street>
                                                        <postalCode>1010</postalCode>
                                                        <city>Lausanne</city>
                                                    </address>
                                                    <phone>
                                                        <number>0213140001</number>
                                                    </phone>
                                                    <email>
                                                        <address>info@chuv.com</address>
                                                    </email>
                                                </institute>
                                            </mandator>
                                            <patient>
                                                <person>
                                                    <firstName>Alice</firstName>
                                                    <lastName>Aupaysdesmerveilles</lastName>
                                                    <address>
                                                        <street>Av. d'Ouchy 27</street>
                                                        <postalCode>1006</postalCode>
                                                        <city>Lausanne</city>
                                                    </address>
                                                    <phone>
                                                        <number>0216170002</number>
                                                    </phone>
                                                    <email>
                                                        <address>alice.apdm@gmail.com</address>
                                                    </email>
                                                </person>
                                                <institute>
                                                    <id>1234</id>
                                                    <name>EHNV</name>
                                                    <address>
                                                        <street>Street name 2</street>
                                                        <postalCode>1400</postalCode>
                                                        <city>Yverdon-les-Bains</city>
                                                    </address>
                                                    <phone>
                                                        <number>0123456789</number>
                                                    </phone>
                                                    <email>
                                                        <address>info@ehnv.com</address>
                                                    </email>
                                                </institute>
                                            </patient>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingMandatoryValuesPerson = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin>
                                            <mandator>
                                                <person>
                                                    <address>
                                                    </address>
                                                    <phone>
                                                    </phone>
                                                    <email>
                                                    </email>
                                                </person>
                                            </mandator>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingMandatoryValuesInstitute = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin>
                                            <mandator>
                                                <person>
                                                    <firstName>John</firstName>
                                                    <lastName>Doe</lastName>
                                                    <address>
                                                        <street>Av. de l'Ours 2</street>
                                                        <postalCode>1010</postalCode>
                                                        <city>Lausanne</city>
                                                    </address>
                                                    <phone>
                                                        <number>0213140002</number>
                                                    </phone>
                                                    <email>
                                                        <address>john.doe@chuv.com</address>
                                                    </email>
                                                </person>
                                                <institute>
                                                    <address>
                                                    </address>
                                                    <phone>
                                                    </phone>
                                                    <email>
                                                    </email>
                                                </institute>
                                            </mandator>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingMandatoryValuesPatientPerson = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin>
                                            <patient>
                                                <person>
                                                    <address>
                                                    </address>
                                                    <phone>
                                                    </phone>
                                                    <email>
                                                    </email>
                                                </person>
                                            </patient>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringMissingMandatoryValuesPatientInstitute =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">
                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <admin>
                                            <patient>
                                                <person>
                                                    <firstName>John</firstName>
                                                    <lastName>Doe</lastName>
                                                    <address>
                                                        <street>Av. de l'Ours 2</street>
                                                        <postalCode>1010</postalCode>
                                                        <city>Lausanne</city>
                                                    </address>
                                                    <phone>
                                                        <number>0213140002</number>
                                                    </phone>
                                                    <email>
                                                        <address>john.doe@chuv.com</address>
                                                    </email>
                                                </person>
                                                <institute>
                                                    <address>
                                                    </address>
                                                    <phone>
                                                    </phone>
                                                    <email>
                                                    </email>
                                                </institute>
                                            </patient>
                                        </admin>

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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                                    <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";



    std::string xmlStringAllValuesCompleteRequest = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
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
                                    </query>
                                    )";


    std::string xmlStringDefaultValuesMinimalRequest = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                    <drugId>rifampicin</drugId>
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
                                                <drugId>rifampicin</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                <output>
                                                    <format>xml</format>
                                                    <language>en</language>
                                                </output>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringNoRequest = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                    <drugId>rifampicin</drugId>
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
                                        </requests>
                                    </query>
                                    )";


    std::string xmlStringMandatoryRequest = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                    <drugId>rifampicin</drugId>
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
                                                        <configId>imatinib.gotta2012</configId>
                                                <adjustmentDate>2018-07-06T08:00:00</adjustmentDate>
                                                <options>
                                                    <loadingOption>noLoadingDose</loadingOption>
                                                    <restPeriodOption>noRestPeriod</restPeriodOption>
                                                    <targetExtractionOption>populationValues</targetExtractionOption>
                                                    <formulationAndRouteSelectionOption>allFormulationAndRoutes</formulationAndRouteSelectionOption>
                                                </options>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";


    std::string xmlStringBadFormat = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <badFormatted>
                                    )";

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/


/// \brief Test cases for XpertQueryImport.
///        Load an xml file with every possible data in admin node
///        and check if the received values are expected. There should be no import errors.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, CompleteAdminXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringAllValuesCompleteAdmin);

    const AdminData& pAdmin = *query->getAdminData();

    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);

    const PersonData& mandator = pAdmin.getMandator()->getPerson();
    const AddressData& mandatorAddress = *mandator.getAddress();
    const PhoneData& mandatorPhone = *mandator.getPhone();
    const EmailData& mandatorEmail = *mandator.getEmail();
    const InstituteData& mandatorInstitute = *pAdmin.getMandator()->getInstitute();
    const AddressData& mandatorInstituteAddress = *mandatorInstitute.getAddress();
    const PhoneData& mandatorInstitutePhone = *mandatorInstitute.getPhone();
    const EmailData& mandatorInstituteEmail = *mandatorInstitute.getEmail();

    EXPECT_EQ(mandator.getId(), "asdf");
    EXPECT_EQ(mandator.getTitle(), "Dr.");
    EXPECT_EQ(mandator.getFirstName(), "John");
    EXPECT_EQ(mandator.getLastName(), "Doe");
    EXPECT_EQ(mandatorAddress.getStreet(), "Av. de l'Ours 2");
    EXPECT_EQ(mandatorAddress.getPostalCode(), "1010");
    EXPECT_EQ(mandatorAddress.getCity(), "Lausanne");
    EXPECT_EQ(mandatorAddress.getState(), "Vaud");
    EXPECT_EQ(mandatorAddress.getCountry(), "Suisse");
    EXPECT_EQ(mandatorPhone.getNumber(), "0213140002");
    EXPECT_EQ(mandatorPhone.getType(), "professional");
    EXPECT_EQ(mandatorEmail.getAddress(), "john.doe@chuv.com");
    EXPECT_EQ(mandatorEmail.getType(), "professional");
    EXPECT_EQ(mandatorInstitute.getId(), "456789");
    EXPECT_EQ(mandatorInstitute.getName(), "CHUV");
    EXPECT_EQ(mandatorInstituteAddress.getStreet(), "Av. de l'Ours 1");
    EXPECT_EQ(mandatorInstituteAddress.getPostalCode(), "1010");
    EXPECT_EQ(mandatorInstituteAddress.getCity(), "Lausanne");
    EXPECT_EQ(mandatorInstituteAddress.getState(), "Vaud");
    EXPECT_EQ(mandatorInstituteAddress.getCountry(), "Suisse");
    EXPECT_EQ(mandatorInstitutePhone.getNumber(), "0213140001");
    EXPECT_EQ(mandatorInstitutePhone.getType(), "professional");
    EXPECT_EQ(mandatorInstituteEmail.getAddress(), "info@chuv.com");
    EXPECT_EQ(mandatorInstituteEmail.getType(), "professional");

    const PersonData& patient = pAdmin.getPatient()->getPerson();
    const AddressData& patientAddress = *patient.getAddress();
    const PhoneData& patientPhone = *patient.getPhone();
    const EmailData& patientEmail = *patient.getEmail();
    const InstituteData& patientInstitute = *pAdmin.getPatient()->getInstitute();
    const AddressData& patientInstituteAddress = *patientInstitute.getAddress();
    const PhoneData& patientInstitutePhone = *patientInstitute.getPhone();
    const EmailData& patientInstituteEmail = *patientInstitute.getEmail();

    EXPECT_EQ(patient.getId(), "123456");
    EXPECT_EQ(patient.getFirstName(), "Alice");
    EXPECT_EQ(patient.getLastName(), "Aupaysdesmerveilles");
    EXPECT_EQ(patientAddress.getStreet(), "Av. d'Ouchy 27");
    EXPECT_EQ(patientAddress.getPostalCode(), "1006");
    EXPECT_EQ(patientAddress.getCity(), "Lausanne");
    EXPECT_EQ(patientAddress.getState(), "Vaud");
    EXPECT_EQ(patientAddress.getCountry(), "Suisse");
    EXPECT_EQ(patientPhone.getNumber(), "0216170002");
    EXPECT_EQ(patientPhone.getType(), "professional");
    EXPECT_EQ(patientEmail.getAddress(), "alice.apdm@gmail.com");
    EXPECT_EQ(patientEmail.getType(), "private");
    EXPECT_EQ(patientInstitute.getId(), "1234");
    EXPECT_EQ(patientInstitute.getName(), "EHNV");
    EXPECT_EQ(patientInstituteAddress.getStreet(), "Street name 2");
    EXPECT_EQ(patientInstituteAddress.getPostalCode(), "1400");
    EXPECT_EQ(patientInstituteAddress.getCity(), "Yverdon-les-Bains");
    EXPECT_EQ(patientInstituteAddress.getState(), "Vaud");
    EXPECT_EQ(patientInstituteAddress.getCountry(), "Suisse");
    EXPECT_EQ(patientInstitutePhone.getNumber(), "0123456789");
    EXPECT_EQ(patientInstitutePhone.getType(), "professional");
    EXPECT_EQ(patientInstituteEmail.getAddress(), "info@ehnv.com");
    EXPECT_EQ(patientInstituteEmail.getType(), "professional");

    EXPECT_EQ(pAdmin.getClinicalDatas()->getData().find("goodNote")->second, " nice ");
    EXPECT_EQ(pAdmin.getClinicalDatas()->getData().find("badNote")->second, "");
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with no admin element.
///        The admin data pointer should be valid and there should be no import errors.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, NoAdminXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringNoAdminElement);


    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);
    EXPECT_NE(query->getAdminData(), nullptr);
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element that has no mandator, no patient and no clinical data.
///        There should be no import errors.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, EmptyAdminXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringEmptyAdminElement);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);
    EXPECT_NE(query->getAdminData(), nullptr);

    EXPECT_NE(query->getAdminData()->getMandator(), nullptr);
    EXPECT_NE(query->getAdminData()->getPatient(), nullptr);
    EXPECT_NE(query->getAdminData()->getClinicalDatas(), nullptr);
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element that has minimal person elements in
///        mandator and patient. There should be no import errors.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MinimalPersonInMandatorAndPatientXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringMissingValuesPerson);

    const AdminData* admin = query->getAdminData();

    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);
    EXPECT_NE(admin, nullptr);

    const PersonData& mandator = admin->getMandator()->getPerson();
    const PersonData& patient = admin->getPatient()->getPerson();


    EXPECT_EQ(mandator.getId(), "");
    EXPECT_EQ(mandator.getTitle(), "");
    EXPECT_NE(mandator.getAddress(), nullptr);
    EXPECT_NE(mandator.getPhone(), nullptr);
    EXPECT_NE(mandator.getEmail(), nullptr);
    EXPECT_NE(admin->getMandator()->getInstitute(), nullptr);

    EXPECT_EQ(patient.getId(), "");
    EXPECT_EQ(patient.getTitle(), "");
    EXPECT_NE(patient.getAddress(), nullptr);
    EXPECT_NE(patient.getPhone(), nullptr);
    EXPECT_NE(patient.getEmail(), nullptr);
    EXPECT_NE(admin->getPatient()->getInstitute(), nullptr);
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element that has minimal institutes elements
///        in mandator and patient elements. There should be no import errors.
///        The missing values must be nullptr or empty string.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MinimalInstituteInMandatorAndPatientXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringMissingValuesInstitute);

    const AdminData* admin = query->getAdminData();

    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);
    EXPECT_NE(admin, nullptr);

    const InstituteData* mandatorInstitute = admin->getMandator()->getInstitute();
    const InstituteData* patientInstitute = admin->getPatient()->getInstitute();


    EXPECT_EQ(mandatorInstitute->getId(), "");
    EXPECT_NE(mandatorInstitute->getAddress(), nullptr);
    EXPECT_NE(mandatorInstitute->getPhone(), nullptr);
    EXPECT_NE(mandatorInstitute->getEmail(), nullptr);

    EXPECT_EQ(patientInstitute->getId(), "");
    EXPECT_NE(patientInstitute->getAddress(), nullptr);
    EXPECT_NE(patientInstitute->getPhone(), nullptr);
    EXPECT_NE(patientInstitute->getEmail(), nullptr);
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element that has address phone and email in institute
///        and person elements. There should be no import errors. The missing values must empty string.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MinimalAddressPhoneAndEmailXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringMissingValuesAddress);

    const AdminData* pAdmin = query->getAdminData();

    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);

    const PersonData& mandator = pAdmin->getMandator()->getPerson();
    const AddressData& mandatorAddress = *mandator.getAddress();
    const PhoneData& mandatorPhone = *mandator.getPhone();
    const EmailData& mandatorEmail = *mandator.getEmail();
    const InstituteData& mandatorInstitute = *pAdmin->getMandator()->getInstitute();
    const AddressData& mandatorInstituteAddress = *mandatorInstitute.getAddress();
    const PhoneData& mandatorInstitutePhone = *mandatorInstitute.getPhone();
    const EmailData& mandatorInstituteEmail = *mandatorInstitute.getEmail();

    const PersonData& patient = pAdmin->getPatient()->getPerson();
    const AddressData& patientAddress = *patient.getAddress();
    const PhoneData& patientPhone = *patient.getPhone();
    const EmailData& patientEmail = *patient.getEmail();
    const InstituteData& patientInstitute = *pAdmin->getPatient()->getInstitute();
    const AddressData& patientInstituteAddress = *patientInstitute.getAddress();
    const PhoneData& patientInstitutePhone = *patientInstitute.getPhone();
    const EmailData& patientInstituteEmail = *patientInstitute.getEmail();

    EXPECT_EQ(mandatorAddress.getState(), "");
    EXPECT_EQ(mandatorInstituteAddress.getState(), "");
    EXPECT_EQ(patientAddress.getState(), "");
    EXPECT_EQ(patientInstituteAddress.getState(), "");

    EXPECT_EQ(mandatorAddress.getCountry(), "");
    EXPECT_EQ(mandatorInstituteAddress.getCountry(), "");
    EXPECT_EQ(patientAddress.getCountry(), "");
    EXPECT_EQ(patientInstituteAddress.getCountry(), "");

    EXPECT_EQ(mandatorPhone.getType(), "");
    EXPECT_EQ(mandatorInstitutePhone.getType(), "");
    EXPECT_EQ(patientPhone.getType(), "");
    EXPECT_EQ(patientInstitutePhone.getType(), "");

    EXPECT_EQ(mandatorEmail.getType(), "");
    EXPECT_EQ(mandatorInstituteEmail.getType(), "");
    EXPECT_EQ(patientEmail.getType(), "");
    EXPECT_EQ(patientInstituteEmail.getType(), "");
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element but the mandatory values are missing in the mandator person.
///        There should be an import error. The error message contains all the elements missing.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MissingMandatoryValuesInCompleteMandatorPersonXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringMissingMandatoryValuesPerson);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Error);
    EXPECT_NE(importer.getErrorMessage().find("firstName"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("lastName"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("street"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("postalCode"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("city"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("number"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("address"), std::string::npos);
}

/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element but the mandatory values are missing in the mandator institute.
///        There should be an import error. The error message contains all the elements missing.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MissingMandatoryValuesInCompleteMandatorInstituteXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringMissingMandatoryValuesInstitute);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Error);
    EXPECT_NE(importer.getErrorMessage().find("name"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("street"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("postalCode"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("city"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("number"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("address"), std::string::npos);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element but the mandatory values are missing in the patient person.
///        There should be an import error. The error message contains all the elements missing.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MissingMandatoryValuesInCompletePatientPersonXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult =
            importer.importFromString(query, xmlStringMissingMandatoryValuesPatientPerson);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Error);
    EXPECT_NE(importer.getErrorMessage().find("firstName"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("lastName"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("street"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("postalCode"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("city"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("number"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("address"), std::string::npos);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file with an admin element but the mandatory values are missing in the patient institute.
///        There should be an import error. The error message contains all the elements missing.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MissingMandatoryValuesInCompletePatientInstituteXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult =
            importer.importFromString(query, xmlStringMissingMandatoryValuesPatientInstitute);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Error);
    EXPECT_NE(importer.getErrorMessage().find("name"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("street"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("postalCode"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("city"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("number"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("address"), std::string::npos);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file with every possible data in xpertRequest node
///        and check if the received values are expected. There should be no import errors.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, CompleteXpertRequestXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringAllValuesCompleteRequest);


    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);

    EXPECT_EQ(query->getXpertRequests().size(), 1);

    const XpertRequestData& xpertRequest = *(query->getXpertRequests()[0]);

    EXPECT_EQ(xpertRequest.getDrugId(), "rifampicin");
    EXPECT_EQ(xpertRequest.getOutputFormat() == OutputFormat::XML, true);
    EXPECT_EQ(xpertRequest.getOutputLang() == OutputLang::ENGLISH, true);
    EXPECT_EQ(xpertRequest.getAdjustmentTime(), DateTime("2018-07-06T08:00:00", "%Y-%m-%dT%H:%M:%S"));
    EXPECT_EQ(xpertRequest.getLoadingOption() == LoadingOption::NoLoadingDose, true);
    EXPECT_EQ(xpertRequest.getRestPeriodOption() == RestPeriodOption::NoRestPeriod, true);
    EXPECT_EQ(xpertRequest.getTargetExtractionOption() == Core::TargetExtractionOption::PopulationValues, true);
    EXPECT_EQ(
            xpertRequest.getFormulationAndRouteSelectionOption()
                    == Core::FormulationAndRouteSelectionOption::AllFormulationAndRoutes,
            true);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file with the default data in an xpertRequest node
///        and check if the received values are expected. There should be no import errors.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MinimalXpertRequestXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringDefaultValuesMinimalRequest);


    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);

    EXPECT_EQ(query->getXpertRequests().size(), 1);

    const XpertRequestData& xpertRequest = *(query->getXpertRequests()[0]);


    EXPECT_EQ(xpertRequest.getAdjustmentTime(), Common::DateTime::undefinedDateTime());
    EXPECT_EQ(xpertRequest.getLoadingOption() == LoadingOption::Unspecified, true);
    EXPECT_EQ(xpertRequest.getRestPeriodOption() == RestPeriodOption::Unspecified, true);
    EXPECT_EQ(
            xpertRequest.getTargetExtractionOption() == Core::TargetExtractionOption::DefinitionIfNoIndividualTarget,
            true);
    EXPECT_EQ(
            xpertRequest.getFormulationAndRouteSelectionOption()
                    == Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute,
            true);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file without any xpertRequest and check that an error is returned.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, xpertQueryImport_importError_withoutXpertRequest)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringNoRequest);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Error);
    EXPECT_NE(importer.getErrorMessage().find("No xpertRequest found"), std::string::npos);
    EXPECT_EQ(query->getXpertRequests().size(), 0);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file without mandatory values in the xpertRequest node and check
///        that error is returned. The error message tells what is missing.
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, MissingMandatoryInXpertRequestXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringMandatoryRequest);

    EXPECT_EQ(importResult, XpertQueryImport::Status::Error);
    EXPECT_NE(importer.getErrorMessage().find("drugId"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("format"), std::string::npos);
    EXPECT_NE(importer.getErrorMessage().find("language"), std::string::npos);
}


/// \brief Test cases for XpertQueryImport.
///        Load an xml file not well formatted.
///        There is an import error and the import status is "CantCreateXmlDocument".
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, FileBadlyFormattedXmlLoader)
{

    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringBadFormat);

    EXPECT_EQ(importResult, XpertQueryImport::Status::CantCreateXmlDocument);
}


/// \brief Test cases for XpertQueryImport.
///        Try to open a file that does not exist.
///        There is an import error and the import status is "CantOpenFile".
/// \param none but uses the related fixture
TEST_F(XpertQueryImportTest, NonExistingFileXmlLoader)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromFile(query, "this file is not existing.xml");

    EXPECT_EQ(importResult, XpertQueryImport::Status::CantOpenFile);
}

} // namespace Xpert
} // namespace Tucuxi
