// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/query/xpertquerytocoreextractor.h" // Source file to be tested

/// \brief Gtest of xpertquerytocoreextractor.
///        This struct tests only the methods created by XpertQueryToCoreExtractor.
///        Therefore, it doesn't test the inherited methods. Here, to goal is to check
///        that the returned drugTreatment is not null and that the error message is empty string.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined functions for the tests          *
 ***********************************************************/

// extractDrugTreatment fixture
class DrugTreatmentExtractorTest : public testing::Test
{
public:
    std::string xmlStringValid = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                            </xpertRequest>
                                        </requests>
                                    </query>
                                    )";

    std::string xmlStringInvalid = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                            <xpertRequest>
                                                <drugId>imatinib</drugId>
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

/// \brief Test cases for the extractDrugTreatment method.
///        - Check that the drug treatment is normally extracted for
///        the xpertRequest that requires a valid drug. A drug is valid
///        if it appears only once.
///        There are two xpertRequests.
///        One for imatinib and one for rifampicin. There is exactly
///        one drug element for each of these drugs. The extraction
///        must succeed without any error and the treatment pointers are not nullptr.
///        - When there is no drug or multiple drugs that match a requested drug, it checks
///        that an error message is returned and that the drugTreatment pointers are nullptr.
///        There are two xpertRequests.
///        One for rifampicin and one for imatinib.
///        In this case, rifampicin is present twice and imatinib none.
/// \param none but uses the related fixture
TEST_F(DrugTreatmentExtractorTest, SuccessfulExtractor)
{
    // Extract the treatments
    unique_ptr<XpertQueryData> query = nullptr;

    XpertQueryImport importer;
    XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringValid);

    XpertQueryToCoreExtractor extractor;

    std::string errorMessage0;
    unique_ptr<Core::DrugTreatment> drugTreatment0 =
            extractor.extractDrugTreatment(query->getXpertRequests()[0].get(), *query, errorMessage0);

    std::string errorMessage1;
    unique_ptr<Core::DrugTreatment> drugTreatment1 =
            extractor.extractDrugTreatment(query->getXpertRequests()[1].get(), *query, errorMessage1);

    EXPECT_EQ(errorMessage0, "");
    EXPECT_EQ(errorMessage1, "");
    EXPECT_EQ(importResult, Xpert::XpertQueryImport::Status::Ok);
    EXPECT_NE(drugTreatment0, nullptr);
    EXPECT_NE(drugTreatment1, nullptr);
}

TEST_F(DrugTreatmentExtractorTest, FailedExtractor)
{
    // Extract the treatments
    unique_ptr<Xpert::XpertQueryData> query = nullptr;

    Xpert::XpertQueryImport importer;
    Xpert::XpertQueryImport::Status importResult = importer.importFromString(query, xmlStringInvalid);

    Xpert::XpertQueryToCoreExtractor extractor;

    std::string errorMessage0;
    unique_ptr<Core::DrugTreatment> drugTreatment0 =
            extractor.extractDrugTreatment(query->getXpertRequests()[0].get(), *query, errorMessage0);

    std::string errorMessage1;
    unique_ptr<Core::DrugTreatment> drugTreatment1 =
            extractor.extractDrugTreatment(query->getXpertRequests()[1].get(), *query, errorMessage1);


    EXPECT_EQ(importResult, Xpert::XpertQueryImport::Status::Ok);
    EXPECT_EQ(errorMessage0, "Too many drugs matching. Could not extract drug treatment.");
    EXPECT_EQ(errorMessage1, "No drug matching. Could not extract drug treatment.");
    EXPECT_EQ(drugTreatment0, nullptr);
    EXPECT_EQ(drugTreatment1, nullptr);
}

} // namespace Xpert
} // namespace Tucuxi
