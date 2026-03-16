// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/targetevent.h"

#include "testutils.h"
#include "tucucdss-core/flow/general/justificationcreator.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/result/xpertrequestresult.h"

/// \brief Gtest of JustificationCreator.
///        Tests for JustificationCreator from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

class JustificationCreatorTest : public testing::Test
{
public:
    // A query with a daily dosage (450 mg oral, daily at 08:30).
    std::string queryDailyDosage = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<query xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" xsi:noNamespaceSchemaLocation="xml_query.xsd">
    <date>2025-03-12T00:00:00</date>
    <admin>
    </admin>
    <drugTreatment>
        <patient>
            <covariates>
            </covariates>
        </patient>
        <drugs>
            <drug>
                <drugId>imatinib</drugId>
                <activePrinciple>imatinib</activePrinciple>
                <brandName>somebrand</brandName>
                <atc>something</atc>
                <treatment>
                    <dosageHistory>
                        <dosageTimeRange>
                            <start>2024-04-11T08:30:00</start>
                            <end>2024-04-22T08:00:00</end>
                            <dosage>
                                <dosageLoop>
                                    <dailyDosage>
                                        <time>08:30:00</time>
                                        <dose>
                                            <value>450</value>
                                            <unit>mg</unit>
                                            <infusionTimeInMinutes>0</infusionTimeInMinutes>
                                        </dose>
                                        <formulationAndRoute>
                                            <formulation>oralSolution</formulation>
                                            <administrationName>foo bar</administrationName>
                                            <administrationRoute>oral</administrationRoute>
                                        </formulationAndRoute>
                                    </dailyDosage>
                                </dosageLoop>
                            </dosage>
                        </dosageTimeRange>
                    </dosageHistory>
                </treatment>
                <samples>
                    <sample>
                        <sampleId>0001</sampleId>
                        <sampleDate>2024-04-30T10:30:00</sampleDate>
                        <concentrations>
                            <concentration>
                                <analyteId>imatinib</analyteId>
                                <value>800</value>
                                <unit>ug/l</unit>
                            </concentration>
                        </concentrations>
                    </sample>
                </samples>
            </drug>
        </drugs>
    </drugTreatment>
    <requests>
        <xpertRequest>
            <drugId>imatinib</drugId>
            <configId>imatinib.gotta2012</configId>
            <output>
                <format>html</format>
                <language>en</language>
            </output>
            <adjustmentDate>2024-05-12T10:15:00</adjustmentDate>
            <options>
                <loadingOption>loadingDoseAllowed</loadingOption>
                <restPeriodOption>noRestPeriod</restPeriodOption>
                <targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
                <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
            </options>
        </xpertRequest>
    </requests>
</query>
)";

    // A query with an empty dosage history (no doses).
    std::string queryEmptyDosage = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<query xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" xsi:noNamespaceSchemaLocation="xml_query.xsd">
    <date>2025-03-12T00:00:00</date>
    <admin>
    </admin>
    <drugTreatment>
        <patient>
            <covariates>
            </covariates>
        </patient>
        <drugs>
            <drug>
                <drugId>imatinib</drugId>
                <activePrinciple>imatinib</activePrinciple>
                <brandName>somebrand</brandName>
                <atc>something</atc>
                <treatment>
                    <dosageHistory>
                    </dosageHistory>
                </treatment>
                <samples>
                    <sample>
                        <sampleId>0001</sampleId>
                        <sampleDate>2024-04-30T10:30:00</sampleDate>
                        <concentrations>
                            <concentration>
                                <analyteId>imatinib</analyteId>
                                <value>800</value>
                                <unit>ug/l</unit>
                            </concentration>
                        </concentrations>
                    </sample>
                </samples>
            </drug>
        </drugs>
    </drugTreatment>
    <requests>
        <xpertRequest>
            <drugId>imatinib</drugId>
            <configId>imatinib.gotta2012</configId>
            <output>
                <format>html</format>
                <language>en</language>
            </output>
            <adjustmentDate>2024-05-12T10:15:00</adjustmentDate>
            <options>
                <loadingOption>loadingDoseAllowed</loadingOption>
                <restPeriodOption>noRestPeriod</restPeriodOption>
                <targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
                <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
            </options>
        </xpertRequest>
    </requests>
</query>
)";

    // Common formulation and route used to build dosage objects in tests.
    Core::FormulationAndRoute oralRoute{Core::Formulation::OralSolution, Core::AdministrationRoute::Oral};

    /// \brief Build a DosageAdjustment with a single DosageTimeRange containing the given dosage,
    ///        and a target evaluation with specified score and value.
    /// \param _dosage The dosage to place in the adjustment's history.
    /// \param _startDate Start date of the dosage time range.
    /// \param _endDate End date of the dosage time range.
    /// \param _targetValue The target evaluation value (current exposure).
    /// \param _targetBest The target best value for comparison.
    /// \param _score The target score.
    Core::DosageAdjustment buildAdjustment(
            const Core::Dosage& _dosage,
            const Common::DateTime& _startDate,
            const Common::DateTime& _endDate,
            double _targetValue,
            double _targetBest,
            double _score)
    {
        Core::DosageAdjustment adj;
        adj.m_history.addTimeRange(Core::DosageTimeRange(_startDate, _endDate, _dosage));

        Core::TargetEvaluationResult targetResult(
                Core::TargetType::Residual, _score, _targetValue, Common::TucuUnit("ug/l"));
        Core::TargetEvent targetEvent = Core::TargetEvent::createTargetEventWithoutTimeAndMic(
                Core::ActiveMoietyId("imatinib"),
                Core::TargetType::Residual,
                Common::TucuUnit("ug/l"),
                Common::TucuUnit("ug/l"),
                500.0,
                _targetBest,
                1500.0);
        targetResult.setTarget(targetEvent);
        adj.m_targetsEvaluation.push_back(targetResult);
        return adj;
    }

    /// \brief Set up AdjustmentData on the XpertRequestResult with a single adjustment
    ///        and matching current dosage with score.
    /// \param _xpertRequestResult The result to configure.
    /// \param _adj The best adjustment to add.
    /// \param _currentTargetValue The current treatment's target evaluation value.
    /// \param _currentTargetBest The target best value for the current evaluation.
    void setupAdjustmentData(
            XpertRequestResult& _xpertRequestResult,
            const Core::DosageAdjustment& _adj,
            double _currentTargetValue,
            double _currentTargetBest)
    {
        auto adjustmentData = make_unique<Core::AdjustmentData>("justification_test");
        adjustmentData->addAdjustment(_adj);

        // Build the current dosage with score (used for exposure comparison).
        Core::DosageAdjustment currentDosageWithScore;
        Core::TargetEvaluationResult currentTarget(
                Core::TargetType::Residual, 0.5, _currentTargetValue, Common::TucuUnit("ug/l"));
        Core::TargetEvent currentTargetEvent = Core::TargetEvent::createTargetEventWithoutTimeAndMic(
                Core::ActiveMoietyId("imatinib"),
                Core::TargetType::Residual,
                Common::TucuUnit("ug/l"),
                Common::TucuUnit("ug/l"),
                500.0,
                _currentTargetBest,
                1500.0);
        currentTarget.setTarget(currentTargetEvent);
        currentDosageWithScore.m_targetsEvaluation.push_back(currentTarget);
        adjustmentData->setCurrentDosageWithScore(currentDosageWithScore);

        _xpertRequestResult.setAdjustmentData(move(adjustmentData));
    }
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief When adjustmentData is null, perform() should return early
///        without setting any justification.
TEST_F(JustificationCreatorTest, PerformNullAdjustmentData)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Do NOT set adjustmentData — it remains nullptr.
    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    // The justification should remain default-initialized.
    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getFirstDoseText(), "");
}


/// \brief SIMPLE justification with a DailyDose adjustment, dose increase.
///        Old treatment: 450 mg daily. New adjustment: 600 mg daily.
///        Current exposure below target → BELOW.
TEST_F(JustificationCreatorTest, PerformSimpleDailyDose)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Build a daily dose adjustment: 600 mg, daily at 08:30.
    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 800.0, 1000.0, 0.8);

    // Current exposure is 700 (below target best of 1000) → BELOW.
    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationType(), JustificationType::SIMPLE);
    EXPECT_EQ(justification.getJustificationDoseSign(), JustificationDoseSign::INCREASE);
    EXPECT_EQ(justification.getJustificationExposureSign(), JustificationExposureSign::BELOW);
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::EQUAL);
    EXPECT_DOUBLE_EQ(justification.getFirstDoseValue(), 600.0);
    EXPECT_FALSE(justification.getFirstDoseText().empty());
}


/// \brief SIMPLE justification with a LastingDose adjustment.
///        Old treatment: 450 mg daily. New adjustment: 500 mg every 12h (lasting).
TEST_F(JustificationCreatorTest, PerformSimpleLastingDose)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Build a lasting dose adjustment: 500 mg every 12h.
    Core::LastingDose lastingDose(
            500.0, Common::TucuUnit("mg"), oralRoute, Common::Duration(), Common::Duration(std::chrono::hours(12)));
    Core::DosageLoop dosageLoop(lastingDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationType(), JustificationType::SIMPLE);
    EXPECT_EQ(justification.getJustificationDoseSign(), JustificationDoseSign::INCREASE);
    EXPECT_DOUBLE_EQ(justification.getFirstDoseValue(), 500.0);
    EXPECT_FALSE(justification.getFirstDoseText().empty());
}


/// \brief SIMPLE justification with a WeeklyDose adjustment.
///        Old treatment: 450 mg daily. New adjustment: 800 mg weekly on Monday.
TEST_F(JustificationCreatorTest, PerformSimpleWeeklyDose)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Build a weekly dose adjustment: 800 mg every Monday at 08:00.
    Core::WeeklyDose weeklyDose(
            800.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))),
            date::weekday{1u}); // Monday
    Core::DosageLoop dosageLoop(weeklyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationType(), JustificationType::SIMPLE);
    EXPECT_EQ(justification.getJustificationDoseSign(), JustificationDoseSign::INCREASE);
    EXPECT_DOUBLE_EQ(justification.getFirstDoseValue(), 800.0);
    EXPECT_FALSE(justification.getFirstDoseText().empty());
}


/// \brief DOUBLE justification (2 time ranges in the adjustment).
///        The first dose is a loading dose, the second is the maintenance dose.
TEST_F(JustificationCreatorTest, PerformDoubleAdjustment)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Build an adjustment with 2 time ranges.
    Common::DateTime adjStart1("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd1("2024-05-15 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjStart2("2024-05-15 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd2("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");

    Core::DailyDose loadingDose(
            800.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))));
    Core::DosageLoop loadingLoop(loadingDose);
    Core::DailyDose maintenanceDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))));
    Core::DosageLoop maintenanceLoop(maintenanceDose);

    Core::DosageAdjustment adj;
    adj.m_history.addTimeRange(Core::DosageTimeRange(adjStart1, adjEnd1, loadingLoop));
    adj.m_history.addTimeRange(Core::DosageTimeRange(adjStart2, adjEnd2, maintenanceLoop));

    Core::TargetEvaluationResult targetResult(Core::TargetType::Residual, 0.9, 900.0, Common::TucuUnit("ug/l"));
    Core::TargetEvent targetEvent = Core::TargetEvent::createTargetEventWithoutTimeAndMic(
            Core::ActiveMoietyId("imatinib"),
            Core::TargetType::Residual,
            Common::TucuUnit("ug/l"),
            Common::TucuUnit("ug/l"),
            500.0,
            1000.0,
            1500.0);
    targetResult.setTarget(targetEvent);
    adj.m_targetsEvaluation.push_back(targetResult);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationType(), JustificationType::DOUBLE);
    EXPECT_DOUBLE_EQ(justification.getFirstDoseValue(), 800.0);
    EXPECT_FALSE(justification.getSecondDoseText().empty());
    EXPECT_EQ(justification.getSecondDoseDate(), adjStart2);
}


/// \brief When the treatment has an empty dosage history, old dose = -1.
///        Justification dose sign should be NEW.
TEST_F(JustificationCreatorTest, PerformEmptyDosageHistory)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryEmptyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationDoseSign(), JustificationDoseSign::NEW);
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::NEW);
}


/// \brief Dose decrease: old dose (450) > new dose (300) → DECREASE.
TEST_F(JustificationCreatorTest, PerformDoseDecrease)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            300.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 1200.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationDoseSign(), JustificationDoseSign::DECREASE);
}


/// \brief Dose equal: old dose (450) == new dose (450) → EQUAL.
TEST_F(JustificationCreatorTest, PerformDoseEqual)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            450.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 1000.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationDoseSign(), JustificationDoseSign::EQUAL);
}


/// \brief Exposure above: current exposure (1200) > target best (1000) → ABOVE.
TEST_F(JustificationCreatorTest, PerformExposureAbove)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    // Current exposure 1200 > target best 1000 → ABOVE.
    setupAdjustmentData(xpertRequestResult, adj, 1200.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationExposureSign(), JustificationExposureSign::ABOVE);
}


/// \brief Exposure below: current exposure (700) < target best (1000) → BELOW.
TEST_F(JustificationCreatorTest, PerformExposureBelow)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    // Current exposure 700 < target best 1000 → BELOW.
    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationExposureSign(), JustificationExposureSign::BELOW);
}


/// \brief Exposure equal: current exposure (1000) == target best (1000) → EQUAL.
TEST_F(JustificationCreatorTest, PerformExposureEqual)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    // Current exposure 1000 == target best 1000 → EQUAL.
    setupAdjustmentData(xpertRequestResult, adj, 1000.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationExposureSign(), JustificationExposureSign::EQUAL);
}


/// \brief Interval higher: old daily (interval=1), new weekly (interval=3) → HIGHER.
TEST_F(JustificationCreatorTest, PerformIntervalHigher)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // New adjustment is weekly → higher interval than old daily.
    Core::WeeklyDose weeklyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))),
            date::weekday{1u});
    Core::DosageLoop dosageLoop(weeklyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::HIGHER);
}


/// \brief Interval lower: old daily (interval=1), new lasting 12h (interval=0) → LOWER.
TEST_F(JustificationCreatorTest, PerformIntervalLower)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // New adjustment is lasting 12h → lower interval than old daily.
    Core::LastingDose lastingDose(
            500.0, Common::TucuUnit("mg"), oralRoute, Common::Duration(), Common::Duration(std::chrono::hours(12)));
    Core::DosageLoop dosageLoop(lastingDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::LOWER);
}


/// \brief Interval equal: old daily, new daily → EQUAL.
TEST_F(JustificationCreatorTest, PerformIntervalEqual)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // New adjustment is daily → same interval as old daily.
    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8)) + Common::Duration(std::chrono::minutes(30))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::EQUAL);
}


/// \brief Interval new: empty dosage history → NEW interval.
TEST_F(JustificationCreatorTest, PerformIntervalNew)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryEmptyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))));
    Core::DosageLoop dosageLoop(dailyDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::NEW);
}


/// \brief DosageRepeat dispatch: verify that perform() handles a DosageRepeat correctly.
TEST_F(JustificationCreatorTest, PerformDosageRepeat)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Build a DosageRepeat: 5 repetitions of daily 600 mg.
    Core::DailyDose dailyDose(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))));
    Core::DosageRepeat dosageRepeat(dailyDose, 5);
    Core::DosageLoop dosageLoop(dosageRepeat);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationType(), JustificationType::SIMPLE);
    EXPECT_DOUBLE_EQ(justification.getFirstDoseValue(), 600.0);
    EXPECT_FALSE(justification.getFirstDoseText().empty());
}


/// \brief DosageSequence dispatch: verify that perform() handles a DosageSequence correctly.
TEST_F(JustificationCreatorTest, PerformDosageSequence)
{
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Build a DosageSequence: two daily doses (600 mg then 400 mg).
    Core::DailyDose dailyDose1(
            600.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(8))));
    Core::DailyDose dailyDose2(
            400.0,
            Common::TucuUnit("mg"),
            oralRoute,
            Common::Duration(),
            Common::TimeOfDay(Common::Duration(std::chrono::hours(20))));
    Core::DosageSequence dosageSequence(dailyDose1);
    dosageSequence.addDosage(dailyDose2);
    Core::DosageLoop dosageLoop(dosageSequence);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationType(), JustificationType::SIMPLE);
    // DosageSequence iterates dosages; the last one processed sets the dose value.
    EXPECT_FALSE(justification.getFirstDoseText().empty());
}


/// \brief Lasting dose interval comparison: both old and new are lasting,
///        old 12h < new 24h → HIGHER.
TEST_F(JustificationCreatorTest, PerformLastingIntervalHigher)
{
    // Use a query with a lasting dose (12h) as old treatment.
    // We create the query by replacing the daily dosage with a lasting dosage.
    std::string queryLastingDosage = queryDailyDosage;
    {
        // Replace dailyDosage block with lastingDosage block.
        std::string oldBlock =
                "<dosageLoop>\n"
                "                                    <dailyDosage>\n"
                "                                        <time>08:30:00</time>\n"
                "                                        <dose>\n"
                "                                            <value>450</value>\n"
                "                                            <unit>mg</unit>\n"
                "                                            <infusionTimeInMinutes>0</infusionTimeInMinutes>\n"
                "                                        </dose>\n"
                "                                        <formulationAndRoute>\n"
                "                                            <formulation>oralSolution</formulation>\n"
                "                                            <administrationName>foo bar</administrationName>\n"
                "                                            <administrationRoute>oral</administrationRoute>\n"
                "                                        </formulationAndRoute>\n"
                "                                    </dailyDosage>\n"
                "                                </dosageLoop>";
        std::string newBlock =
                "<dosageLoop>\n"
                "                                    <lastingDosage>\n"
                "                                        <interval>12:00:00</interval>\n"
                "                                        <dose>\n"
                "                                            <value>450</value>\n"
                "                                            <unit>mg</unit>\n"
                "                                            <infusionTimeInMinutes>0</infusionTimeInMinutes>\n"
                "                                        </dose>\n"
                "                                        <formulationAndRoute>\n"
                "                                            <formulation>oralSolution</formulation>\n"
                "                                            <administrationName>foo bar</administrationName>\n"
                "                                            <administrationRoute>oral</administrationRoute>\n"
                "                                        </formulationAndRoute>\n"
                "                                    </lastingDosage>\n"
                "                                </dosageLoop>";
        size_t pos = queryLastingDosage.find(oldBlock);
        if (pos != std::string::npos) {
            queryLastingDosage.replace(pos, oldBlock.length(), newBlock);
        }
    }

    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryLastingDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // New adjustment: lasting 24h → higher interval than old 12h.
    Core::LastingDose lastingDose(
            500.0, Common::TucuUnit("mg"), oralRoute, Common::Duration(), Common::Duration(std::chrono::hours(24)));
    Core::DosageLoop dosageLoop(lastingDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::HIGHER);
}


/// \brief Lasting dose interval comparison: both old and new are lasting,
///        old 24h > new 12h → LOWER.
TEST_F(JustificationCreatorTest, PerformLastingIntervalLower)
{
    // Use a query with a lasting dose (24h) as old treatment.
    std::string queryLastingDosage = queryDailyDosage;
    {
        std::string oldBlock =
                "<dosageLoop>\n"
                "                                    <dailyDosage>\n"
                "                                        <time>08:30:00</time>\n"
                "                                        <dose>\n"
                "                                            <value>450</value>\n"
                "                                            <unit>mg</unit>\n"
                "                                            <infusionTimeInMinutes>0</infusionTimeInMinutes>\n"
                "                                        </dose>\n"
                "                                        <formulationAndRoute>\n"
                "                                            <formulation>oralSolution</formulation>\n"
                "                                            <administrationName>foo bar</administrationName>\n"
                "                                            <administrationRoute>oral</administrationRoute>\n"
                "                                        </formulationAndRoute>\n"
                "                                    </dailyDosage>\n"
                "                                </dosageLoop>";
        std::string newBlock =
                "<dosageLoop>\n"
                "                                    <lastingDosage>\n"
                "                                        <interval>24:00:00</interval>\n"
                "                                        <dose>\n"
                "                                            <value>450</value>\n"
                "                                            <unit>mg</unit>\n"
                "                                            <infusionTimeInMinutes>0</infusionTimeInMinutes>\n"
                "                                        </dose>\n"
                "                                        <formulationAndRoute>\n"
                "                                            <formulation>oralSolution</formulation>\n"
                "                                            <administrationName>foo bar</administrationName>\n"
                "                                            <administrationRoute>oral</administrationRoute>\n"
                "                                        </formulationAndRoute>\n"
                "                                    </lastingDosage>\n"
                "                                </dosageLoop>";
        size_t pos = queryLastingDosage.find(oldBlock);
        if (pos != std::string::npos) {
            queryLastingDosage.replace(pos, oldBlock.length(), newBlock);
        }
    }

    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryLastingDosage, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // New adjustment: lasting 12h → lower interval than old 24h.
    Core::LastingDose lastingDose(
            500.0, Common::TucuUnit("mg"), oralRoute, Common::Duration(), Common::Duration(std::chrono::hours(12)));
    Core::DosageLoop dosageLoop(lastingDose);
    Common::DateTime adjStart("2024-05-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Common::DateTime adjEnd("2024-06-12 10:15:00", "%Y-%m-%d %H:%M:%S");
    Core::DosageAdjustment adj = buildAdjustment(dosageLoop, adjStart, adjEnd, 900.0, 1000.0, 0.9);

    setupAdjustmentData(xpertRequestResult, adj, 700.0, 1000.0);

    JustificationCreator justificationCreator;
    justificationCreator.perform(xpertRequestResult);

    const Justification& justification = xpertRequestResult.getJustification();
    EXPECT_EQ(justification.getJustificationInterval(), JustificationInterval::LOWER);
}


} // namespace Xpert
} // namespace Tucuxi
