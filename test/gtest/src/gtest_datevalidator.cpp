// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/flow/general/datevalidator.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/result/xpertrequestresult.h"
#include "tucucdss-core/utils/xpertutils.h"

/// \brief Gtest of DateValidator.
///        Tests for DateValidator from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

// DateValidator::extractDoseDates tests, used to ensure that no date is missing.
class DateValidatorTest : public testing::Test
{
public:
    std::string querySimpleDoseList = R"(
<?xml version="1.0" ?>
<query xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" xsi:noNamespaceSchemaLocation="xml_query.xsd">
	<queryId>query__00212</queryId>
	<clientId>00212</clientId>
	<date>2023-06-26T15:49:00</date>
	<!-- Date the xml has been sent -->
	<language>en</language>
	<!-- Administrative data -->
	<admin>
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
				<drugId>imatinib</drugId>
				<activePrinciple>imatinib</activePrinciple>
				<brandName/>
				<atc/>
				<!-- All the information regarding the treatment -->
				<treatment>
					<dosageHistory>
						<dosageTimeRange>
							<start>2023-06-11T15:49:00</start>
							<end>2023-06-29T15:49:00</end>
							<dosage>
								<simpleDoseList>
									<unit>mg</unit>
									<formulationAndRoute>
										<formulation>oralSolution</formulation>
										<administrationName>foo bar</administrationName>
										<administrationRoute>oral</administrationRoute>
									</formulationAndRoute>
									<doseList>
										<doseDateValue>
											<doseDate>2023-06-14T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-15T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-16T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-17T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-18T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-19T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-20T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-21T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-22T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-23T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-24T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>10000.0</value>
										</doseDateValue>
										<doseDateValue>
											<doseDate>2023-06-25T15:49:00</doseDate>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<value>17000.0</value>
										</doseDateValue>
									</doseList>
								</simpleDoseList>
							</dosage>
						</dosageTimeRange>
					</dosageHistory>
				</treatment>
				<!-- Samples history -->
				<samples>
					<sample>
						<sampleId>00001</sampleId>
						<sampleDate>2023-06-25T15:49:00</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>imatinib</analyteId>
								<value>133.097396707069</value>
								<unit>ng/ml</unit>
							</concentration>
						</concentrations>
					</sample>
				</samples>
			</drug>
		</drugs>
	</drugTreatment>
	<!-- List of the requests we want the server to take care of -->
	<requests>
		<xpertRequest>
			<drugId>imatinib</drugId>
			<configId>imatinib.gotta2012</configId>
			<output>
				<format>html</format>
				<language>en</language>
			</output>
			<adjustmentDate>2023-06-26T15:49:00</adjustmentDate>
			<options>
				<loadingOption>loadingDoseAllowed</loadingOption>
				<restPeriodOption>restPeriodAllowed</restPeriodOption>
				<targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
				<formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
			</options>
		</xpertRequest>
	</requests>
</query>
)";

    std::string querySingleDoseAtTimeList = R"(
<?xml version="1.0" ?>
<query xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" xsi:noNamespaceSchemaLocation="xml_query.xsd">
	<queryId>query__00212</queryId>
	<clientId>00212</clientId>
	<date>2023-06-26T15:49:00</date>
	<!-- Date the xml has been sent -->
	<language>en</language>
	<!-- Administrative data -->
	<admin>
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
				<drugId>imatinib</drugId>
				<activePrinciple>imatinib</activePrinciple>
				<brandName/>
				<atc/>
				<!-- All the information regarding the treatment -->
				<treatment>
					<dosageHistory>
						<dosageTimeRange>
							<start>2023-06-14T15:49:00</start>
							<end>2023-06-26T15:49:00</end>
							<dosage>
								<singleDoseAtTimeList>
									<singleDoseAtTime>
										<doseDate>2023-06-14T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-15T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-16T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-17T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-18T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-19T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-20T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-21T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-22T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-23T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-24T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
									<singleDoseAtTime>
										<doseDate>2023-06-25T15:49:00</doseDate>
										<dose>
											<value>100.0</value>
											<infusionTimeInMinutes>0</infusionTimeInMinutes>
											<unit>mg</unit>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
										</formulationAndRoute>
									</singleDoseAtTime>
								</singleDoseAtTimeList>
							</dosage>
						</dosageTimeRange>
					</dosageHistory>
				</treatment>
				<!-- Samples history -->
				<samples>
					<sample>
						<sampleId>00001</sampleId>
						<sampleDate>2023-06-25T15:49:00</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>imatinib</analyteId>
								<value>133.097396707069</value>
								<unit>ng/ml</unit>
							</concentration>
						</concentrations>
					</sample>
				</samples>
			</drug>
		</drugs>
	</drugTreatment>
	<!-- List of the requests we want the server to take care of -->
	<requests>
		<xpertRequest>
			<drugId>imatinib</drugId>
			<configId>imatinib.gotta2012</configId>
			<output>
				<format>html</format>
				<language>en</language>
			</output>
			<adjustmentDate>2023-06-26T15:49:00</adjustmentDate>
			<options>
				<loadingOption>loadingDoseAllowed</loadingOption>
				<restPeriodOption>restPeriodAllowed</restPeriodOption>
				<targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
				<formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
			</options>
		</xpertRequest>
	</requests>
</query>
)";

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
                <activePrinciple>something</activePrinciple>
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
                                            <infusionTimeInMinutes>60</infusionTimeInMinutes>
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
                    <sample>
                        <sampleId>0002</sampleId>
                        <sampleDate>2024-05-12T10:15:00</sampleDate>
                        <concentrations>
                            <concentration>
                                <analyteId>imatinib</analyteId>
                                <value>1110</value>
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


    std::string queryWeeklyDosage = R"(
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
                <activePrinciple>something</activePrinciple>
                <brandName>somebrand</brandName>
                <atc>something</atc>
                <treatment>
                    <dosageHistory>
                        <dosageTimeRange>
                            <start>2024-04-11T08:30:00</start>
                            <end>2024-06-13T08:00:00</end>
                            <dosage>
                                <dosageLoop>
                                    <weeklyDosage>
                                        <day>1</day>
                                        <time>08:30:00</time>
                                        <dose>
                                            <value>450</value>
                                            <unit>mg</unit>
                                            <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                        </dose>
                                        <formulationAndRoute>
                                            <formulation>oralSolution</formulation>
                                            <administrationName>foo bar</administrationName>
                                            <administrationRoute>oral</administrationRoute>
                                        </formulationAndRoute>
                                    </weeklyDosage>
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
                    <sample>
                        <sampleId>0002</sampleId>
                        <sampleDate>2024-05-12T10:15:00</sampleDate>
                        <concentrations>
                            <concentration>
                                <analyteId>imatinib</analyteId>
                                <value>1110</value>
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

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Check that extractDoseDates() does not miss a date for the
///        SimpleDoseList
TEST_F(DateValidatorTest, DateValidatorSimpleDoseList)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(querySimpleDoseList, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DosageHistory const& dosageHistory = xpertRequestResult.getTreatment()->getDosageHistory();

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());

    std::vector<Common::DateTime> doseDates = dateValidator->extractDoseDates(dosageHistory);

    std::vector<Common::DateTime> expectedDates = {
            Common::DateTime("2023-06-14 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-15 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-16 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-17 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-18 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-19 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-20 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-21 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-22 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-23 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-24 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-25 15:49:00", "%Y-%m-%d %H:%M:%S"),
    };
    EXPECT_EQ(doseDates, expectedDates);
}


/// \brief Check that extractDoseDates() does not miss a date for the
///        SingleDoseAtTimeList
TEST_F(DateValidatorTest, DateValidatorSingleDoseAtTimeList)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(querySingleDoseAtTimeList, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DosageHistory const& dosageHistory = xpertRequestResult.getTreatment()->getDosageHistory();

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());

    std::vector<Common::DateTime> doseDates = dateValidator->extractDoseDates(dosageHistory);

    std::vector<Common::DateTime> expectedDates = {
            Common::DateTime("2023-06-14 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-15 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-16 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-17 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-18 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-19 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-20 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-21 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-22 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-23 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-24 15:49:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2023-06-25 15:49:00", "%Y-%m-%d %H:%M:%S"),
    };
    EXPECT_EQ(doseDates, expectedDates);
}


/// \brief Check that extractDoseDates() does not miss a date for the
///        DailyDosage
TEST_F(DateValidatorTest, DateValidatorDailyDosage)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryDailyDosage, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DosageHistory const& dosageHistory = xpertRequestResult.getTreatment()->getDosageHistory();

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());

    std::vector<Common::DateTime> doseDates = dateValidator->extractDoseDates(dosageHistory);

    std::vector<Common::DateTime> expectedDates = {
            Common::DateTime("2024-04-11 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-12 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-13 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-14 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-15 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-16 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-17 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-18 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-19 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-20 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-21 08:30:00", "%Y-%m-%d %H:%M:%S"),
    };
    EXPECT_EQ(doseDates, expectedDates);
}


/// \brief Check that extractDoseDates() does not miss a date for the
///        WeeklyDosage
TEST_F(DateValidatorTest, DateValidatorWeeklyDosage)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryWeeklyDosage, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    Core::DosageHistory const& dosageHistory = xpertRequestResult.getTreatment()->getDosageHistory();

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());

    std::vector<Common::DateTime> doseDates = dateValidator->extractDoseDates(dosageHistory);

    std::vector<Common::DateTime> expectedDates = {
            Common::DateTime("2024-04-15 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-22 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-04-29 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-05-6 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-05-13 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-05-20 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-05-27 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-06-3 08:30:00", "%Y-%m-%d %H:%M:%S"),
            Common::DateTime("2024-06-10 08:30:00", "%Y-%m-%d %H:%M:%S"),
    };
    EXPECT_EQ(doseDates, expectedDates);
}


/// \brief Check that extractDoseDates() works for a DosageRepeat wrapping a DailyDosage.
///        Covers DateValidator::extractDoseDates(const DosageRepeat&, ...) (line 227).
TEST_F(DateValidatorTest, DateValidatorDosageRepeat)
{
    // Replace <dosageLoop> with <dosageRepeat><iterations>5</iterations>
    std::string queryStr = queryDailyDosage;
    std::string oldOpen = "<dosageLoop>";
    std::string newOpen = "<dosageRepeat><iterations>5</iterations>";
    size_t pos = queryStr.find(oldOpen);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldOpen.size(), newOpen);

    std::string oldClose = "</dosageLoop>";
    std::string newClose = "</dosageRepeat>";
    pos = queryStr.find(oldClose);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldClose.size(), newClose);

    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    Core::DosageHistory const& dosageHistory = xpertRequestResult.getTreatment()->getDosageHistory();

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    std::vector<Common::DateTime> doseDates = dateValidator->extractDoseDates(dosageHistory);

    // DosageRepeat delegates to inner dailyDosage; same dates as DailyDosage test
    EXPECT_EQ(doseDates.size(), 11u);
    EXPECT_EQ(doseDates.front(), Common::DateTime("2024-04-11 08:30:00", "%Y-%m-%d %H:%M:%S"));
    EXPECT_EQ(doseDates.back(), Common::DateTime("2024-04-21 08:30:00", "%Y-%m-%d %H:%M:%S"));
}


/// \brief Check that extractDoseDates() works for a DosageSequence containing DosageRepeats.
///        Covers DateValidator::extractDoseDates(const DosageSequence&, ...) (line 234)
///        and DateValidator::extractDoseDatesDosageBoundedList() (lines 245-256).
TEST_F(DateValidatorTest, DateValidatorDosageSequence)
{
    // Replace <dosageLoop> with <dosageSequence> to create a sequence with one dailyDosage
    std::string queryStr = queryDailyDosage;
    std::string oldOpen = "<dosageLoop>";
    std::string newOpen = "<dosageSequence>";
    size_t pos = queryStr.find(oldOpen);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldOpen.size(), newOpen);

    std::string oldClose = "</dosageLoop>";
    std::string newClose = "</dosageSequence>";
    pos = queryStr.find(oldClose);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldClose.size(), newClose);

    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    Core::DosageHistory const& dosageHistory = xpertRequestResult.getTreatment()->getDosageHistory();

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    std::vector<Common::DateTime> doseDates = dateValidator->extractDoseDates(dosageHistory);

    EXPECT_EQ(doseDates.size(), 11u);
    EXPECT_EQ(doseDates.front(), Common::DateTime("2024-04-11 08:30:00", "%Y-%m-%d %H:%M:%S"));
    EXPECT_EQ(doseDates.back(), Common::DateTime("2024-04-21 08:30:00", "%Y-%m-%d %H:%M:%S"));
}


/***********************************************************
 *           DateValidator::perform() Tests                *
 ***********************************************************/

/// Covers DateValidator::perform(): early return when treatment is nullptr.
TEST_F(DateValidatorTest, PerformNullTreatment)
{
    // Change the drug element's drugId so the treatment won't match the request
    std::string queryStr = querySimpleDoseList;
    std::string oldId = "<drugId>imatinib</drugId>";
    size_t pos = queryStr.find(oldId);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldId.size(), "<drugId>otherdrug</drugId>");

    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    ASSERT_EQ(xpertRequestResult.getTreatment(), nullptr);

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No drug matching. Could not extract drug treatment.");
}

/// Covers DateValidator::perform(): early return when samples are present but dosage history is empty.
TEST_F(DateValidatorTest, PerformSamplesWithEmptyDosageHistory)
{
    // Remove dosageTimeRange content so dosage history is empty but samples remain
    std::string queryStr = querySimpleDoseList;
    std::string startTag = "<dosageTimeRange>";
    std::string endTag = "</dosageTimeRange>";
    size_t start = queryStr.find(startTag);
    size_t end = queryStr.find(endTag) + endTag.size();
    ASSERT_NE(start, std::string::npos);
    queryStr.erase(start, end - start);

    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): early return when drug model is nullptr.
TEST_F(DateValidatorTest, PerformNullDrugModel)
{
    // Use vector overload which doesn't assign the drug model
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(querySimpleDoseList, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    ASSERT_EQ(xpertRequestResult.getDrugModel(), nullptr);

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): early return when adjustment date is undefined.
TEST_F(DateValidatorTest, PerformNoAdjustmentDate)
{
    // Remove adjustment date from query
    std::string queryStr = querySimpleDoseList;
    std::string target = "<adjustmentDate>2023-06-26T15:49:00</adjustmentDate>";
    size_t pos = queryStr.find(target);
    ASSERT_NE(pos, std::string::npos);
    queryStr.erase(pos, target.size());

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): half-life unit in days ("d") branch (lines 50-51).
TEST_F(DateValidatorTest, PerformHalfLifeInDays)
{
    // Modify the imatinib model to use days for the half-life unit
    std::string modelStr = TestUtils::originalImatinibModelString;
    size_t hlPos = modelStr.find("<halfLife>");
    ASSERT_NE(hlPos, std::string::npos);
    std::string oldUnit = "<unit>h</unit>";
    size_t unitPos = modelStr.find(oldUnit, hlPos);
    ASSERT_NE(unitPos, std::string::npos);
    modelStr.replace(unitPos, oldUnit.size(), "<unit>d</unit>");

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(querySimpleDoseList, modelStr, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    // With half-life in days the computation is different but should still succeed
    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): unsupported half-life unit branch (lines 54-57).
TEST_F(DateValidatorTest, PerformUnsupportedHalfLifeUnit)
{
    // Modify the imatinib model to use an unsupported unit for the half-life
    std::string modelStr = TestUtils::originalImatinibModelString;
    size_t hlPos = modelStr.find("<halfLife>");
    ASSERT_NE(hlPos, std::string::npos);
    std::string oldUnit = "<unit>h</unit>";
    size_t unitPos = modelStr.find(oldUnit, hlPos);
    ASSERT_NE(unitPos, std::string::npos);
    modelStr.replace(unitPos, oldUnit.size(), "<unit>min</unit>");

    unique_ptr<XpertQueryResult> xpertQueryResult;
    EXPECT_THROW(TestUtils::setupEnv(querySimpleDoseList, modelStr, xpertQueryResult), std::runtime_error);
}

/// Covers DateValidator::perform(): error when measurement is too far from doses.
TEST_F(DateValidatorTest, PerformMeasTooFarFromDoses)
{
    // Move the sample date far after the last dose (beyond 5 half-lives = 60h)
    std::string queryStr = querySimpleDoseList;
    std::string oldDate = "<sampleDate>2023-06-25T15:49:00</sampleDate>";
    std::string newDate = "<sampleDate>2023-07-15T15:49:00</sampleDate>";
    size_t pos = queryStr.find(oldDate);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldDate.size(), newDate);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_FALSE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): success with no samples (skips sample date checks).
TEST_F(DateValidatorTest, PerformNoSamples)
{
    // Replace samples section with an empty one
    std::string queryStr = querySimpleDoseList;
    size_t start = queryStr.find("<samples>");
    size_t end = queryStr.find("</samples>") + std::string("</samples>").size();
    ASSERT_NE(start, std::string::npos);
    queryStr.replace(start, end - start, "<samples></samples>");

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
    EXPECT_TRUE(xpertRequestResult.getSampleDateValidationResults().empty());
}

/// Covers DateValidator::perform(): error when all samples are after the adjustment date.
TEST_F(DateValidatorTest, PerformAllSamplesAfterAdjustment)
{
    // Set adjustment date before all sample dates
    std::string queryStr = querySimpleDoseList;
    std::string oldDate = "<adjustmentDate>2023-06-26T15:49:00</adjustmentDate>";
    std::string newDate = "<adjustmentDate>2023-06-20T15:49:00</adjustmentDate>";
    size_t pos = queryStr.find(oldDate);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldDate.size(), newDate);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_FALSE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): warning when sample is too far from the adjustment date.
TEST_F(DateValidatorTest, PerformSampleTooFarFromAdjustment)
{
    // Set adjustment date far after the last sample (more than 5 half-lives = 60h)
    std::string queryStr = querySimpleDoseList;
    std::string oldDate = "<adjustmentDate>2023-06-26T15:49:00</adjustmentDate>";
    std::string newDate = "<adjustmentDate>2023-07-10T15:49:00</adjustmentDate>";
    size_t pos = queryStr.find(oldDate);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldDate.size(), newDate);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
    ASSERT_EQ(xpertRequestResult.getSampleDateValidationResults().size(), 1u);
    EXPECT_FALSE(xpertRequestResult.getSampleDateValidationResults()[0].getWarning().empty());
}

/// Covers DateValidator::checkDoseToMeasureDistances(): sample date is before
/// all dose dates, so upper_bound returns begin() and the function returns false.
TEST_F(DateValidatorTest, PerformMeasureBeforeAllDoses)
{
    // Move the sample date before the first dose (2023-06-14)
    std::string queryStr = querySimpleDoseList;
    std::string oldDate = "<sampleDate>2023-06-25T15:49:00</sampleDate>";
    std::string newDate = "<sampleDate>2023-06-10T15:49:00</sampleDate>";
    size_t pos = queryStr.find(oldDate);
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, oldDate.size(), newDate);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_FALSE(xpertRequestResult.getErrorMessage().empty());
}

/// Covers DateValidator::perform(): successful validation with no errors or warnings.
TEST_F(DateValidatorTest, PerformSuccess)
{
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(querySimpleDoseList, TestUtils::originalImatinibModelString, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    DateValidator* dateValidator = dynamic_cast<DateValidator*>(TestUtils::flowStepProvider.getDateValidator());
    dateValidator->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.getErrorMessage().empty());
}


} // namespace Xpert
} // namespace Tucuxi
