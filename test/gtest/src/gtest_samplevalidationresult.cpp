// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/result/samplevalidationresult.h"

/// \brief Gtest of SampleValidationResult.
///        Tests for SampleValidationResult class.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

class SampleValidationResultTest : public testing::Test
{
public:
    void SetUp() override
    {
        TestUtils::loadTranslationsFile(TestUtils::englishTranslationFile);
    }

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the SampleValidationResult constructor and getters.
///        Check that the constructor properly initializes all members.
TEST_F(SampleValidationResultTest, ConstructorInitialization)
{
    SampleValidationResult result(nullptr, 50, WarningLevel::NORMAL, true);

    EXPECT_EQ(result.getGroupNumberOver99Percentile(), 50);
    EXPECT_EQ(result.isAscending(), true);
    EXPECT_EQ(result.getSource(), nullptr);
}

/// \brief Test cases for the SampleValidationResult::getWarningLevel method.
///        Check that the warning level is correctly computed based on percentile bucket.
TEST_F(SampleValidationResultTest, WarningLevelComputation)
{
    SampleValidationResult ltP10(nullptr, 5, WarningLevel::NORMAL, true);
    SampleValidationResult p10P25(nullptr, 15, WarningLevel::NORMAL, true);
    SampleValidationResult p25P75(nullptr, 50, WarningLevel::NORMAL, true);
    SampleValidationResult p75P90(nullptr, 80, WarningLevel::NORMAL, true);
    SampleValidationResult gtP90(nullptr, 95, WarningLevel::NORMAL, true);

    EXPECT_EQ(ltP10.getWarningLevel(), WarningLevel::CRITICAL);
    EXPECT_EQ(p10P25.getWarningLevel(), WarningLevel::WARNING);
    EXPECT_EQ(p25P75.getWarningLevel(), WarningLevel::NORMAL);
    EXPECT_EQ(p75P90.getWarningLevel(), WarningLevel::WARNING);
    EXPECT_EQ(gtP90.getWarningLevel(), WarningLevel::CRITICAL);
}

/// \brief Test cases for the SampleValidationResult::computeWarning method.
///        Check that warning messages are correctly computed for edge percentiles.
TEST_F(SampleValidationResultTest, WarningMessageComputation)
{
    SampleValidationResult group1(nullptr, 1, WarningLevel::NORMAL, true);
    SampleValidationResult group10(nullptr, 10, WarningLevel::NORMAL, true);
    SampleValidationResult group11(nullptr, 11, WarningLevel::NORMAL, true);
    SampleValidationResult group90(nullptr, 90, WarningLevel::NORMAL, true);
    SampleValidationResult group91(nullptr, 91, WarningLevel::NORMAL, true);
    SampleValidationResult group100(nullptr, 100, WarningLevel::NORMAL, true);

    EXPECT_EQ(group1.getWarning(), "99% of the population is above this measure");
    EXPECT_EQ(group10.getWarning(), "90% of the population is above this measure");
    EXPECT_EQ(group11.getWarning(), "");
    EXPECT_EQ(group90.getWarning(), "");
    EXPECT_EQ(group91.getWarning(), "90% of the population is below this measure");
    EXPECT_EQ(group100.getWarning(), "99% of the population is below this measure");
}

/// \brief Test cases for the SampleValidationResult::getWarningLevel method.
///        Check that the warning level corresponds to the percentile bucket
///        for boundary values (indirectly tests bucketOf through getWarningLevel).
TEST_F(SampleValidationResultTest, PercentileBucketBoundaryValues)
{
    SampleValidationResult p1(nullptr, 1, WarningLevel::NORMAL, true);
    SampleValidationResult p10(nullptr, 10, WarningLevel::NORMAL, true);
    SampleValidationResult p11(nullptr, 11, WarningLevel::NORMAL, true);
    SampleValidationResult p25(nullptr, 25, WarningLevel::NORMAL, true);
    SampleValidationResult p26(nullptr, 26, WarningLevel::NORMAL, true);
    SampleValidationResult p50(nullptr, 50, WarningLevel::NORMAL, true);
    SampleValidationResult p75(nullptr, 75, WarningLevel::NORMAL, true);
    SampleValidationResult p76(nullptr, 76, WarningLevel::NORMAL, true);
    SampleValidationResult p90(nullptr, 90, WarningLevel::NORMAL, true);
    SampleValidationResult p91(nullptr, 91, WarningLevel::NORMAL, true);
    SampleValidationResult p100(nullptr, 100, WarningLevel::NORMAL, true);

    EXPECT_EQ(p1.getWarningLevel(), WarningLevel::CRITICAL);
    EXPECT_EQ(p10.getWarningLevel(), WarningLevel::CRITICAL);
    EXPECT_EQ(p11.getWarningLevel(), WarningLevel::WARNING);
    EXPECT_EQ(p25.getWarningLevel(), WarningLevel::WARNING);
    EXPECT_EQ(p26.getWarningLevel(), WarningLevel::NORMAL);
    EXPECT_EQ(p50.getWarningLevel(), WarningLevel::NORMAL);
    EXPECT_EQ(p75.getWarningLevel(), WarningLevel::NORMAL);
    EXPECT_EQ(p76.getWarningLevel(), WarningLevel::WARNING);
    EXPECT_EQ(p90.getWarningLevel(), WarningLevel::WARNING);
    EXPECT_EQ(p91.getWarningLevel(), WarningLevel::CRITICAL);
    EXPECT_EQ(p100.getWarningLevel(), WarningLevel::CRITICAL);
}

/// \brief Test cases for the SampleValidationResult::isAscending getter.
///        Check that the isAscending flag is correctly stored and retrieved.
TEST_F(SampleValidationResultTest, AscendingFlag)
{
    SampleValidationResult ascending(nullptr, 50, WarningLevel::NORMAL, true);
    SampleValidationResult descending(nullptr, 50, WarningLevel::NORMAL, false);

    EXPECT_EQ(ascending.isAscending(), true);
    EXPECT_EQ(descending.isAscending(), false);
}

/// \brief Test cases for the SampleValidationResult::computePercentileSentence method.
///        Check that the percentile sentence is correctly generated.
TEST_F(SampleValidationResultTest, PercentileSentenceGeneration)
{
    std::string sentence = SampleValidationResult::computePercentileSentence(50, 1.5, "ug/l", "400 mg", true);

    EXPECT_FALSE(sentence.empty());
    EXPECT_TRUE(sentence.find("1.50 ug/l") != std::string::npos);
}

/// \brief The reworded ascending-phase warning must reach the report clean,
TEST_F(SampleValidationResultTest, AscendingSentenceHasNoUnresolvedPlaceholder)
{
    std::string translations = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<translations xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="translations_file.xsd">
    <translation key="txt_measured_of_prefix">The measured level of </translation>
    <translation key="txt_with_dose_prefix"> in patients receiving the drug at a dosage of </translation>
    <translation key="txt_expected_p25_p75"> corresponds to the concentrations usually observed</translation>
    <translation key="txt_sample_in_ascending"> is difficult to interpret as it is taken during the drug absorption phase, which is characterized by high intra- and inter-individual variability. In general, it is recommended to perform sampling later after the dose, once the absorption phase is over, for a more accurate interpretation. Nevertheless this level </translation>
</translations>)";
    TestUtils::loadTranslationsFile(translations);

    std::string sentence = SampleValidationResult::computePercentileSentence(50, 1.5, "ug/l", "400 mg", true);

    EXPECT_FALSE(TestUtils::containsUnresolvedPlaceholder(sentence)) << sentence;
    EXPECT_EQ(sentence.find("unknown translation"), std::string::npos) << sentence;
    EXPECT_EQ(sentence.find("Nh"), std::string::npos) << sentence;
}

/// \brief The placeholder-detection test helper itself: it must flag an unsubstituted "{name}" and ignore ordinary
///        text and stray braces.
TEST(ContainsUnresolvedPlaceholderTest, DetectsUnresolvedPlaceholders)
{
    // The class of defect behind the historical "Nh post-dose" string.
    EXPECT_TRUE(TestUtils::containsUnresolvedPlaceholder("sample at least {hours}h post-dose"));
    EXPECT_TRUE(TestUtils::containsUnresolvedPlaceholder("from {daily_before} to {daily_after}"));

    // Fully substituted or placeholder-free text is clean.
    EXPECT_FALSE(TestUtils::containsUnresolvedPlaceholder("sampling 1h45m post-dose"));
    EXPECT_FALSE(TestUtils::containsUnresolvedPlaceholder("from 450.00 mg to 800.00 mg"));

    // Stray isolated braces are not treated as placeholders.
    EXPECT_FALSE(TestUtils::containsUnresolvedPlaceholder("a { b } c"));
    EXPECT_FALSE(TestUtils::containsUnresolvedPlaceholder("no braces here"));
}

} // namespace Xpert
} // namespace Tucuxi
