// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/query/xpertquerydata.h"
#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of xpertqueryresult.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

// XpertQueryResult class and methods fixture
class XpertQueryResultTest : public testing::Test
{
public:
    std::string xmlString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-11T13:45:30</date>

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
                                                <drugId>rifampicin</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                <output>
                                                    <format>xml</format>
                                                    <language>en</language>
                                                </output>
                                            </xpertRequest>
                                            <xpertRequest>
                                                <drugId>imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                <output>
                                                    <format>xml</format>
                                                    <language>en</language>
                                                </output>
                                                <adjustmentDate>2018-08-06T08:00:00</adjustmentDate>
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string emptyAdminString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                            <query version="1.0"
                                                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                                xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                                <date>2018-07-11T13:45:30</date>

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

    std::string fullAdminString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                        <clinicalData key="goodNote"> nice </clinicalData>
                                                        <clinicalData key="badNote"> <yet>random note</yet> </clinicalData>
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
                                                    </xpertRequest>
                                                </requests>
                                            </query>
                                            )";

    std::string queryStringCorrectValuesPart = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                            <query version="1.0"
                                                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                                xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                                <date>2018-07-11T13:45:30</date>

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
                                                    <xpertRequest>
                                                        <drugId>imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                        <output>
                                                            <format>xml</format>
                                                            <language>en</language>
                                                        </output>
                                                        <adjustmentDate>2018-08-06T08:00:00</adjustmentDate>
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


    std::string queryStringCorrectValuesFull = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                            <query version="1.0"
                                                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                                xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                                <date>2018-07-11T13:45:30</date>

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
                                                            <formulationAndRouteSelectionOption>defaultFormulationAndRoute</formulationAndRouteSelectionOption>
                                                        </options>
                                                    </xpertRequest>
                                                    <xpertRequest>
                                                        <drugId>imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                        <output>
                                                            <format>html</format>
                                                            <language>fr</language>
                                                        </output>
                                                    </xpertRequest>
                                                </requests>
                                            </query>
                                            )";

    std::string queryStringCorrectDateTime = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                     <query version="1.0"
                                         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                         xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                         <date>2018-07-11T13:45:30</date>

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

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the XpertQueryResult class and methods method.
///        Check that the ownership of the xpertQueryData is taken by XpertRequestResult.
///        The test succeeds if the XpertQueryData pointer is not nullptr before the
///        XpertQueryResult construction and if the XpertQueryData pointer is nullptr
///        after the XpertQueryResult construction.
/// \param none but uses the related fixture
TEST_F(XpertQueryResultTest, XpertQueryDataOwnershipTaker)
{
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlString);

    // Must not be nullptr.
    EXPECT_NE(query, nullptr);

    XpertQueryResult xpertQueryResult(std::move(query), "");

    // Compare
    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);
    EXPECT_EQ(query, nullptr);
}

/// \brief Test cases for the XpertQueryResult class and methods method.
///        Check that the admin data are correctly retrieved.
///        The test imports two xpert queries.
///        The first one has no admin element, but still the import works and leaves no lingering nullptr.
///        The second one has a full admin element. When using XpertQueryResult::getAdminData, all
///        values of the admin element can be retrieved.
/// \param none but uses the related fixture
TEST_F(XpertQueryResultTest, AdminDataGetter)
{
    // Import the queries

    unique_ptr<XpertQueryData> queryEmptyAdmin = nullptr;
    unique_ptr<XpertQueryData> queryCompleteAdmin = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResultEmptyAdmin = importer.importFromString(queryEmptyAdmin, emptyAdminString);
    XpertQueryImport::Status importResultFullAdmin = importer.importFromString(queryCompleteAdmin, fullAdminString);

    // Execute

    XpertQueryResult xpertQueryResultWithEmptyAdmin(move(queryEmptyAdmin), "");
    XpertQueryResult xpertQueryResultWithFullAdmin(move(queryCompleteAdmin), "");

    // Compare

    EXPECT_EQ(importResultEmptyAdmin, XpertQueryImport::Status::Ok);
    EXPECT_EQ(importResultFullAdmin, XpertQueryImport::Status::Ok);

    EXPECT_NE(xpertQueryResultWithEmptyAdmin.getAdminData(), nullptr);
    EXPECT_NE(xpertQueryResultWithFullAdmin.getAdminData(), nullptr);

    const PersonData& mandator = xpertQueryResultWithFullAdmin.getAdminData()->getMandator()->getPerson();
    const AddressData& mandatorAddress = *mandator.getAddress();
    const PhoneData& mandatorPhone = *mandator.getPhone();
    const EmailData& mandatorEmail = *mandator.getEmail();
    const InstituteData& mandatorInstitute =
            *xpertQueryResultWithFullAdmin.getAdminData()->getMandator()->getInstitute();
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

    const PersonData& patient = xpertQueryResultWithFullAdmin.getAdminData()->getPatient()->getPerson();
    const AddressData& patientAddress = *patient.getAddress();
    const PhoneData& patientPhone = *patient.getPhone();
    const EmailData& patientEmail = *patient.getEmail();
    const InstituteData& patientInstitute = *xpertQueryResultWithFullAdmin.getAdminData()->getPatient()->getInstitute();
    //  XXXXX does not work
    // const Data& patientInstituteAddress = *patientInstitute.getAddress();
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
    //  XXXXX does not work
    // EXPECT_EQ(patientInstituteAddress.getStreet(), "Street name 2");
    // EXPECT_EQ(patientInstituteAddress.getPostalCode(), "1400");
    // EXPECT_EQ(patientInstituteAddress.getCity(), "Yverdon-les-Bains");
    // EXPECT_EQ(patientInstituteAddress.getState(), "Vaud");
    // EXPECT_EQ(patientInstituteAddress.getCountry(), "Suisse");
    EXPECT_EQ(patientInstitutePhone.getNumber(), "0123456789");
    EXPECT_EQ(patientInstitutePhone.getType(), "professional");
    EXPECT_EQ(patientInstituteEmail.getAddress(), "info@ehnv.com");
    EXPECT_EQ(patientInstituteEmail.getType(), "professional");

    EXPECT_EQ(
            xpertQueryResultWithFullAdmin.getAdminData()->getClinicalDatas()->getData().find("goodNote")->second,
            " nice ");
    EXPECT_EQ(xpertQueryResultWithFullAdmin.getAdminData()->getClinicalDatas()->getData().find("badNote")->second, "");
}

/// \brief Test cases for the XpertQueryResult class and methods method.
///        Check that the XpertRequestResult are correctly created.
///        The query has two xpertRequests. One if for imatinib and the other
///        is for rifampicin.
///
///        Rifampicin:
///           - The XpertRequestResult has no errors and can still be processed.
///           - The treatment is not nullptr.
///           - The drug model is nullptr.
///
///        Imatinib:
///           - The XpertRequestResult has an error because the query doesn't have a
///             drug element for imatinib.
///           - It should no longer be processed.
///           - The treatment is nullptr.
///           - The drug model is nullptr.
/// \param none but uses the related fixture
TEST_F(XpertQueryResultTest, XpertRequestResultGetter)
{
    // Import the query
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, queryStringCorrectValuesPart);

    // Execute
    XpertQueryResult xpertQueryResult(move(query), "");

    // Compare
    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);

    EXPECT_EQ(xpertQueryResult.getXpertRequestResults().size(), 2);

    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[0].getDrugModel(), nullptr);
    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[0].getErrorMessage(), "");
    EXPECT_NE(xpertQueryResult.getXpertRequestResults()[0].getTreatment(), nullptr);
    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[0].getXpertRequest().getDrugId(), "rifampicin");
    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[0].shouldContinueProcessing(), true);

    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[1].getDrugModel(), nullptr);
    EXPECT_EQ(
            xpertQueryResult.getXpertRequestResults()[1].getErrorMessage(),
            "No drug matching. Could not extract drug treatment.");
    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[1].getTreatment(), nullptr);
    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[1].getXpertRequest().getDrugId(), "imatinib");
    EXPECT_EQ(xpertQueryResult.getXpertRequestResults()[1].shouldContinueProcessing(), false);
}

/// \brief Test cases for the XpertQueryResult class and methods method.
///        Check that the xpertRequest data are correctly retrieved in the XpertRequestResult.
///        The test imports a query with two xpertRequest elements.
///        The first element is a complete xpertRequest. All values must be retrieved correctly.
///        The second element is a minimal xpertRequest. All values must be retrieved correctly, but
///        the missing values have default values.
/// \param none but uses the related fixture
TEST_F(XpertQueryResultTest, XpertRequestDataRetriever)
{
    // Import the query
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, queryStringCorrectValuesFull);

    // Execute
    XpertQueryResult xpertQueryResult(move(query), "");

    // Compare
    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);

    const XpertRequestData& completeXpertRequest = xpertQueryResult.getXpertRequestResults()[0].getXpertRequest();
    const XpertRequestData& minimalXpertRequest = xpertQueryResult.getXpertRequestResults()[1].getXpertRequest();

    EXPECT_EQ(completeXpertRequest.getDrugId(), "rifampicin");
    EXPECT_EQ(completeXpertRequest.getOutputLang() == OutputLang::ENGLISH, true);
    EXPECT_EQ(completeXpertRequest.getOutputFormat() == OutputFormat::XML, true);
    EXPECT_EQ(
            completeXpertRequest.getAdjustmentTime(), Common::DateTime("2018-07-06T08:00:00", TestUtils::date_format));
    EXPECT_EQ(completeXpertRequest.getLoadingOption() == LoadingOption::NoLoadingDose, true);
    EXPECT_EQ(completeXpertRequest.getRestPeriodOption() == RestPeriodOption::NoRestPeriod, true);
    EXPECT_EQ(completeXpertRequest.getTargetExtractionOption() == Core::TargetExtractionOption::PopulationValues, true);
    EXPECT_EQ(
            completeXpertRequest.getFormulationAndRouteSelectionOption()
                    == Core::FormulationAndRouteSelectionOption::DefaultFormulationAndRoute,
            true);


    EXPECT_EQ(minimalXpertRequest.getDrugId(), "imatinib");
    EXPECT_EQ(minimalXpertRequest.getOutputLang() == OutputLang::FRENCH, true);
    EXPECT_EQ(minimalXpertRequest.getOutputFormat() == OutputFormat::HTML, true);
    EXPECT_EQ(minimalXpertRequest.getAdjustmentTime().isUndefined(), true);
    EXPECT_EQ(minimalXpertRequest.getLoadingOption() == LoadingOption::Unspecified, true);
    EXPECT_EQ(minimalXpertRequest.getRestPeriodOption() == RestPeriodOption::Unspecified, true);
    EXPECT_EQ(
            minimalXpertRequest.getTargetExtractionOption()
                    == Core::TargetExtractionOption::DefinitionIfNoIndividualTarget,
            true);
    EXPECT_EQ(
            minimalXpertRequest.getFormulationAndRouteSelectionOption()
                    == Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute,
            true);
}

/// \brief Test cases for the XpertQueryResult class and methods method.
///        Check that computation time of the XpertRequestResult is retrieved from the xpert query.
///        At top of the xpert query, the date element is "2018-07-11T13:45:30".
///        XpertQueryResult::getComputationTime should return the equivalent date of "2018-07-11T13:45:30".
/// \param none but uses the related fixture
TEST_F(XpertQueryResultTest, ComputationTimeRetriever)
{
    // Import the query
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, queryStringCorrectDateTime);

    // Execute
    XpertQueryResult xpertQueryResult(move(query), "");

    // Compare
    EXPECT_EQ(importResult, XpertQueryImport::Status::Ok);
    EXPECT_EQ(xpertQueryResult.getComputationTime(), Common::DateTime("2018-07-11T13:45:30", TestUtils::date_format));
}

} // namespace Xpert
} // namespace Tucuxi
