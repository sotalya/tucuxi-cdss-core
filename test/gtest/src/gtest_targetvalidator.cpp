// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/flow/general/targetvalidator.h" // Source file to be tested
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of TargetValidator.
///        Tests for TargetValidator from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined functions for the tests          *
 ***********************************************************/

// TargetValidator::perform fixture
class TargetValidatorTest : public testing::Test
{
public:
    std::string queryStringSuccessDifferentType = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                        <target>
                                                            <activeMoietyId>imatinib</activeMoietyId>
                                                            <targetType>mean</targetType>
                                                            <unit>mg/l</unit>
                                                            <min>20</min>
                                                            <best>25</best>
                                                            <max>30</max>
                                                            <inefficacyAlarm>15</inefficacyAlarm>
                                                            <toxicityAlarm>50</toxicityAlarm>
                                                        </target>
                                                        <target>
                                                            <activeMoietyId>imatinib</activeMoietyId>
                                                            <targetType>residual</targetType>
                                                            <unit>mg/l</unit>
                                                            <min>30</min>
                                                            <best>35</best>
                                                            <max>40</max>
                                                            <inefficacyAlarm>25</inefficacyAlarm>
                                                            <toxicityAlarm>60</toxicityAlarm>
                                                        </target>
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


    std::string queryStringSuccessNoTarget = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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

    std::string queryStringSameType = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                        <target>
                                                            <activeMoietyId>imatinib</activeMoietyId>
                                                            <targetType>residual</targetType>
                                                            <unit>mg/l</unit>
                                                            <min>20</min>
                                                            <best>25</best>
                                                            <max>30</max>
                                                            <inefficacyAlarm>15</inefficacyAlarm>
                                                            <toxicityAlarm>50</toxicityAlarm>
                                                        </target>
                                                        <target>
                                                            <activeMoietyId>imatinib</activeMoietyId>
                                                            <targetType>residual</targetType>
                                                            <unit>mg/l</unit>
                                                            <min>30</min>
                                                            <best>35</best>
                                                            <max>40</max>
                                                            <inefficacyAlarm>25</inefficacyAlarm>
                                                            <toxicityAlarm>60</toxicityAlarm>
                                                        </target>
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


    std::string queryStringNotInDrugModel = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                        <target>
                                                            <activeMoietyId>randomactivemoiety</activeMoietyId>
                                                            <targetType>mean</targetType>
                                                            <unit>mg/l</unit>
                                                            <min>20</min>
                                                            <best>25</best>
                                                            <max>30</max>
                                                            <inefficacyAlarm>15</inefficacyAlarm>
                                                            <toxicityAlarm>50</toxicityAlarm>
                                                        </target>
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

/// \brief Test cases for the TargetValidator::perform method.
///        This method performs a target validation with a treatment that specifes custom targets
///        that have the same active moiety but a different target type.
///        The XpertRequestResult must not contain any errors and shouldContinueProcessing must return true.
/// \param none but uses the related fixture
TEST_F(TargetValidatorTest, SuccessDifferentTypeValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<Xpert::XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccessDifferentType, TestUtils::originalImatinibModelString, xpertQueryResult);

    Xpert::XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getTargetValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
}

/// \brief Test cases for the TargetValidator::perform method.
///        This method performs a target validation with a treatment that does not specify custom targets.
///        XpertRequestResult::shouldBeProcessed must return true.
/// \param none but uses the related fixture
TEST_F(TargetValidatorTest, SuccessNoTargetValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<Xpert::XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccessNoTarget, TestUtils::originalImatinibModelString, xpertQueryResult);

    Xpert::XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getTargetValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
}

/// \brief Test cases for the TargetValidator::perform method.
///        The test loads an xpertRequest without assigning a drug model to it.
///        The TargetValidator must set the error in the XpertRequestResult and
///        shouldBeProcessed must return false.
/// \param none but uses the related fixture
TEST_F(TargetValidatorTest, NoDrugModelValidator)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDrugModel, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getTargetValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No drug model set.");
}

/// \brief Test cases for the TargetValidator::perform method.
///        Check that there is an error if the treatment of an XpertRequestResult is nullptr
///        in TargetValidator.
///        The TargetValidator must set the error in the XpertRequestResult and
///        shouldBeProcessed must return false.
/// \param none but uses the related fixture
TEST_F(TargetValidatorTest, NoTreatmentValidator)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoTreatment, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getTargetValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No treatment set.");
}

/// \brief Test cases for the TargetValidator::perform method.
///        This method performs a target validation with a treatment that specifies custom targets
///        that have the same active moiety and the sametarget type.
///        The XpertRequestResult must have an error and shouldContinueProcessing must return false.
/// \param none but uses the related fixture
TEST_F(TargetValidatorTest, SameTypeValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<Xpert::XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSameType, TestUtils::originalImatinibModelString, xpertQueryResult);

    Xpert::XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getTargetValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "Two patient's targets with the same active moiety and the same target type detected.");
}

/// \brief Test cases for the TargetValidator::perform method.
///        This method performs a target validation with a treatment that specifes a custom target
///        that has a different active moiety than the imatinib drug model.
///        The XpertRequestResult must have an error and shouldContinueProcessing must return false.
/// \param none but uses the related fixture
TEST_F(TargetValidatorTest, NotInDrugModelValidator)
{
    // Prepare the XpertRequestResult
    unique_ptr<Xpert::XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNotInDrugModel, TestUtils::originalImatinibModelString, xpertQueryResult);

    Xpert::XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getTargetValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "A target is related to an active moiety (randomactivemoiety) that does not belong to the drug model: ch.tucuxi.imatinib.gotta2012");
}



} // namespace Xpert
} // namespace Tucuxi
