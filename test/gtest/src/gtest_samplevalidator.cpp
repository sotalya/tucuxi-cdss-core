// SPDX-License-Identifier: AGPL-3.0-or-later
#include <stdexcept>

#include <gtest/gtest.h>

#include "tucucommon/datetime.h"
#include "tucucommon/unit.h"

#include "tucucore/definitions.h"
#include "tucucore/drugtreatment/sample.h"

#include "testutils.h"
#include "tucucdss-core/flow/general/samplevalidator.h" // Source file to be tested
#include "tucucdss-core/flow/general/samplevalidator.h"
#include "tucucdss-core/result/samplevalidationresult.h"
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of SampleValidator.
///        Tests for SampleValidator from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined functions for the tests          *
 ***********************************************************/

// SampleValidator::perform and SampleValidator::findGroupPositionOver99Percentiles fixture
class SampleValidatorTest : public testing::Test
{
public:
    // Core::Sample, SampleValidator,
    // friend class SampleValidator;
    // friend class Core::Sample;

    // using SampleValidator::findGroupPositionOver99Percentiles;



    /// \brief Create some percentile data and insert them into _percentileData.
    /// \param _percentileData PercentilesData object which receives the result.
    void createPercentilesData(Core::PercentilesData& _percentileData)
    {

        // Prepare the PercentiledData
        // There are going to be 99 percentiles (1-99) with 2 CycleDatas:
        // 1.1.2022 10:00:00 - 1.1.2022 11:00:00, 1.1.2022 11:00:00 - 1.1.2022 12:00:00
        Common::DateTime d1{"2022-01-01T10:00:00", TestUtils::date_format};
        Common::DateTime d2{"2022-01-01T11:00:00", TestUtils::date_format};
        Common::DateTime d3{"2022-01-01T12:00:00", TestUtils::date_format};

        vector<Common::DateTime> cyclesStarts{d1, d2};
        vector<Common::DateTime> cyclesEnds{d2, d3};

        // For the 99 percentiles (1-99), each percentile start at p * 10 (1 = 10, 99 = 990)
        // The first cycle goes from p * 10 to p * 10 + 1 and the second from p * 10 + 1 to p * 10 + 2
        // (if p = 1 -> 10 - 11 , 11 - 12)
        // There will be 3 points per hour.
        // So, for the first percentile:
        // Cycle 1 [1.1.2022 10:00:00 - 1.1.2022 11:00:00], values: 10 10.5 11
        //                                                   times:  0  0.5  1
        // Cycle 2 [1.1.2022 11:00:00 - 1.1.2022 12:00:00], values: 11 11.5 12
        //                                                   times:  0  0.5  1

        const Core::TimeOffsets times{0, 0.5, 1};

        // For each percentile
        for (size_t pi = 1; pi <= 99; ++pi) {

            // Create the cycle data
            vector<Core::CycleData> cycles;
            for (size_t cdi = 0; cdi < 2; ++cdi) {

                cycles.emplace_back(cyclesStarts[cdi], cyclesEnds[cdi], unit);

                // Prepare the concentrations
                Core::Concentrations concentrations;
                for (size_t ci = 0; ci < 3; ++ci) {
                    concentrations.emplace_back(pi * 10 + cdi + ci * 0.5);
                }

                cycles[cdi].addData(times, concentrations);
            }

            _percentileData.addPercentileData(cycles);
        }
    };

    /// \brief Unit used to create the percentile data.
    const Common::TucuUnit unit{"ug/l"};



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

    std::string queryStringNoDosage = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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

    std::string queryStringSortedNotEmpty = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                        <sample>
                                                            <sampleId>123456</sampleId>
                                                            <sampleDate>2018-07-08T07:00:00</sampleDate>
                                                            <concentrations>
                                                                <concentration>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <value>10.6</value>
                                                                    <unit>mg/l</unit>
                                                                </concentration>
                                                            </concentrations>
                                                        </sample>
                                                        <sample>
                                                            <sampleId>123456</sampleId>
                                                            <sampleDate>2018-07-06T13:00:00</sampleDate>
                                                            <concentrations>
                                                                <concentration>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <value>0.8</value>
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

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the SampleValidator::perform method.
///        Check that there is an error if the treatment of an XpertRequestResult is nullptr
///        in SampleValidator.
///        The SampleValidator must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and there should be 0 SampleValidationResult.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, NullptrTreatmentSampleValidator)
{

    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoTreatment, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No treatment set.");
    EXPECT_EQ(xpertRequestResult.getSampleValidationResults().size(), 0);
}

/// \brief Test cases for the SampleValidator::perform method.
///        Check that there is an error if the treatment of an XpertRequestResult has
///        samples but no dosages in SampleValidator.
///        The SampleValidator must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and there should be 0 SampleValidationResult.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, SamplesWithoutDosagesSampleValidator)
{

    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDosage, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "Samples found but dosage history is empty.");
    EXPECT_EQ(xpertRequestResult.getSampleValidationResults().size(), 0);
}

/// \brief Test cases for the SampleValidator::perform method.
///        The test loads an xpertRequest without assigning a drug model to it.
///        The SampleValidator must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and there should be 0 SampleValidationResult.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, NullptrDrugModelSampleValidator)
{

    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDrugModel, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No drug model set.");
    EXPECT_EQ(xpertRequestResult.getSampleValidationResults().size(), 0);
}


/// \brief Test cases for the SampleValidatorResult objects.
///        This test creates fake SampleValidationResult objects and checks
///        that the warnings are correct.
///           - If the group is 1, the message must be "99% of the population is above this measure".
///           - If the group is 10, the message must be "90% of the population is above this measure".
///           - If the group is 11, the message must be "".
///           - If the group is 90, the message must be "".
///           - If the group is 91, the message must be "90% of the population is below this measure".
///           - If the group is 100, the message must be "99% of the population is below this measure".
///
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, CorrectWarningComputerForSampleValidationResult)
{

    // Load the translations file for the warning message computation
    TestUtils::loadTranslationsFile(TestUtils::englishTranslationFile);

    // Creating SampleValidationResult objects that are located in a different group. There are
    // 100 groups that are implicitly formed by the 99 percentiles.
    SampleValidationResult sr1 = SampleValidationResult(nullptr, 1, WarningLevel::NORMAL, true);
    SampleValidationResult sr10 = SampleValidationResult(nullptr, 10, WarningLevel::NORMAL, true);
    SampleValidationResult sr11 = SampleValidationResult(nullptr, 11, WarningLevel::NORMAL, true);
    SampleValidationResult sr90 = SampleValidationResult(nullptr, 90, WarningLevel::NORMAL, true);
    SampleValidationResult sr91 = SampleValidationResult(nullptr, 91, WarningLevel::NORMAL, true);
    SampleValidationResult sr100 = SampleValidationResult(nullptr, 100, WarningLevel::NORMAL, true);

    // Compare
    EXPECT_EQ(sr1.getWarning(), "99% of the population is above this measure");
    EXPECT_EQ(sr10.getWarning(), "90% of the population is above this measure");
    EXPECT_EQ(sr11.getWarning(), "");
    EXPECT_EQ(sr90.getWarning(), "");
    EXPECT_EQ(sr91.getWarning(), "90% of the population is below this measure");
    EXPECT_EQ(sr100.getWarning(), "99% of the population is below this measure");
}

/// \brief Test cases for the SampleValidator::findGroupPositionOver99Percentiles method.
///        This method checks that the SampleValidator::findPosOver99Percentile method returns
///        the correct positions for a given set of samples and a given percentileData.
///
///        We test the SampleValidator class with SampleValidator::findPosOver100Percentile and not
///        with SampleValidator::perform because this last method performs a request
///        on the core that returns "unpredictable" percentiles. So, basically 90% of SampleValidator's
///        behavior can be verified with SampleValidator::findPosOver100Percentile.
///
///        Thus, this struct is friend with SampleValidator to acces this protected method.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, GivenPercentilesDataAndSamplesGroupPositionOver99PercentilesFinder)
{
    Core::PercentilesData percentilesData{""};
    createPercentilesData(percentilesData);

    // We perform 9 series of tests.
    // The times tested are from 1.1.2022 10:00:00 to 1.1.2022 12:00:00 every 15 minutes.
    // 18 concentrations are tested, 2 at each time: one slightly below the percentile
    //                                               one slightly above the percentile
    vector<Common::DateTime> testDates{
            Common::DateTime{"2022-01-01T10:00:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T10:15:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T10:30:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T10:45:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T11:00:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T11:15:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T11:30:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T11:45:00", TestUtils::date_format},
            Common::DateTime{"2022-01-01T12:00:00", TestUtils::date_format},
    };

    SampleValidator sampleValidator;
    // For each percentile
    for (size_t pi = 1; pi <= 99; ++pi) {
        for (size_t ti = 0; ti < 9; ++ti) {
            double percentileValue = pi * 10 + ti * 0.25;

            // Create a sample that is slightly above and a sample that is slightly below the percentile
            Core::Sample slightlyAbove(testDates[ti], Core::AnalyteId{""}, percentileValue + 0.01, unit);
            Core::Sample slightlyBelow(testDates[ti], Core::AnalyteId{""}, percentileValue - 0.01, unit);

            bool isAscending = true;

            unsigned groupSlightlyAbove =
                    sampleValidator.findGroupPositionOver99Percentiles(&percentilesData, slightlyAbove, isAscending);
            unsigned groupSlightlyBelow =
                    sampleValidator.findGroupPositionOver99Percentiles(&percentilesData, slightlyBelow, isAscending);

            // Compare
            EXPECT_EQ(groupSlightlyAbove, pi + 1);
            EXPECT_EQ(groupSlightlyBelow, pi);
        }
    }
}


/// \brief Test cases for the SampleValidator::findGroupPositionOver99Percentiles method.
///        This method checks that the method SampleValidator::findPosOver99Percentile throws
///        an invalid_argument exception when a sample unit cannot be converted into the unit
///        of the cycleData of the percentilesData object.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, SampleUnitConversionFailureGroupPositionOver99PercentilesFinder)
{
    // Prepare percentilesData and SampleValidator
    Core::PercentilesData percentilesData{""};
    createPercentilesData(percentilesData);

    // Prepare percentilesData and SampleValidator
    SampleValidator sampleValidator;

    // Prepare a sample with "kg" as unit.
    // "kg" cannot be converted in "ug/l".
    Core::Sample sample(
            Common::DateTime{"2022-01-01T11:30:00", TestUtils::date_format},
            Core::AnalyteId{""},
            1,
            Common::TucuUnit{"kg"});

    bool isAscending = true;
    // Compare
    EXPECT_THROW(
            sampleValidator.findGroupPositionOver99Percentiles(&percentilesData, sample, isAscending),
            std::invalid_argument);
}


/// \brief Test cases for the SampleValidator::findGroupPositionOver99Percentiles method.
///        This method checks that the method SampleValidator::findPosOver100Percentile throws
///        an invalid_argument exception when a sample date cannot be found in the cycleData objects.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, SampleDateNotFoundInCycleDataGroupPositionOver99PercentilesFinder)
{

    Core::PercentilesData percentilesData{""};
    createPercentilesData(percentilesData);

    // Prepare percentilesData and SampleValidator
    SampleValidator sampleValidator;

    // Prepare a sample with a date in 2023.
    // 2023 is not between  "2022-01-01T10:00:00" and ""2022-01-01T12:00:00"".
    Core::Sample sample(Common::DateTime{"2023-01-01T11:30:00", TestUtils::date_format}, Core::AnalyteId{""}, 1, unit);
    bool isAscending = true;
    // Compare
    EXPECT_THROW(
            sampleValidator.findGroupPositionOver99Percentiles(&percentilesData, sample, isAscending),
            std::invalid_argument);
}

/// \brief Test cases for the SampleValidator::perform method.
///        This method performs a real sample validation with 3 samples. The vector retrived by
///        XpertRequestResult::getSampleValidationResults must be sorted by sample date.
///        We only check the sample date. The other interesting values, such as:
///        percentile, warning type, warning message are already tested by more deterministic tests.
///
///        The smallest date comes first.
/// \param none but uses the related fixture
TEST_F(SampleValidatorTest, NotEmptySampleValidationResultGetter)
{

    // Prepare the XpertRequestResult
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSortedNotEmpty, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    const vector<SampleValidationResult>& sampleValidationResults = xpertRequestResult.getSampleValidationResults();

    // Compare
    EXPECT_EQ(sampleValidationResults.size(), xpertRequestResult.getTreatment()->getSamples().size());
    EXPECT_EQ(
            sampleValidationResults[0].getSource()->getDate(),
            Common::DateTime("2018-07-06T13:00:00", TestUtils::date_format));
    EXPECT_EQ(
            sampleValidationResults[1].getSource()->getDate(),
            Common::DateTime("2018-07-07T06:00:30", TestUtils::date_format));
    EXPECT_EQ(
            sampleValidationResults[2].getSource()->getDate(),
            Common::DateTime("2018-07-08T07:00:00", TestUtils::date_format));
}


/// \brief Covers SampleValidator::perform() and
///        SampleValidator::groupSamplesByTime():
///        no samples leads to an empty group, the loop continues, and
///        results are empty.
TEST_F(SampleValidatorTest, NoSamplesPerform)
{
    // Start from the sorted query and remove all sample content
    std::string queryStr = queryStringSortedNotEmpty;
    size_t samplesStart = queryStr.find("<samples>");
    ASSERT_NE(samplesStart, std::string::npos);
    size_t samplesEnd = queryStr.find("</samples>", samplesStart);
    ASSERT_NE(samplesEnd, std::string::npos);
    samplesEnd += std::string("</samples>").size();
    queryStr.replace(samplesStart, samplesEnd - samplesStart, "<samples></samples>");

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.shouldContinueProcessing());
    EXPECT_EQ(xpertRequestResult.getSampleValidationResults().size(), 0);
}

/// \brief Covers SampleValidator::groupSamplesByTime():
///        a single sample triggers the early return before the end of the function.
TEST_F(SampleValidatorTest, SingleSamplePerform)
{
    // Start from the sorted query and keep only the first sample
    std::string queryStr = queryStringSortedNotEmpty;
    size_t firstSampleEnd = queryStr.find("</sample>");
    ASSERT_NE(firstSampleEnd, std::string::npos);
    firstSampleEnd += std::string("</sample>").size();
    size_t samplesCloseTag = queryStr.find("</samples>");
    ASSERT_NE(samplesCloseTag, std::string::npos);
    queryStr.erase(firstSampleEnd, samplesCloseTag - firstSampleEnd);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.shouldContinueProcessing());
    EXPECT_EQ(xpertRequestResult.getSampleValidationResults().size(), 1);
}

/// \brief Covers SampleValidator::perform():
///        a sample with an incompatible unit ("kg") causes
///        findGroupPositionOver99Percentiles to throw invalid_argument,
///        which is caught and sets an error message.
TEST_F(SampleValidatorTest, SampleWithIncompatibleUnitPerform)
{
    // Start from the sorted query, keep 1 sample, change its unit to "kg"
    std::string queryStr = queryStringSortedNotEmpty;
    size_t firstSampleEnd = queryStr.find("</sample>");
    ASSERT_NE(firstSampleEnd, std::string::npos);
    firstSampleEnd += std::string("</sample>").size();
    size_t samplesCloseTag = queryStr.find("</samples>");
    ASSERT_NE(samplesCloseTag, std::string::npos);
    queryStr.erase(firstSampleEnd, samplesCloseTag - firstSampleEnd);

    // Change concentration unit from mg/l to kg (incompatible with the drug model unit)
    size_t unitPos = queryStr.find("<unit>mg/l</unit>");
    ASSERT_NE(unitPos, std::string::npos);
    queryStr.replace(unitPos, std::string("<unit>mg/l</unit>").size(), "<unit>kg</unit>");

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    EXPECT_FALSE(xpertRequestResult.shouldContinueProcessing());
    EXPECT_TRUE(xpertRequestResult.getErrorMessage().find("Error handling sample") != std::string::npos);
}

/// \brief Covers SampleValidator::perform():
///        using a formulation/route not supported by the imatinib oral model
///        causes the percentile computation to fail, returning nullptr.
TEST_F(SampleValidatorTest, PercentilesComputationFailed)
{
    // Start from the sorted query, keep 1 sample, change route to IV bolus
    std::string queryStr = queryStringSortedNotEmpty;
    size_t firstSampleEnd = queryStr.find("</sample>");
    ASSERT_NE(firstSampleEnd, std::string::npos);
    firstSampleEnd += std::string("</sample>").size();
    size_t samplesCloseTag = queryStr.find("</samples>");
    ASSERT_NE(samplesCloseTag, std::string::npos);
    queryStr.erase(firstSampleEnd, samplesCloseTag - firstSampleEnd);

    // Change route and absorption model to IV bolus (not supported by imatinib oral model)
    size_t routePos = queryStr.find("<administrationRoute>oral</administrationRoute>");
    ASSERT_NE(routePos, std::string::npos);
    queryStr.replace(
            routePos,
            std::string("<administrationRoute>oral</administrationRoute>").size(),
            "<administrationRoute>intravenousBolus</administrationRoute>");
    size_t absPos = queryStr.find("<absorptionModel>extravascular</absorptionModel>");
    ASSERT_NE(absPos, std::string::npos);
    queryStr.replace(
            absPos,
            std::string("<absorptionModel>extravascular</absorptionModel>").size(),
            "<absorptionModel>intravascular</absorptionModel>");

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    TestUtils::flowStepProvider.getSampleValidator()->perform(xpertRequestResult);

    EXPECT_FALSE(xpertRequestResult.shouldContinueProcessing());
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "Percentiles computation failed.");
}

} // namespace Xpert
} // namespace Tucuxi
