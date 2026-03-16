// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/query/configimport.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/result/xpertrequestresult.h"
#include "tucucdss-core/utils/xpertutils.h"


using namespace std;

namespace Tucuxi {
namespace Xpert {

class CovariateCheckerTest : public testing::Test
{
public:
    std::string translationFile = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<translations
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="translations_file.xsd">

    <!-- Dosages -->
    <translation key="maximum_dosage_warning">Maximum recommended dosage reached</translation>
    <translation key="minimum_dosage_warning">Minimum recommended dosage reached</translation>

    <!-- Samples -->
    <translation key="population_below">% of the population is below this measure</translation>
    <translation key="population_above">% of the population is above this measure</translation>

    <!-- Report common -->
    <translation key="drug">Drug</translation>
    <translation key="none">None</translation>
    <translation key="from">From</translation>
    <translation key="to">To</translation>
    <translation key="date">Date</translation>
    <translation key="hour_acronym">h</translation>

    <!-- Report header -->
    <translation key="tuberxpert_report">TuberXpert report</translation>
    <translation key="computed_on">Computed on</translation>

    <!-- Report intro -->
    <translation key="id">ID</translation>
    <translation key="last_dose">Last dose</translation>
    <translation key="drug_model">Drug model</translation>

    <!-- Report contacts -->
    <translation key="contacts">Contacts</translation>
    <translation key="mandator">Mandator</translation>
    <translation key="patient">Patient</translation>
    <translation key="name">Name</translation>
    <translation key="address">Address</translation>
    <translation key="phone">Phone</translation>
    <translation key="email">Email</translation>
    <translation key="professional">professional</translation>
    <translation key="private">private</translation>

    <!-- Report clinical data -->
    <translation key="clinical_data">Clinical data</translation>

    <!-- Report Covariates -->
    <translation key="covariates">Covariates</translation>
    <translation key="value">Value</translation>
    <translation key="source_patient">patient</translation>
    <translation key="source_default">default</translation>
    <translation key="yes">Yes</translation>
    <translation key="no">No</translation>
    <translation key="undefined">Undefined</translation>
    <translation key="man">Man</translation>
    <translation key="woman">Woman</translation>

    <!-- Report Treatment -->
    <translation key="treatment">Treatment</translation>
    <translation key="type">Type</translation>
    <translation key="continually">continually</translation>
    <translation key="at_steady_state">at steady state with last dose on</translation>
    <translation key="times">time(s)</translation>
    <translation key="offset">offset</translation>
    <translation key="interval">interval</translation>
    <translation key="daily_at">daily at</translation>
    <translation key="every">every</translation>
    <translation key="day_1">Monday</translation>
    <translation key="day_2">Tuesday</translation>
    <translation key="day_3">Wednesday</translation>
    <translation key="day_4">Thursday</translation>
    <translation key="day_5">Friday</translation>
    <translation key="day_6">Saturday</translation>
    <translation key="day_7">Sunday</translation>
    <translation key="at">at</translation>
    <translation key="dosage">Dosage</translation>

    <translation key="oral">oral</translation>
    <translation key="nasal">nasal</translation>
    <translation key="rectal">rectal</translation>
    <translation key="vaginal">vaginal</translation>
    <translation key="sublingual">sublingual</translation>
    <translation key="transdermal">transdermal</translation>
    <translation key="subcutaneous">subcutaneous</translation>
    <translation key="intramuscular">intramuscular</translation>
    <translation key="intravenous_drip">intravenous drip</translation>
    <translation key="intravenous_bolus">intravenous bolus</translation>

    <!-- Report Samples -->
    <translation key="samples">Samples</translation>
    <translation key="measure">Measure</translation>
    <translation key="percentile">Percentile</translation>

    <!-- Report Adjustments -->
    <translation key="adjustments">Adjustments found</translation>
    <translation key="intro_phrase">Based on the above information, the following adjustments are relevant to achieve the targets.</translation>
    <translation key="per_interval">Per interval</translation>
    <translation key="displayed_adjustments">Displayed adjustments</translation>
    <translation key="displayed_adjustment">Displayed adjustment</translation>
    <translation key="score">Score</translation>
    <translation key="adjustment_suggested">Adjustment suggested</translation>
    <translation key="suggestion_phrase">Considering the above information, TuberXpert recommends the following adjustment.</translation>

    <!-- Report Targets -->
    <translation key="targets_phrase">With the best suggestion, the targets are expected to change as follows.</translation>
    <translation key="unknown">Unknown</translation>
    <translation key="residual">Residual</translation>
    <translation key="peak">Peak</translation>
    <translation key="mean">Mean</translation>
    <translation key="auc">Area under the curve</translation>
    <translation key="auc24">Area under the curve for 24 hours</translation>
    <translation key="cumulative_auc">Cumulative area under the curve</translation>
    <translation key="auc_over_mic">Area under the curve over minimum inhibitory concentration</translation>
    <translation key="auc24_over_mic">Area under the curve for 24 hours over minimum inhibitory concentration</translation>
    <translation key="time_over_mic">Time over minimum inhibitory concentration</translation>
    <translation key="auc_divided_by_mic">Area under the curve divided by minimum inhibitory concentration</translation>
    <translation key="auc24_divided_by_mic">Area under the curve for 24 hours divided by minimum inhibitory concentration</translation>
    <translation key="peak_divided_by_mic">Peak divided by minimum inhibitory concentration</translation>
    <translation key="residual_divided_by_mic">Residual divided by minimum inhibitory concentration</translation>
    <translation key="fraction_time_over_mic">Fraction time over minimum inhibitory concentration</translation>

    <translation key="inefficacy">Inefficacy</translation>
    <translation key="min">Min</translation>
    <translation key="best">Best</translation>
    <translation key="max">Max</translation>
    <translation key="toxicity">Toxicity</translation>

    <!-- Report Pharmacokinetic parameters -->
    <translation key="pharmacokinetic_parameters">Pharmacokinetic parameters</translation>
    <translation key="typical_patient">Typical patient</translation>
    <translation key="a_priori">A priori</translation>
    <translation key="a_posteriori">A posteriori</translation>

    <!-- Report Predictions -->
    <translation key="predictions">Predictions</translation>
    <translation key="extrapolated_steady_state_auc24">Extrapolated steady state AUC24</translation>
    <translation key="steady_state_peak">Steady state peak</translation>
    <translation key="steady_state_residual">Steady state residual</translation>

    <!-- Report Computation covariates -->
    <translation key="computation_covariates">Covariates used for computation</translation>
    <translation key="covariate">Covariate</translation>

    <!-- Error advertise -->
    <translation key="warning">Warning</translation>
    <translation key="error">Error</translation>

    <!-- Covarriable Checker -->
    <translation key="no_covar_given">Was not given for calculation!</translation>
    <translation key="invalid_range">Value should be between</translation>
    <translation key="and">and</translation>

    <!-- Configuration range message -->
    <translation key="exclude_range">exclude</translation>
    <translation key="include_range">include</translation>

</translations>
    )";


    std::string xmlString = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
			<configId>imatinib.gotta2012</configId>
			<drugId>imatinib</drugId>
			<drugModelId>ch.tucuxi.imatinib.gotta2012</drugModelId>
			<description>
				<desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
			</description>
			<covariates>
				<covariate>
					<covariateId>age</covariateId>
					<covariateType>ageInYears</covariateType>
                    <dataType>double</dataType>
					<validRange>
						<constraint>
							<value>18</value>
							<operator>gt</operator>
						</constraint>
						<constraint>
							<value>90</value>
							<operator>lt</operator>
						</constraint>
					</validRange>
					<typicalRange>
						<constraint>
							<value>20</value>
							<operator>geq</operator>
						</constraint>
						<constraint>
							<value>88</value>
							<operator>leq</operator>
						</constraint>
					</typicalRange>
				</covariate>
				<covariate>
					<covariateId>bodyweight</covariateId>
					<covariateType>standard</covariateType>
                    <dataType>double</dataType>
					<validRange>
						<constraint>
							<value>40</value>
							<operator>gt</operator>
						</constraint>
						<constraint>
							<value>130</value>
							<operator>lt</operator>
						</constraint>
					</validRange>
					<typicalRange>
						<constraint>
							<value>44</value>
							<operator>geq</operator>
						</constraint>
						<constraint>
							<value>110</value>
							<operator>leq</operator>
						</constraint>
					</typicalRange>
				</covariate>
				<covariate>
					<covariateId>sex</covariateId>
					<covariateType>sex</covariateType>
                    <dataType>double</dataType>
					<validRange>
						<constraint>
							<value>0.0</value>
							<operator>geq</operator>
						</constraint>
						<constraint>
							<value>1.0</value>
							<operator>leq</operator>
						</constraint>
					</validRange>
					<typicalRange>
						<constraint>
							<value>0.0</value>
							<operator>geq</operator>
						</constraint>
						<constraint>
							<value>1.0</value>
							<operator>leq</operator>
						</constraint>
					</typicalRange>
				</covariate>
			</covariates>
		</config>
            </configs>
        </cdssConfig>
    )";


    std::string queryStringSuccess = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<query version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

    <date>2022-06-24T13:45:30</date> <!-- Date that defines the computation time -->

     <drugTreatment>
        <!-- All the information regarding the patient -->
        <patient>
            <name>Bruno</name>
            <address>Ciao</address>
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
                    <value>75</value>
                    <unit>kg</unit>
                        <dataType>double</dataType>
                    <nature>discrete</nature>
                </covariate>
                <covariate>
                    <covariateId>sex</covariateId>
                    <date>2017-07-06T08:00:00</date>
                    <value>0</value>
                    <unit></unit>
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
                                            <absorptionModel>extravascular</absorptionModel>
                                        </formulationAndRoute>
                                    </lastingDosage>
                                </dosageLoop>
                            </dosage>
                        </dosageTimeRange>
                    </dosageHistory>
                </treatment>
                <samples>
                    <sample>
                        <sampleId>123456</sampleId>
                        <sampleDate>2018-07-07T06:00:00</sampleDate>
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
                                <value>0.6</value>
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
            </drug>
        </drugs>
    </drugTreatment>

    <requests>
        <!-- List of the requests we want the TuberXpert to take care of -->
        <xpertRequest>
            <drugId>imatinib</drugId>
            <configId>imatinib.gotta2012</configId>
            <output>
                <format>html</format>
                <language>fr</language>
            </output>
            <adjustmentDate>2018-07-11T08:00:00</adjustmentDate>
            <options>
                <loadingOption>loadingDoseAllowed</loadingOption>
                <restPeriodOption>restPeriodAllowed</restPeriodOption>
                <targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
                <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
            </options>
        </xpertRequest>
    </requests>
</query>)";

    std::string queryStringError = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<query version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

    <date>2022-06-24T13:45:30</date> <!-- Date that defines the computation time -->

     <drugTreatment>
        <!-- All the information regarding the patient -->
        <patient>
            <name>Bruno</name>
            <address>Ciao</address>
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
                    <value>35</value>
                    <unit>kg</unit>
                        <dataType>double</dataType>
                    <nature>discrete</nature>
                </covariate>
                <covariate>
                    <covariateId>sex</covariateId>
                    <date>2017-07-06T08:00:00</date>
                    <value>0</value>
                    <unit></unit>
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
                                            <absorptionModel>extravascular</absorptionModel>
                                        </formulationAndRoute>
                                    </lastingDosage>
                                </dosageLoop>
                            </dosage>
                        </dosageTimeRange>
                    </dosageHistory>
                </treatment>
                <samples>
                    <sample>
                        <sampleId>123456</sampleId>
                        <sampleDate>2018-07-07T06:00:00</sampleDate>
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
                                <value>0.6</value>
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
            </drug>
        </drugs>
    </drugTreatment>

    <requests>
        <!-- List of the requests we want the TuberXpert to take care of -->
        <xpertRequest>
            <drugId>imatinib</drugId>
            <configId>imatinib.gotta2012</configId>
            <output>
                <format>html</format>
                <language>fr</language>
            </output>
            <adjustmentDate>2018-07-11T08:00:00</adjustmentDate>
            <options>
                <loadingOption>loadingDoseAllowed</loadingOption>
                <restPeriodOption>restPeriodAllowed</restPeriodOption>
                <targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
                <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
            </options>
        </xpertRequest>
    </requests>
</query>)";

    std::string queryStringWarning = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<query version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

    <date>2022-06-24T13:45:30</date> <!-- Date that defines the computation time -->

     <drugTreatment>
        <!-- All the information regarding the patient -->
        <patient>
            <name>Bruno</name>
            <address>Ciao</address>
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
                    <value>120</value>
                    <unit>kg</unit>
                        <dataType>double</dataType>
                    <nature>discrete</nature>
                </covariate>
                <covariate>
                    <covariateId>sex</covariateId>
                    <date>2017-07-06T08:00:00</date>
                    <value>0</value>
                    <unit></unit>
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
                                            <absorptionModel>extravascular</absorptionModel>
                                        </formulationAndRoute>
                                    </lastingDosage>
                                </dosageLoop>
                            </dosage>
                        </dosageTimeRange>
                    </dosageHistory>
                </treatment>
                <samples>
                    <sample>
                        <sampleId>123456</sampleId>
                        <sampleDate>2018-07-07T06:00:00</sampleDate>
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
                                <value>0.6</value>
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
            </drug>
        </drugs>
    </drugTreatment>

    <requests>
        <!-- List of the requests we want the TuberXpert to take care of -->
        <xpertRequest>
            <drugId>imatinib</drugId>
            <configId>imatinib.gotta2012</configId>
            <output>
                <format>html</format>
                <language>fr</language>
            </output>
            <adjustmentDate>2018-07-11T08:00:00</adjustmentDate>
            <options>
                <loadingOption>loadingDoseAllowed</loadingOption>
                <restPeriodOption>restPeriodAllowed</restPeriodOption>
                <targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
                <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
            </options>
        </xpertRequest>
    </requests>
</query>)";

    std::string queryStringMissing = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<query version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

    <date>2022-06-24T13:45:30</date> <!-- Date that defines the computation time -->

     <drugTreatment>
        <!-- All the information regarding the patient -->
        <patient>
            <name>Bruno</name>
            <address>Ciao</address>
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
                    <covariateId>sex</covariateId>
                    <date>2017-07-06T08:00:00</date>
                    <value>0</value>
                    <unit></unit>
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
                                            <absorptionModel>extravascular</absorptionModel>
                                        </formulationAndRoute>
                                    </lastingDosage>
                                </dosageLoop>
                            </dosage>
                        </dosageTimeRange>
                    </dosageHistory>
                </treatment>
                <samples>
                    <sample>
                        <sampleId>123456</sampleId>
                        <sampleDate>2018-07-07T06:00:00</sampleDate>
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
                                <value>0.6</value>
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
            </drug>
        </drugs>
    </drugTreatment>

    <requests>
        <!-- List of the requests we want the TuberXpert to take care of -->
        <xpertRequest>
            <drugId>imatinib</drugId>
            <configId>imatinib.gotta2012</configId>
            <output>
                <format>html</format>
                <language>en</language>
            </output>
            <adjustmentDate>2018-07-11T08:00:00</adjustmentDate>
            <options>
                <loadingOption>loadingDoseAllowed</loadingOption>
                <restPeriodOption>restPeriodAllowed</restPeriodOption>
                <targetExtractionOption>definitionIfNoIndividualTarget</targetExtractionOption>
                <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
            </options>
        </xpertRequest>
    </requests>
</query>)";
};

TEST_F(CovariateCheckerTest, Success)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccess, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    xpertRequestResult.setConfiguration(configurations);

    // Execute
    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    //     // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();


    EXPECT_EQ(results.size(), 4);

    EXPECT_EQ(results[0].getSource()->getId(), "gist");
    EXPECT_EQ(results[0].getValue(), "0.000000");
    EXPECT_EQ(results[0].getUnit().toString(), "-");
    EXPECT_EQ(dataTypeToString(results[0].getDataType()), "bool");
    EXPECT_EQ(results[0].getType() == CovariateType::PATIENT, false);
    EXPECT_EQ(results[0].getWarning(), "Was not given for calculation!");

    EXPECT_EQ(results[1].getSource()->getId(), "bodyweight");
    EXPECT_EQ(results[1].getValue(), "75");
    EXPECT_EQ(results[1].getUnit().toString(), "kg");
    EXPECT_EQ(dataTypeToString(results[1].getDataType()), "double");
    EXPECT_EQ(results[1].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[1].getWarning(), "");

    EXPECT_EQ(results[2].getSource()->getId(), "sex");
    EXPECT_EQ(results[2].getValue(), "0");
    EXPECT_EQ(results[2].getUnit().toString(), "");
    EXPECT_EQ(dataTypeToString(results[2].getDataType()), "double");
    EXPECT_EQ(results[2].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[2].getWarning(), "");

    EXPECT_EQ(results[3].getSource()->getId(), "age");
    EXPECT_EQ(results[3].getValue(), "1990-01-01 00:00:00");
    EXPECT_EQ(results[3].getUnit().toString(), "");
    EXPECT_EQ(dataTypeToString(results[3].getDataType()), "date");
    EXPECT_EQ(results[3].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[3].getWarning(), "");
}

TEST_F(CovariateCheckerTest, Error)
{
    TestUtils::loadTranslationsFile(translationFile);

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringError, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    xpertRequestResult.setConfiguration(configurations);

    // Execute
    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    //     // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();

    EXPECT_EQ(results[1].getSource()->getId(), "bodyweight");
    EXPECT_EQ(results[1].getValue(), "35");
    EXPECT_EQ(results[1].getUnit().toString(), "kg");
    EXPECT_EQ(dataTypeToString(results[1].getDataType()), "double");
    EXPECT_EQ(results[1].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[1].getWarning(), "Error: Value should be between 40.00 (exclude) and 130.00 (exclude) ");
}

TEST_F(CovariateCheckerTest, Warning)
{
    TestUtils::loadTranslationsFile(translationFile);

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);


    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringWarning, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    xpertRequestResult.setConfiguration(configurations);

    // Execute
    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    //     // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();

    EXPECT_EQ(results[1].getSource()->getId(), "bodyweight");
    EXPECT_EQ(results[1].getValue(), "120");
    EXPECT_EQ(results[1].getUnit().toString(), "kg");
    EXPECT_EQ(dataTypeToString(results[1].getDataType()), "double");
    EXPECT_EQ(results[1].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[1].getWarning(), "Warning: Value should be between 44.00 (include) and 110.00 (include) ");
}

TEST_F(CovariateCheckerTest, Missing)
{
    TestUtils::loadTranslationsFile(translationFile);

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringMissing, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    xpertRequestResult.setConfiguration(configurations);

    // Execute
    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    //     // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();

    EXPECT_EQ(results[3].getSource()->getId(), "bodyweight");
    EXPECT_EQ(results[3].getValue(), "70.000000");
    EXPECT_EQ(results[3].getUnit().toString(), "kg");
    EXPECT_EQ(dataTypeToString(results[3].getDataType()), "double");
    EXPECT_EQ(results[3].getType() == CovariateType::PATIENT, false);
    EXPECT_EQ(results[1].getWarning(), "Was not given for calculation!");
}


/// Covers CovariateChecker::perform(): early return when treatment is nullptr.
TEST_F(CovariateCheckerTest, NullTreatment)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Drug treatment is for "otherdrug" but request is for "imatinib",
    // so the treatment for imatinib is nullptr.
    std::string queryStr = queryStringSuccess;
    size_t pos = queryStr.find("<drugId>imatinib</drugId>");
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, 25, "<drugId>otherdrug</drugId>");

    vector<std::string> models{TestUtils::originalImatinibModelString};

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No treatment set.");
}

/// Covers CovariateChecker::perform(): early return when drug model is not found in repository.
TEST_F(CovariateCheckerTest, NoDrugModelFound)
{
    TestUtils::loadTranslationsFile(translationFile);

    vector<std::string> models{TestUtils::originalImatinibModelString};

    // Config references a drug model that does not exist in the repository
    std::string badModelConfig = xmlString;
    size_t pos = badModelConfig.find("ch.tucuxi.imatinib.gotta2012");
    ASSERT_NE(pos, std::string::npos);
    badModelConfig.replace(pos, 28, "ch.tucuxi.nonexistent.model");

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(badModelConfig, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccess, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    EXPECT_NE(xpertRequestResult.getErrorMessage().find("No drug model found for id:"), std::string::npos);
}

/// Covers CovariateChecker::perform(): warning logged when patient covariate is not in drug model.
TEST_F(CovariateCheckerTest, UnknownCovariate)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Add an extra covariate "height" that is not in the drug model
    std::string queryStr = queryStringSuccess;
    std::string extraCov =
            R"(<covariate>
                    <covariateId>height</covariateId>
                    <date>2017-07-06T08:00:00</date>
                    <value>180</value>
                    <unit>cm</unit>
                        <dataType>double</dataType>
                    <nature>discrete</nature>
                </covariate>
            </covariates>)";
    size_t pos = queryStr.find("</covariates>");
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, 13, extraCov);

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    // Line 65 logs a warning for "height" (not in drug model) but continues
    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
}

/// Covers CovariateChecker::perform(): error when age covariate is specified directly in TQF.
TEST_F(CovariateCheckerTest, AgeInTqf)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Replace "birthdate" covariate with "age" covariate in patient data.
    // The drug model has an "age" covariate with AgeInYears type, so
    // lines 96-97 detect that age was supplied directly instead of birthdate.
    std::string queryStr = queryStringSuccess;

    size_t pos = queryStr.find("<covariateId>birthdate</covariateId>");
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, 35, "<covariateId>age</covariateId>");

    pos = queryStr.find("<value>1990-01-01T00:00:00</value>");
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, 34, "<value>30</value>");

    pos = queryStr.find("<dataType>date</dataType>");
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, 25, "<dataType>double</dataType>");

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "Age specified in the TQF --- must use birth date instead");
}

/// Covers CovariateChecker::getWarningMessage(): IsEmpty evaluation result for covariate with empty value.
TEST_F(CovariateCheckerTest, EmptyCovariate)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Set bodyweight value to empty
    std::string queryStr = queryStringSuccess;
    size_t bwPos = queryStr.find("<covariateId>bodyweight</covariateId>");
    ASSERT_NE(bwPos, std::string::npos);
    size_t valPos = queryStr.find("<value>", bwPos);
    size_t valEnd = queryStr.find("</value>", valPos);
    queryStr.replace(valPos, valEnd + 8 - valPos, "<value></value>");

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();

    // Find the bodyweight result (empty value triggers IsEmpty path)
    bool foundEmptyWarning = false;
    for (const auto& r : results) {
        if (r.getSource()->getId() == "bodyweight") {
            // getWarningMessage returns translate("error") + ": " + translate("empty_covar_value")
            EXPECT_NE(r.getWarning().find("Error:"), std::string::npos);
            foundEmptyWarning = true;
            break;
        }
    }
    EXPECT_TRUE(foundEmptyWarning);
}

/// Covers CovariateChecker::getWarningMessage(): default/CovariateNotExist case.
/// The "gist" covariate is in the drug model but not in the config XML,
/// so configuration.validateBoolValue returns CovariateNotExist.
TEST_F(CovariateCheckerTest, CovariateNotExistDefault)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Add a "gist" covariate (in drug model, Bool type) that is NOT in config
    std::string queryStr = queryStringSuccess;
    std::string gistCov =
            R"(<covariate>
                    <covariateId>gist</covariateId>
                    <date>2017-07-06T08:00:00</date>
                    <value>1</value>
                    <unit></unit>
                        <dataType>bool</dataType>
                    <nature>discrete</nature>
                </covariate>
            </covariates>)";
    size_t pos = queryStr.find("</covariates>");
    ASSERT_NE(pos, std::string::npos);
    queryStr.replace(pos, 13, gistCov);

    vector<std::string> models{TestUtils::originalImatinibModelString};

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();

    // Find the gist result — should hit the default branch
    bool foundDefault = false;
    for (const auto& r : results) {
        if (r.getSource()->getId() == "gist") {
            EXPECT_EQ(r.getWarning(), "Unknown validation result.");
            foundDefault = true;
            break;
        }
    }
    EXPECT_TRUE(foundDefault);
}

/// Covers CovariateChecker::perform() line 156: birthdate with non-Date dataType
/// triggers std::invalid_argument when useToCompute is true and covariateId is "age".
TEST_F(CovariateCheckerTest, BirthdateNonDateDataType)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Change birthdate's dataType from "date" to "double" and value accordingly
    std::string queryStr = queryStringSuccess;
    size_t pos = queryStr.find("<covariateId>birthdate</covariateId>");
    ASSERT_NE(pos, std::string::npos);
    size_t valPos = queryStr.find("<value>", pos);
    size_t valEnd = queryStr.find("</value>", valPos);
    queryStr.replace(valPos, valEnd + 8 - valPos, "<value>30</value>");
    size_t dtPos = queryStr.find("<dataType>date</dataType>", pos);
    ASSERT_NE(dtPos, std::string::npos);
    queryStr.replace(dtPos, 25, "<dataType>double</dataType>");

    vector<std::string> models{TestUtils::originalImatinibModelString};
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    EXPECT_THROW(TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult), std::invalid_argument);
}

/// Covers CovariateChecker::perform() lines 206-208: invalid covariate value
/// triggers std::invalid_argument via stringToValue, caught and rethrown.
TEST_F(CovariateCheckerTest, InvalidCovariateValue)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Set bodyweight to a non-numeric value
    std::string queryStr = queryStringSuccess;
    size_t bwPos = queryStr.find("<covariateId>bodyweight</covariateId>");
    ASSERT_NE(bwPos, std::string::npos);
    size_t valPos = queryStr.find("<value>", bwPos);
    size_t valEnd = queryStr.find("</value>", valPos);
    queryStr.replace(valPos, valEnd + 8 - valPos, "<value>abc</value>");

    vector<std::string> models{TestUtils::originalImatinibModelString};
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlString, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    EXPECT_THROW(TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult), std::invalid_argument);
}


/// Covers CovariateChecker::perform(): birthdate provided
/// but drug model has no age covariate, so useToCompute is false. Creates an owned
/// CovariateDefinition with AgeInYears type and result with WarningLevel::NORMAL.
TEST_F(CovariateCheckerTest, BirthdateWithoutAgeInModel)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Remove the age covariate from the drug model
    std::string modelStr = TestUtils::imatinibModelStringNoAge;

    // Remove the age covariate from the config XML as well
    std::string xmlNoAge = xmlString;
    size_t ageCovPos = xmlNoAge.find("<covariateId>age</covariateId>");
    ASSERT_NE(ageCovPos, std::string::npos);
    size_t cStart = xmlNoAge.rfind("<covariate>", ageCovPos);
    ASSERT_NE(cStart, std::string::npos);
    size_t cEnd = xmlNoAge.find("</covariate>", ageCovPos);
    ASSERT_NE(cEnd, std::string::npos);
    cEnd += std::string("</covariate>").size();
    xmlNoAge.erase(cStart, cEnd - cStart);

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlNoAge, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    vector<std::string> models{modelStr};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccess, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult);

    EXPECT_TRUE(xpertRequestResult.shouldContinueProcessing());

    // The age result should exist with WarningLevel::NORMAL and an empty warning
    const auto& results = xpertRequestResult.getCovariateValidationResults();
    bool foundAge = false;
    for (const auto& r : results) {
        if (r.getSource()->getId() == "age") {
            EXPECT_EQ(r.getWarning(), "");
            EXPECT_EQ(r.getWarningLevel(), WarningLevel::NORMAL);
            foundAge = true;
            break;
        }
    }
    EXPECT_TRUE(foundAge);
}

/// Covers CovariateChecker::perform(): birthdate with non-Date dataType
/// when drug model has no age covariate (useToCompute=false) triggers std::invalid_argument.
TEST_F(CovariateCheckerTest, BirthdateWithoutAgeInModelNonDateType)
{
    TestUtils::loadTranslationsFile(translationFile);

    // Remove the age covariate from the drug model
    std::string modelStr = TestUtils::imatinibModelStringNoAge;

    // Remove the age covariate from the config XML as well
    std::string xmlNoAge = xmlString;
    size_t ageCovPos = xmlNoAge.find("<covariateId>age</covariateId>");
    ASSERT_NE(ageCovPos, std::string::npos);
    size_t cStart = xmlNoAge.rfind("<covariate>", ageCovPos);
    ASSERT_NE(cStart, std::string::npos);
    size_t cEnd = xmlNoAge.find("</covariate>", ageCovPos);
    ASSERT_NE(cEnd, std::string::npos);
    cEnd += std::string("</covariate>").size();
    xmlNoAge.erase(cStart, cEnd - cStart);

    // Change birthdate to double type in query
    std::string queryStr = queryStringSuccess;
    size_t pos = queryStr.find("<covariateId>birthdate</covariateId>");
    ASSERT_NE(pos, std::string::npos);
    size_t valPos = queryStr.find("<value>", pos);
    size_t valEnd = queryStr.find("</value>", valPos);
    queryStr.replace(valPos, valEnd + 8 - valPos, "<value>30</value>");
    size_t dtPos = queryStr.find("<dataType>date</dataType>", pos);
    ASSERT_NE(dtPos, std::string::npos);
    queryStr.replace(dtPos, 25, "<dataType>double</dataType>");

    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();
    ConfigImport importer;
    ComputationLimits computationLimits;
    auto status = importer.importFromString(xmlNoAge, *configurations, computationLimits);
    ASSERT_EQ(status, Tucuxi::Common::IImport::Status::Ok);

    vector<std::string> models{modelStr};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStr, models, xpertQueryResult);
    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];
    xpertRequestResult.setConfiguration(configurations);

    EXPECT_THROW(TestUtils::flowStepProvider.getCovariateChecker()->perform(xpertRequestResult), std::invalid_argument);
}

} // namespace Xpert
} // namespace Tucuxi
