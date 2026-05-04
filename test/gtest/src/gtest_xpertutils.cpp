// SPDX-License-Identifier: AGPL-3.0-or-later
#include <stdexcept>

#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/language/languageexception.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h" // Source file to be tested

/// \brief Gtest of xpertutils.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

// getOldestDosageTimeRangeStart fixture
class OldestDosageTimeRangeStartFinderTest : public testing::Test
{
public:
    Core::DosageHistory CreateDosageHistoryForOldestTest()
    {
        // Making dosage history
        Core::DosageHistory dosageHistory;

        // Common elements
        Core::Unit unit{"mg"};
        Core::FormulationAndRoute formulationAndRoute; //{Core::AbsorptionModel::Extravascular};
        Common::Duration duration, interval{chrono::hours(1)};

        // Making first time range "2022-01-01 10h00 - 2022-01-02 13h00"
        Core::LastingDose lastingDose1{1, unit, formulationAndRoute, duration, interval};
        Core::DosageTimeRange timeRange1{
                Common::DateTime("2022-01-01T10:00:00", "%Y-%m-%dT%H:%M:%S"),
                Common::DateTime("2022-01-02T13:00:00", "%Y-%m-%dT%H:%M:%S"),
                lastingDose1};

        // Making second time range "2022-01-03 14h00 - 2022-01-04 16h00"
        Core::LastingDose lastingDose2{1, unit, formulationAndRoute, duration, interval};
        Core::DosageTimeRange timeRange2{
                Common::DateTime("2022-01-03T14:00:00", "%Y-%m-%dT%H:%M:%S"),
                Common::DateTime("2022-01-04T16:00:00", "%Y-%m-%dT%H:%M:%S"),
                lastingDose1};

        // Filling dosage history
        dosageHistory.addTimeRange(timeRange1);
        dosageHistory.addTimeRange(timeRange2);

        return dosageHistory;
    }

private:
};

// getLatestDosageTimeRangeStart fixture
class LatestDosageTimeRangeStartFinderTest : public testing::Test
{
public:
    Core::DosageHistory CreateDosageHistoryForLatestTest()
    {
        // Making dosage history
        Core::DosageHistory dosageHistory;

        // Common elements
        Core::Unit unit{"mg"};
        Core::FormulationAndRoute formulationAndRoute; //{Core::AbsorptionModel::Extravascular};
        Common::Duration duration, interval{chrono::hours(1)};

        // Making first time range "2022-01-01 10h00 - 2022-01-02 13h00"
        Core::LastingDose lastingDose1{1, unit, formulationAndRoute, duration, interval};
        Core::DosageTimeRange timeRange1{
                Common::DateTime("2022-01-01T10:00:00", "%Y-%m-%dT%H:%M:%S"),
                Common::DateTime("2022-01-02T13:00:00", "%Y-%m-%dT%H:%M:%S"),
                lastingDose1};

        // Making second time range "2022-01-03 14h00 - 2022-01-04 16h00"
        Core::LastingDose lastingDose2{1, unit, formulationAndRoute, duration, interval};
        Core::DosageTimeRange timeRange2{
                Common::DateTime("2022-01-03T14:00:00", "%Y-%m-%dT%H:%M:%S"),
                Common::DateTime("2022-01-04T16:00:00", "%Y-%m-%dT%H:%M:%S"),
                lastingDose1};

        // Making dosage history
        dosageHistory.addTimeRange(timeRange1);
        dosageHistory.addTimeRange(timeRange2);

        return dosageHistory;
    }

    Core::DosageHistory dosageHistory = CreateDosageHistoryForLatestTest();

private:
};

// computeFileName fixture
class FileNameComputerTest : public testing::Test
{
public:
    std::string queryString =
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
                                                            <sampleDate>2018-07-07T07:00:00</sampleDate>
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
                                                            <sampleDate>2018-07-07T13:00:00</sampleDate>
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

    // Preparation - importation
    unique_ptr<XpertQueryResult> _xpertQueryResult = nullptr;


private:
};

// executeRequestAndGetResult fixture
class RequestExecuterTest : public testing::Test
{
public:
    std::string queryStringRequest = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2018-07-11T13:45:30</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                    <covariate>
                                                        <covariateId>birthdate</covariateId>
                                                        <date>2018-07-11T10:45:30</date>
                                                        <value>1990-01-01T00:00:00</value>
                                                        <unit></unit>
                                                          <dataType>date</dataType>
                                                        <nature>discrete</nature>
                                                    </covariate>
                                                    <covariate>
                                                        <covariateId>bodyweight</covariateId>
                                                        <date>2017-07-06T08:00:00</date>
                                                        <value>70</value>
                                                        <unit>kg</unit>
                                                          <dataType>double</dataType>
                                                        <nature>discrete</nature>
                                                    </covariate>
                                                    <covariate>
                                                        <covariateId>bodyweight</covariateId>
                                                        <date>2018-07-06T08:00:00</date>
                                                        <value>150000</value>
                                                        <unit>g</unit>
                                                          <dataType>double</dataType>
                                                        <nature>discrete</nature>
                                                    </covariate>
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
                                                            <sampleDate>2018-07-07T13:00:00</sampleDate>
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

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the dataTypeToString method.
///        Convert all the DataType to string. Check that the string contains the correct value.
///        Check that the value are expected.
///        - Bool must be "bool"
///        - Int must be "int"
///        - Double must be "doule"
///        - Date must be "date".
/// \param none
TEST(DataTypeConverterTest, IntConversion)
{
    Core::DataType dataType = Core::DataType::Int;
    EXPECT_EQ("int", dataTypeToString(dataType));
}

TEST(DataTypeConverterTest, DoubleConversion)
{
    Core::DataType dataType = Core::DataType::Double;
    EXPECT_EQ("double", dataTypeToString(dataType));
}

TEST(DataTypeConverterTest, BoolConversion)
{
    Core::DataType dataType = Core::DataType::Bool;
    EXPECT_EQ("bool", dataTypeToString(dataType));
}

TEST(DataTypeConverterTest, DateConversion)
{
    Core::DataType dataType = Core::DataType::Date;
    EXPECT_EQ("date", dataTypeToString(dataType));
}

/// \brief Test cases for the doubleToString method.
///        Convert some doubles to string. Check that the strings contain two decimals.
///        Check that the value are expected.
///        - 123.456 must 123.46
///        - 5 must be 5.00
///        - -0.987 must be -0.99
/// \param none
TEST(DoubleConverterTest, PositiveValue)
{
    double doubleInput = 123.456;
    std::string expected = "123.46";
    ASSERT_EQ(expected, doubleToString(doubleInput));
}

TEST(DoubleConverterTest, ExactValue)
{
    double doubleInput = 5;
    std::string expected = "5.00";
    ASSERT_EQ(expected, doubleToString(doubleInput));
}

TEST(DoubleConverterTest, NegativeValue)
{
    double doubleInput = -0.987;
    std::string expected = "-0.99";
    ASSERT_EQ(expected, doubleToString(doubleInput));
}

/// \brief Test cases for the covariateTypeToString method.
///        Convert all the CovariateType to string. Check that the values are expected.
///        - Model must be "default"
///        - Patient must be "patient"
///        - -1 -> invalid_argument
/// \param none
TEST(CovariateTypeConverterTest, ModelConversion)
{
    CovariateType covariateType = CovariateType::MODEL;
    EXPECT_EQ("default", covariateTypeToString(covariateType));
}

TEST(CovariateTypeConverterTest, PatientConversion)
{
    CovariateType covariateType = CovariateType::PATIENT;
    EXPECT_EQ("patient", covariateTypeToString(covariateType));
}

TEST(CovariateTypeConverterTest, InvalidConversion)
{
    /// "invalid_argument"
    EXPECT_THROW(covariateTypeToString(CovariateType(-1)), std::invalid_argument);
}

/// \brief Test cases for the outputLangToString method.
///        Convert the OutputLang to string. Check that the values are expected or a LanguageException.
///        At the moment the only supported languages are: English and French.
///        - ENGNLISH must be "en"
///        - FRENCH must be "fr"
///        - -1 -> LanguageException
/// \param none
TEST(OutputLangConverterTest, EnConversion)
{
    OutputLang lang = OutputLang::ENGLISH;
    EXPECT_EQ("en", outputLangToString(lang));
}

TEST(OutputLangConverterTest, FrConversion)
{
    OutputLang lang = OutputLang::FRENCH;
    EXPECT_EQ("fr", outputLangToString(lang));
}

TEST(OutputLangConverterTest, ExceptionConversion)
{
    EXPECT_THROW(outputLangToString(OutputLang(-1)), LanguageException);
}

/// \brief Test cases for the warningLevelToString method.
///        Convert the WarningLevel to string. Check that the values are expected or an invalid_argument.
///        - CRITICAL must be "critical"
///        - NORMAL must be "normal"
/// \param none
TEST(WarningLevelConverterTest, NormalConversion)
{
    WarningLevel level = WarningLevel::NORMAL;
    EXPECT_EQ("normal", warningLevelToString(level));
}

TEST(WarningLevelConverterTest, CriticalConversion)
{
    WarningLevel level = WarningLevel::CRITICAL;
    EXPECT_EQ("critical", warningLevelToString(level));
}

/// \brief Test cases for the dateTimeToXmlString method.
///        Convert a DateTime to a formatted date time string for xml. Check that the string
///        contains the correct value.
///        The 1.1.2022 at 10h must be "2022-01-01T10:00:00"
/// \param none
TEST(DateTimeXmlConverterTest, ValidDateTimeXmlConversion)
{
    Common::DateTime validDateTime = Common::DateTime("1994.01.18 11:42:35", "%Y.%m.%d %H:%M:%S");
    std::string expected = "1994-01-18T11:42:35";
    ASSERT_EQ(expected, dateTimeToXmlString(validDateTime));
}

TEST(DateTimeXmlConverterTest, UndefinedDateTimeXmlConversion)
{
    Common::DateTime undefinedDateTime = DateTime(); // Assuming this is an undefined date and time
    std::string expected = "";
    ASSERT_EQ(expected, dateTimeToXmlString(undefinedDateTime));
}

/// \brief Test cases for the timeOfDayToString method.
///        Convert some TimeOfDay to string. Checks that the values are expected.
///        - 12h 0m 0s must be "12h"
///        - 12h 46m 58s must be "12h46"
/// \param none
TEST(TimeOfDayConverterTest, PlainHoursTimeConversion)
{
    LanguageManager& languageManager = LanguageManager::getInstance();
    languageManager.loadTranslations(TestUtils::englishTranslationFile);

    Common::TimeOfDay validTimeOfDay = Common::TimeOfDay(Duration(chrono::hours(12)));
    std::string expected = "12h";
    ASSERT_EQ(expected, timeOfDayToString(validTimeOfDay));
}

TEST(TimeOfDayConverterTest, WithMinutesTimeConversion)
{
    Common::TimeOfDay validTimeOfDay =
            Common::TimeOfDay(Duration(chrono::hours(12), chrono::minutes(46), chrono::seconds(58)));
    std::string expected = "12h46";
    ASSERT_EQ(expected, timeOfDayToString(validTimeOfDay));
}

/// \brief Test cases for the dateTimeToString method.
///        Convert some dateTime to a date time strings for html/pdf. Check that the strings
///        contain the correct value.
///        - The 1.1.2022 at 10h (asking for the time indication) must be "1.1.2022 10h"
///        - The 1.1.2022 at 10h (not asking for the time indication) must be "1.1.2022"
/// \param none
TEST(DateTimeConverterTest, WithTimeDateTimeConversion)
{
    Common::DateTime validDateTime = Common::DateTime("1994.01.18T11:42:35", "%Y.%m.%dT%H:%M:%S");
    bool withTime = 1;

    std::string expected = "18.1.1994 11h42";
    ASSERT_EQ(expected, dateTimeToString(validDateTime, withTime));
}

TEST(DateTimeConverterTest, WithoutTimeDateTimeConversion)
{
    Common::DateTime validDateTime = Common::DateTime("1994.01.18T11:42:35", "%Y.%m.%dT%H:%M:%S");
    bool withTime = 0;

    std::string expected = "18.1.1994";
    ASSERT_EQ(expected, dateTimeToString(validDateTime, withTime));
}

TEST(DateTimeConverterTest, UndefinedDateTimeConversion)
{
    Common::DateTime undefinedDateTime = Common::DateTime();
    bool withTime = 1;

    std::string expected = "";
    ASSERT_EQ(expected, dateTimeToString(undefinedDateTime, withTime));
}

/// \brief Test cases for the beautifyString method.
///        Convert some covariate string values. Check that
///        resulting strings are the expected values.
///        - "1.0" Bool (no id) must be "Yes"
///        - "0.0" Bool (no id) must be "No"
///        - "1.0" Sex (no id) must be "Man"
///        - "0.0" Sex (no id) must be "Woman"
///        - "0.5" Sex (no id) must be "Undefined"
///        - "42.00" Int age must be "42"
///        - "72.652222" Double bodyweight must be "72.65"
/// \param none
TEST(BeautifyConverterTest, BoolConversion)
{
    std::string value = "1";
    Core::DataType dataType = Core::DataType::Bool;
    std::string id = "";

    std::string expected = "Yes";
    ASSERT_EQ(expected, beautifyString(value, dataType, id));
}

TEST(BeautifyConverterTest, DoubleConversion)
{
    std::string value = "12.345";
    Core::DataType dataType = Core::DataType::Double;
    std::string id = "987";

    std::string expected = "12.35";
    ASSERT_EQ(expected, beautifyString(value, dataType, id));
}

TEST(BeautifyConverterTest, IntConversion)
{
    std::string value = "12";
    Core::DataType dataType = Core::DataType::Int;
    std::string id = "987";

    std::string expected = "12";
    ASSERT_EQ(expected, beautifyString(value, dataType, id));
}

TEST(BeautifyConverterTest, WomanSexConversion)
{
    std::string value = "0";
    Core::DataType dataType = Core::DataType::Double;
    std::string id = "sex";

    std::string expected = "Woman";
    ASSERT_EQ(expected, beautifyString(value, dataType, id));
}

TEST(BeautifyConverterTest, UndefinedSexConversion)
{
    std::string value = "0.5";
    Core::DataType dataType = Core::DataType::Double;
    std::string id = "sex";

    std::string expected = "Undefined";
    ASSERT_EQ(expected, beautifyString(value, dataType, id));
}

/// \brief Test cases for the getStringWithEnglishFallback method.
///        Check that the method getStringWithEnglishFallback returns the correct value.
///        - A translatableString with the required language must returns the targeted translation.
///        - A translatableString without the required language but English returns the English translation.
///        - A translatableString without the required language and without English returns an empty string.
/// \param none
TEST(EnglishStringTranslaterTest, En2EnTranslation)
{
    Common::TranslatableString englishString = Common::TranslatableString("Hello! I am Xpert.");
    OutputLang lang = OutputLang::ENGLISH;

    std::string expected = "Hello! I am Xpert.";
    ASSERT_EQ(expected, getStringWithEnglishFallback(englishString, lang));
}

TEST(EnglishStringTranslaterTest, Fr2EnTranslation)
{
    Common::TranslatableString frenchString = Common::TranslatableString("translatable std::string in en");
    Common::TranslatableString frenchStringWithRequiredLang("translatable std::string in en");
    frenchStringWithRequiredLang.setString("chaine de caracteres traductible en fr", "fr");
    Common::TranslatableString stringWithoutRequiredLangAndWithoutEnglish;
    OutputLang outputLang = OutputLang::FRENCH;

    std::string expectedWithRequiredLang = "chaine de caracteres traductible en fr";
    std::string expected = "translatable std::string in en";
    std::string expectedWithoutRequiredLangAndWithoutEnglish = "";
    ASSERT_EQ(expected, getStringWithEnglishFallback(frenchString, outputLang));
    ASSERT_EQ(expectedWithRequiredLang, getStringWithEnglishFallback(frenchStringWithRequiredLang, outputLang));
    ASSERT_EQ(
            expectedWithoutRequiredLangAndWithoutEnglish,
            getStringWithEnglishFallback(stringWithoutRequiredLangAndWithoutEnglish, outputLang));
}

TEST(EnglishStringTranslaterTest, EmptyTranslation)
{
    Common::TranslatableString emptyString = Common::TranslatableString("");
    OutputLang outputLang = OutputLang();

    std::string expected = "";
    ASSERT_EQ(expected, getStringWithEnglishFallback(emptyString, outputLang));
}

/// \brief Test cases for the getOldestDosageTimeRangeStart method.
///        Test that the getOldestDosageTimeRangeStart works as expected.
///        This test forms a dosage history with two time ranges:
///         1) 2022-01-01 10h00 - 2022-01-02 13h00
///         2) 2022-01-03 14h00 - 2022-01-04 16h00
///
///        Once it requires the oldest start time with a computation time of 2022-01-01 14h00
///        (just after the first time range) which should return 2022-01-01 10h00
///
///        Once it requires the oldest start time with a computation time of 2022-01-01 09h00
///        (before both time ranges) which should return the reference time itself.
/// \param none but uses the related fixture
TEST_F(OldestDosageTimeRangeStartFinderTest, OldestTimeRangeStart10Finder)
{
    Core::DosageHistory dosageHistory = CreateDosageHistoryForOldestTest();

    Common::DateTime referenceTime = Common::DateTime("2022-01-01T14:00:00", "%Y-%m-%dT%H:%M:%S");

    Common::DateTime expected = Common::DateTime("2022-01-01T10:00:00", "%Y-%m-%dT%H:%M:%S");

    ASSERT_EQ(expected, getOldestDosageTimeRangeStart(dosageHistory, referenceTime));
}

TEST_F(OldestDosageTimeRangeStartFinderTest, OldestTimeRangeStart09Finder)
{
    Core::DosageHistory dosageHistory = CreateDosageHistoryForOldestTest();

    Common::DateTime referenceTime = Common::DateTime("2022-01-01T09:00:00", "%Y-%m-%dT%H:%M:%S");

    Common::DateTime expected = Common::DateTime("2022-01-01T09:00:00", "%Y-%m-%dT%H:%M:%S");

    ASSERT_EQ(expected, getOldestDosageTimeRangeStart(dosageHistory, referenceTime));
}

/// \brief Test cases for the getLatestDosageTimeRangeStart method.
///        This test forms a dosage history with two time ranges:
///         1) 2022-01-01 10h00 - 2022-01-02 13h00
///         2) 2022-01-03 14h00 - 2022-01-04 16h00
///
///        Once it requires the latest start time with a computation time of 2022-01-07 15h00
///        (after both time ranges) which should return 2022-01-03 14h00
///
///        Once it requires the oldest start time with a computation time of 2022-01-01 09h00
///        (before both time ranges) which should return an undefined date time.
/// \param none but uses the related fixture
TEST_F(LatestDosageTimeRangeStartFinderTest, ValidLatestTimeRangeStartFinder)
{
    Core::DosageHistory dosageHistory = CreateDosageHistoryForLatestTest();

    Common::DateTime referenceTime = Common::DateTime("2022-01-07T15:00:00", "%Y-%m-%dT%H:%M:%S");

    Common::DateTime expected = Common::DateTime("2022-01-03T14:00:00", "%Y-%m-%dT%H:%M:%S");

    ASSERT_EQ(expected, getLatestDosageTimeRangeStart(dosageHistory, referenceTime));
}

TEST_F(LatestDosageTimeRangeStartFinderTest, UndefinedLatestTimeRangeStartFinder)
{
    Core::DosageHistory dosageHistory = CreateDosageHistoryForLatestTest();

    Common::DateTime referenceTime = Common::DateTime("2022-01-01T09:00:00", "%Y-%m-%dT%H:%M:%S");

    bool expected = true;

    ASSERT_EQ(expected, getLatestDosageTimeRangeStart(dosageHistory, referenceTime).isUndefined());
}

/// \brief Test cases for the computeFileName method.
///        It computes a XpertRequestResult based on classic imatinib request.
///        The names returned should be:
///        - "random/path/imatinib_1_11-7-2018_13h45m30s.xml" with the path and the extension.
///        - "imatinib_1_11-7-2018_13h45m30s.xml" without the path.
///        - "imatinib_1_11-7-2018_13h45m30s" without the path and without the exension.
/// \param none but uses the related fixture
TEST_F(FileNameComputerTest, FullFileNameComputer)
{
    TestUtils::setupEnv(queryString, TestUtils::originalImatinibModelString, _xpertQueryResult);
    XpertRequestResult& xpertRequestResult = _xpertQueryResult->getXpertRequestResults()[0];

    bool addOutputPath = true;
    bool addExtension = true;
    std::string expected = "random/path/imatinib_1_11-7-2018_13h45m30s.xml";

    EXPECT_EQ(expected, computeFileName(xpertRequestResult, addOutputPath, addExtension));
}

TEST_F(FileNameComputerTest, WithExtensionFileNameComputer)
{
    TestUtils::setupEnv(queryString, TestUtils::originalImatinibModelString, _xpertQueryResult);
    XpertRequestResult& xpertRequestResult = _xpertQueryResult->getXpertRequestResults()[0];

    bool addOutputPath = false;
    bool addExtension = true;
    std::string expected = "imatinib_1_11-7-2018_13h45m30s.xml";

    EXPECT_EQ(expected, computeFileName(xpertRequestResult, addOutputPath, addExtension));
}

TEST_F(FileNameComputerTest, SimpleFileNameComputer)
{
    TestUtils::setupEnv(queryString, TestUtils::originalImatinibModelString, _xpertQueryResult);
    XpertRequestResult& xpertRequestResult = _xpertQueryResult->getXpertRequestResults()[0];

    bool addOutputPath = false;
    bool addExtension = false;
    std::string expected = "imatinib_1_11-7-2018_13h45m30s";

    EXPECT_EQ(expected, computeFileName(xpertRequestResult, addOutputPath, addExtension));
}

/// \brief Test cases for the executeRequestAndGetResult method.
///        Check that the method executeRequestAndGetResult sets the result to nullptr when
///        the execution fails or that the resulting pointer is correctly retrieved. The test
///        prepares two percentile traits, one that must fail and one that must succeed.
///        One trait is prepared with a too large period which will fail.
///        The other is a valid trait like the ones used in samplevalidator.cpp.
///        - The first response must be nullptr.
///        - The second response must not be nullptr and must contain 3 percentiles.
/// \param none
TEST_F(RequestExecuterTest, ResultGetter)
{
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;

    TestUtils::setupEnv(queryStringRequest, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Preparing common values for the traits
    std::string responseId = "";
    Core::PercentileRanks ranks = {1.0, 2.0, 3.0};
    double nbPointsPerHour = 1;
    Core::ComputingOption computingOption{
            Core::PredictionParameterType::Apriori, Core::CompartmentsOption::AllActiveMoieties};
    Common::DateTime start = Common::DateTime("2018-07-07T13:00:00", TestUtils::date_format);

    // Normal end
    Common::DateTime normalEnd = start + chrono::hours(1);

    // Too long start/end (+ 1'000'000 hours)
    Common::DateTime tooFarEnd = start + chrono::hours(1000000);

    // Making traits and response pointers
    unique_ptr<Core::ComputingTraitPercentiles> goodTrait = make_unique<Core::ComputingTraitPercentiles>(
            responseId, start, normalEnd, ranks, nbPointsPerHour, computingOption);

    unique_ptr<Core::PercentilesData> goodResult = nullptr;

    unique_ptr<Core::ComputingTraitPercentiles> failTrait = make_unique<Core::ComputingTraitPercentiles>(
            responseId, start, tooFarEnd, ranks, nbPointsPerHour, computingOption);

    unique_ptr<Core::PercentilesData> failResult = nullptr;

    // Execution
    executeRequestAndGetResult(std::move(goodTrait), xpertRequestResult, goodResult);
    executeRequestAndGetResult(std::move(failTrait), xpertRequestResult, failResult);

    EXPECT_EQ(failResult.get(), nullptr);
    EXPECT_NE(goodResult.get(), nullptr);
    EXPECT_EQ(goodResult->getNbRanks(), 3);
}

/// \brief Test cases for the keyToPhrase method.
///        Test that a camel case key is correctly converted into a phrase.
///        - "" must be ""
///        - "ab c" must be "Ab c"
///        - "camelCase" must be "Camelcase"
/// \param none
TEST(KeyConverterTest, PhraseConversion)
{
    std::string key = "i aM tesTing the METHod.";
    std::string expected = "I am testing the method.";
    EXPECT_EQ(expected, keyToPhrase(key));
}

TEST(KeyConverterTest, WordConversion)
{
    std::string key = "camelCase";
    std::string expected = "Camelcase";
    EXPECT_EQ(expected, keyToPhrase(key));
}

TEST(KeyConverterTest, EmptyConversion)
{
    std::string key = "";
    std::string expected = "";
    EXPECT_EQ(expected, keyToPhrase(key));
}

/// \brief Test cases for the getAgeIn method.
///        The birthday is set "2000-01-01T12:00:00" and the computation time to "2010-01-01T12:00:00"
///        - The age in days must be 3653
///        - The age in weeks must be 521
///        - The age in months must be 119
///        - The age in years must be 10
///        - The age an invalid CovariateType throws an invalid_argument
/// \param none
TEST(AgeInCalculatorTest, DaysConversion)
{
    Core::CovariateType ageType = Core::CovariateType::AgeInDays;
    Common::DateTime birthDate(Common::DateTime("1994.01.18 11:42:35", "%Y.%m.%d %H:%M:%S"));
    Common::DateTime computationTime(Common::DateTime("1994.01.22 23:42:35", "%Y.%m.%d %H:%M:%S"));

    double expected = 4.0;

    EXPECT_DOUBLE_EQ(expected, getAgeIn(ageType, birthDate, computationTime));
}

TEST(AgeInCalculatorTest, WeeksConversion)
{
    Core::CovariateType ageType = Core::CovariateType::AgeInWeeks;
    Common::DateTime birthDate(Common::DateTime("1994.01.18 11:42:35", "%Y.%m.%d %H:%M:%S"));
    Common::DateTime computationTime(Common::DateTime("1994.02.01 23:42:35", "%Y.%m.%d %H:%M:%S"));

    double expected = 2.0;

    EXPECT_DOUBLE_EQ(expected, getAgeIn(ageType, birthDate, computationTime));
}

TEST(AgeInCalculatorTest, MonthsConversion)
{
    Core::CovariateType ageType = Core::CovariateType::AgeInMonths;
    Common::DateTime birthDate(Common::DateTime("1994.01.18 11:42:35", "%Y.%m.%d %H:%M:%S"));
    Common::DateTime computationTime(Common::DateTime("1994.03.01 23:42:35", "%Y.%m.%d %H:%M:%S"));

    double expected = 1.0;

    EXPECT_DOUBLE_EQ(expected, getAgeIn(ageType, birthDate, computationTime));
}

TEST(AgeInCalculatorTest, YearsConversion)
{
    Core::CovariateType ageType = Core::CovariateType::AgeInYears;
    Common::DateTime birthDate(Common::DateTime("1994.01.18 11:42:35", "%Y.%m.%d %H:%M:%S"));
    Common::DateTime computationTime(Common::DateTime("2004.03.01 23:42:35", "%Y.%m.%d %H:%M:%S"));

    double expected = 10.0;

    EXPECT_DOUBLE_EQ(expected, getAgeIn(ageType, birthDate, computationTime));
}

} // namespace Xpert
} // namespace Tucuxi
