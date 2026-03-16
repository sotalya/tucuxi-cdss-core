// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/result/xpertrequestresult.h"
#include "tucucdss-core/utils/xpertutils.h"

/// \brief Gtest of covariatevalidationresult and modelmelector.
///        Tests for CovariateValidatorAndModelSelector from the GeneralXpertFlowStepProvider.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

// getCovariateValidatorAndModelSelector fixture
class CovariateValidatorAndModelSelectorTest : public testing::Test
{
public:
    //  model strings
    std::string model0String = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>)"
                               R"(
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>
                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib</drugId>
                                        <drugModelId>ch.tucuxi.imatinib.gotta2012_1</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>)"
                               R"(
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight1</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight1</name>
                                                    <name lang='fr'>Poids total1</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight1</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight1 >= 44) && (bodyweight1 <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>bodyweight2</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight2</name>
                                                    <name lang='fr'>Poids total</name>
                                                </covariateName>)"
                               R"(
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight2</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight2 >= 44) && (bodyweight2 <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight2</name>
                                                    <name lang='fr'>Poids total2</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>)"
                               R"(
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>

                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>)"
                               R"(
                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>

                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>)"
                               R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                    <name lang='fr'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>

                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>)"
                               R"(
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>

                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>)"
                               R"(
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>)"
                               R"(
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>

                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>)"
                               R"(
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>)"
                               R"(
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>)"
                               R"(
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>)"
                               R"(
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>

                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>)"
                               R"(
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";

    std::string model1String = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>)"
                               R"(
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>

                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.imatinib.gotta2012_2</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>)"
                               R"(
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight3</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight3</name>
                                                    <name lang='fr'>Poids total3</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight3</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight3 >= 44) && (bodyweight3 <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>)"
                               R"(
                                            <covariate>
                                                <covariateId>bodyweight4</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight4</name>
                                                    <name lang='fr'>Poids total4</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [440,1100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight4</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight4 >= 440) && (bodyweight4 <= 1100));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>)"
                               R"(
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight</name>
                                                    <name lang='fr'>Poids total</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>

                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>

                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>)"
                               R"(
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>

                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>)"
                               R"(
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>

                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                    <name lang='fr'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>)"
                               R"(
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>

                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>)"
                               R"(
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>

                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>)"
                               R"(
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>

                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>)"
                               R"(
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>)"
                               R"(
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>

                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>)"
                               R"(
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>

                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>)"

                               R"(
                                                        </dosages>
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>)"
                               R"(
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>

                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";

    std::string model2String = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>)"
                               R"(
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>

                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.imatinib.gotta2012_3</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight5</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight5</name>
                                                    <name lang='fr'>Poids total5</name>
                                                </covariateName>)"
                               R"(
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [440,1100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight5</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight5 >= 440) && (bodyweight5 <= 1100));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>bodyweight6</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight6</name>
                                                    <name lang='fr'>Poids total6</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>)"
                               R"(
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [440,1100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight6</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight6 >= 440) && (bodyweight6 <= 1100));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight</name>
                                                    <name lang='fr'>Poids total</name>
                                                </covariateName>
                                                <description>

                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>

                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>)"
                               R"(
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>

                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>)"
                               R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>

                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>)"
                               R"(
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                    <name lang='fr'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>

                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>)"
                               R"(
                                                <comments/>
                                            </covariate>
                                        </covariates>
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>)"
                               R"(
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>)"
                               R"(
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>

                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>)"
                               R"(
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>

                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>)"
                               R"(
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>

                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>)"
                               R"(
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>)"
                               R"(
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>)"
                               R"(
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>

                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";



    std::string modelString0 = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>)"
                               R"(
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>
                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib0</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.imatinib0.gotta2012</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>)"
                               R"(
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='de'>Total Body Weight</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                        <text lang='fr'>Le poids corporel doit se situer dans l'intervalle [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>)"
                               R"(
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>)"
                               R"(
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                    <name lang='fr'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>)"
                               R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>)"
                               R"(
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>)"
                               R"(
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>)"
                               R"(
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>)"
                               R"(
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>)"
                               R"(
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";
    std::string modelString1 = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>)"
                               R"(
                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib1</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.imatinib1.gotta2012</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight</name>
                                                    <name lang='fr'>Total Body Weight</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='de'>Total body weight of patient, in kilogramms</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>)"
                               R"(
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                        <text lang='fr'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>)"
                               R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>)"
                               R"(
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                    <name lang='fr'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>)"
                               R"(
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>)"
                               R"(
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>)"
                               R"(
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>)"
                               R"(
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>)"
                               R"(
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>)"
                               R"(
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>)"
                               R"(
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";
    std::string modelString2 = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                          </drug>
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>)"
                               R"(
                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib2</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.imatinib2.gotta2012</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight</name>
                                                    <name lang='fr'>Total Body Weight</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='fr'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>)"
                               R"(
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='de'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>)"
                               R"(
                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>)"
                               R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                    <name lang='fr'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>)"
                               R"(
                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>)"
                               R"(
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>)"
                               R"(
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>)"
                               R"(
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>)"
                               R"(
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>)"
                               R"(
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>)"
                               R"(
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";





    std::string noGistImatinibModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
                                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                                    <history>
                                                        <revisions>
                                                            <revision>
                                                                <revisionAction>creation</revisionAction>
                                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                <institution>HEIG-VD // REDS</institution>
                                                                <email>yann.thoma@heig-vd.ch</email>
                                                                <date>2018-10-30</date>
                                                                <comments>
                                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                                </comment>
                                                                </comments>
                                                            </revision>
                                                            <revision>
                                                                <revisionAction>modification</revisionAction>
                                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                <institution>HEIG-VD // REDS</institution>
                                                                <email>yann.thoma@heig-vd.ch</email>
                                                                <date>2018-11-07</date>
                                                                <comments>
                                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                                </comment>
                                                                </comments>
                                                            </revision>
                                                        </revisions>
                                                    </history>
                                                    <head>
                                                        <drug>
                                                            <atcs>
                                                                <atc>L01XE01</atc>
                                                            </atcs>
                                                            <activeSubstances>
                                                                <activeSubstance>imatinib</activeSubstance>
                                                            </activeSubstances>
                                                            <drugName>
                                                                <name lang='en'>Imatinib</name>
                                                                <name lang='fr'>Imatinib</name>
                                                            </drugName>
                                                            <drugDescription>
                                                                <desc lang='en'>TODO : Add a description here</desc>
                                                            </drugDescription>
                                                            <tdmStrategy>
                                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                                            </tdmStrategy>
                                                        </drug>)"
                                            R"(
                                                        <study>
                                                            <studyName>
                                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                                            </studyName>
                                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                                            <description>
                                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                                            </description>
                                                            <references>
                                                                <reference type='bibtex'>
                                                                    <![CDATA[@article{Gotta2012,
                                                                    author={Gotta, Verena
                                                                    and Widmer, Nicolas
                                                                    and Montemurro, Michael
                                                                    and Leyvraz, Serge
                                                                    and Haouala, Amina
                                                                    and Decosterd, Laurent A.
                                                                    and Csajka, Chantal
                                                                    and Buclin, Thierry},
                                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                                    journal={Clinical Pharmacokinetics},
                                                                    year={2012},
                                                                    month={Mar},
                                                                    day={01},
                                                                    volume={51},
                                                                    number={3},
                                                                    pages={187--201},
                                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                                    issn={1179-1926},
                                                                    doi={10.2165/11596990-000000000-00000},
                                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                                    }
                                                ]]>
                                                                </reference>
                                                            </references>
                                                        </study>
                                                        <comments/>
                                                    </head>
                                                    <drugModel>
                                                        <drugId>imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                        <drugModelId>ch.tucuxi.imatinib.gotta2012</drugModelId>
                                                        <domain>
                                                            <description>
                                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                            </description>
                                                            <constraints>
                                                            </constraints>
                                                        </domain>)"
                                            R"(
                                                        <covariates>
                                                            <covariate>
                                                                <covariateId>sex</covariateId>
                                                                <covariateName>
                                                                    <name lang='en'>Sex</name>
                                                                    <name lang='fr'>Sexe</name>
                                                                </covariateName>
                                                                <description>
                                                                    <desc lang='en'>Sex of the patient</desc>
                                                                    <desc lang='fr'>Sexe du patient</desc>
                                                                </description>
                                                                <unit>-</unit>
                                                                <covariateType>sex</covariateType>
                                                                <dataType>double</dataType>
                                                                <interpolationType>direct</interpolationType>
                                                                <refreshPeriod>
                                                                  <unit>y</unit>
                                                                  <value>1</value>
                                                                </refreshPeriod>
                                                                <covariateValue>
                                                                    <standardValue>0.5</standardValue>
                                                                </covariateValue>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </covariate>
                                                        </covariates>)"
                                            R"(
                                                        <activeMoieties>
                                                            <activeMoiety>
                                                                <activeMoietyId>imatinib</activeMoietyId>
                                                                <activeMoietyName>
                                                                    <name lang='en'>Imatinib</name>
                                                                </activeMoietyName>
                                                                <unit>ug/l</unit>
                                                                <analyteIdList>
                                                                    <analyteId>imatinib</analyteId>
                                                                </analyteIdList>
                                                                <analytesToMoietyFormula>
                                                                    <hardFormula>direct</hardFormula>
                                                                    <comments/>
                                                                </analytesToMoietyFormula>
                                                                <targets>
                                                                    <target>
                                                                        <targetType>residual</targetType>
                                                                        <targetValues>
                                                                            <unit>ug/l</unit>
                                                                            <min>
                                                                                <standardValue>750</standardValue>
                                                                            </min>
                                                                            <max>
                                                                                <standardValue>1500</standardValue>
                                                                            </max>
                                                                            <best>
                                                                                <standardValue>1000</standardValue>
                                                                            </best>
                                                                            <toxicityAlarm>
                                                                                <standardValue>10000.0</standardValue>
                                                                            </toxicityAlarm>
                                                                            <inefficacyAlarm>
                                                                                <standardValue>0.0</standardValue>
                                                                            </inefficacyAlarm>
                                                                        </targetValues>
                                                                        <comments>
                                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                                        </comments>
                                                                    </target>
                                                                </targets>
                                                            </activeMoiety>
                                                        </activeMoieties>
                                                        <analyteGroups>
                                                            <analyteGroup>
                                                                <groupId>imatinib</groupId>
                                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                                <analytes>
                                                                    <analyte>
                                                                        <analyteId>imatinib</analyteId>
                                                                        <unit>ug/l</unit>)"
                                            R"(
                                                                        <molarMass>
                                                                            <value>493.603</value>
                                                                            <unit>g/mol</unit>
                                                                        </molarMass>
                                                                        <description>
                                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                                        </description>
                                                                        <errorModel>
                                                                            <errorModelType>proportional</errorModelType>
                                                                            <sigmas>
                                                                                <sigma>
                                                                                    <standardValue>0.3138</standardValue>
                                                                                </sigma>
                                                                            </sigmas>
                                                                            <comments/>
                                                                        </errorModel>
                                                                        <comments/>
                                                                    </analyte>
                                                                </analytes>
                                                                <dispositionParameters>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>CL</parameterId>
                                                                            <unit>l/h</unit>
                                                                            <parameterValue>
                                                                                <standardValue>14.3</standardValue>
                                                                                <aprioriComputation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>CL_population</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                            <input>
                                                                                                <id>sex</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            theta1 = CL_population;
                                                                                            theta4 = 5.42;
                                                                                            theta5 = 1.49;
                                                                                            theta6 = -5.81;
                                                                                            theta7 = -0.806;

                                                                                            MEANBW = 70;
                                                                                            FBW = (70 - MEANBW) / MEANBW;

                                                                                            MEANAG = 50;
                                                                                            FAGE = (50 - MEANAG) / MEANAG;

                                                                                            if (true)
                                                                                              PATH = 1;
                                                                                            else
                                                                                              PATH = 0;

                                                                                            MALE = sex;

                                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                                            return TVCL;
                                                                                                                     ]]>
                                                                                        </code>)"
                                            R"(
                                                                                    </softFormula>
                                                                                    <comments/>
                                                                                </aprioriComputation>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>exponential</bsvType>
                                                                                <stdDevs>
                                                                                    <stdDev>0.356</stdDev>
                                                                                </stdDevs>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>CL</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return CL < 300.0 and CL > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>V</parameterId>
                                                                                <unit>l</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>347</standardValue>
                                                                                    <aprioriComputation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>V_population</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                                <input>
                                                                                                    <id>sex</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>)"
                                            R"(
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                theta2 = V_population;
                                                                                                theta8 = 46.2;
                                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                                return tvv;
                                                                                                                                            ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </aprioriComputation>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>exponential</bsvType>
                                                                                    <stdDevs>
                                                                                        <stdDev>0.629</stdDev>
                                                                                    </stdDevs>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>V shall be positive.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>V</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return V < 300.0 and V > 0.0;
                                                                                                            ]]>
                                                                                                </code>
                                                                                            </softFormula>
                                                                                            <comments/>
                                                                                        </operation>
                                                                                        <comments/>
                                                                                    </validation>
                                                                                    <comments/>
                                                                                </parameter>
                                                                            </parameters>
                                                                            <correlations>
                                                                                <correlation>
                                                                                    <param1>CL</param1>
                                                                                    <param2>V</param2>
                                                                                    <value>0.798</value>
                                                                                    <comments>
                                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                                    </comments>
                                                                                </correlation>)"
                                            R"(
                                                                            </correlations>
                                                                        </dispositionParameters>
                                                                    </analyteGroup>
                                                                </analyteGroups>
                                                                <formulationAndRoutes default='id0'>
                                                                    <formulationAndRoute>
                                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                                        <formulation>parenteralSolution</formulation>
                                                                        <administrationName>champ libre</administrationName>
                                                                        <administrationRoute>oral</administrationRoute>
                                                                        <absorptionModel>extra</absorptionModel>
                                                                        <dosages>
                                                                            <analyteConversions>
                                                                                <analyteConversion>
                                                                                    <analyteId>imatinib</analyteId>
                                                                                    <factor>1</factor>
                                                                                </analyteConversion>
                                                                            </analyteConversions>
                                                                            <availableDoses>
                                                                                <unit>mg</unit>
                                                                                <default>
                                                                                    <standardValue>400</standardValue>
                                                                                </default>
                                                                                <rangeValues>
                                                                                    <from>
                                                                                        <standardValue>100</standardValue>
                                                                                    </from>
                                                                                    <to>
                                                                                        <standardValue>400</standardValue>
                                                                                    </to>
                                                                                    <step>
                                                                                        <standardValue>100</standardValue>
                                                                                    </step>
                                                                                </rangeValues>
                                                                                <fixedValues>
                                                                                    <value>600</value>
                                                                                    <value>800</value>
                                                                                </fixedValues>
                                                                            </availableDoses>
                                                                            <availableIntervals>
                                                                                <unit>h</unit>
                                                                                <default>
                                                                                    <standardValue>24</standardValue>
                                                                                </default>
                                                                                <fixedValues>
                                                                                    <value>12</value>
                                                                                    <value>24</value>
                                                                                </fixedValues>
                                                                            </availableIntervals>
                                                                            <comments/>
                                                                        </dosages>)"
                                            R"(
                                                                        <absorptionParameters>
                                                                            <parameterSetAnalyteGroup>
                                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                                <absorptionModel>extra</absorptionModel>
                                                                                <parameterSet>
                                                                                    <parameters>
                                                                                        <parameter>
                                                                                            <parameterId>F</parameterId>
                                                                                            <unit>%</unit>
                                                                                            <parameterValue>
                                                                                                <standardValue>1</standardValue>
                                                                                            </parameterValue>
                                                                                            <bsv>
                                                                                                <bsvType>none</bsvType>
                                                                                            </bsv>
                                                                                            <validation>
                                                                                                <errorMessage>
                                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                                </errorMessage>
                                                                                                <operation>
                                                                                                    <softFormula>
                                                                                                        <inputs>
                                                                                                            <input>
                                                                                                                <id>F</id>
                                                                                                                <type>double</type>
                                                                                                            </input>
                                                                                                        </inputs>
                                                                                                        <code>
                                                                                                            <![CDATA[
                                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                                        ]]>
                                                                                                            </code>
                                                                                                        </softFormula>
                                                                                                        <comments/>
                                                                                                    </operation>
                                                                                                    <comments/>
                                                                                                </validation>
                                                                                                <comments/>
                                                                                            </parameter>
                                                                                            <parameter>
                                                                                                <parameterId>Ka</parameterId>
                                                                                                <unit>h-1</unit>
                                                                                                <parameterValue>
                                                                                                    <standardValue>0.609</standardValue>
                                                                                                </parameterValue>
                                                                                                <bsv>
                                                                                                    <bsvType>none</bsvType>
                                                                                                </bsv>)"
                                            R"(
                                                                                                <validation>
                                                                                                    <errorMessage>
                                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                                    </errorMessage>
                                                                                                    <operation>
                                                                                                        <softFormula>
                                                                                                            <inputs>
                                                                                                                <input>
                                                                                                                    <id>Ka</id>
                                                                                                                    <type>double</type>
                                                                                                                </input>
                                                                                                            </inputs>
                                                                                                            <code>
                                                                                                                <![CDATA[
                                                                                                                return true;
                                                                                                                ]]>
                                                                                                            </code>
                                                                                                        </softFormula>
                                                                                                        <comments/>
                                                                                                    </operation>
                                                                                                    <comments/>
                                                                                                </validation>
                                                                                                <comments/>
                                                                                            </parameter>
                                                                                        </parameters>
                                                                                        <correlations/>
                                                                                    </parameterSet>
                                                                                </parameterSetAnalyteGroup>
                                                                            </absorptionParameters>
                                                                        </formulationAndRoute>
                                                                    </formulationAndRoutes>
                                                                    <timeConsiderations>
                                                                        <halfLife>
                                                                            <unit>h</unit>
                                                                            <duration>
                                                                                <standardValue>12</standardValue>
                                                                            </duration>
                                                                            <multiplier>20</multiplier>
                                                                            <comments>
                                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                                            </comments>
                                                                        </halfLife>
                                                                        <outdatedMeasure>
                                                                            <unit>d</unit>
                                                                            <duration>
                                                                                <standardValue>100</standardValue>
                                                                            </duration>
                                                                            <comments>
                                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                                            </comments>
                                                                        </outdatedMeasure>
                                                                    </timeConsiderations>
                                                                    <comments/>
                                                                </drugModel>
                                                            </model>)";


    std::string modifiedImatinibModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
                                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                                    <history>
                                                        <revisions>
                                                            <revision>
                                                                <revisionAction>creation</revisionAction>
                                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                <institution>HEIG-VD // REDS</institution>
                                                                <email>yann.thoma@heig-vd.ch</email>
                                                                <date>2018-10-30</date>
                                                                <comments>
                                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                                </comment>
                                                                </comments>
                                                            </revision>
                                                            <revision>
                                                                <revisionAction>modification</revisionAction>
                                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                <institution>HEIG-VD // REDS</institution>
                                                                <email>yann.thoma@heig-vd.ch</email>
                                                                <date>2018-11-07</date>
                                                                <comments>
                                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                                </comment>
                                                                </comments>
                                                            </revision>
                                                        </revisions>
                                                    </history>
                                                    <head>
                                                        <drug>
                                                            <atcs>
                                                                <atc>L01XE01</atc>
                                                            </atcs>
                                                            <activeSubstances>
                                                                <activeSubstance>imatinib</activeSubstance>
                                                            </activeSubstances>
                                                            <drugName>
                                                                <name lang='en'>Imatinib</name>
                                                                <name lang='fr'>Imatinib</name>
                                                            </drugName>
                                                            <drugDescription>
                                                                <desc lang='en'>TODO : Add a description here</desc>
                                                            </drugDescription>
                                                            <tdmStrategy>
                                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                                            </tdmStrategy>
                                                        </drug>
                                                        <study>
                                                            <studyName>
                                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                                Bayesian and Alternative Methods to Predict Trough Levels</name>)"
                                              R"(
                                                            </studyName>
                                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                                            <description>
                                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                                            </description>
                                                            <references>
                                                                <reference type='bibtex'>
                                                                    <![CDATA[@article{Gotta2012,
                                                                    author={Gotta, Verena
                                                                    and Widmer, Nicolas
                                                                    and Montemurro, Michael
                                                                    and Leyvraz, Serge
                                                                    and Haouala, Amina
                                                                    and Decosterd, Laurent A.
                                                                    and Csajka, Chantal
                                                                    and Buclin, Thierry},
                                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                                    journal={Clinical Pharmacokinetics},
                                                                    year={2012},
                                                                    month={Mar},
                                                                    day={01},
                                                                    volume={51},
                                                                    number={3},
                                                                    pages={187--201},
                                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                                    issn={1179-1926},
                                                                    doi={10.2165/11596990-000000000-00000},
                                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                                    }
                                                ]]>
                                                                </reference>
                                                            </references>
                                                        </study>
                                                        <comments/>
                                                    </head>
                                                    <drugModel>
                                                        <drugId>imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                                        <drugModelId>ch.tucuxi.imatinib.gotta2012_modified</drugModelId>
                                                        <domain>
                                                            <description>
                                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                            </description>
                                                            <constraints>
                                                                <constraint>
                                                                    <constraintType>hard</constraintType>
                                                                    <errorMessage>
                                                                        <text lang='en'>The age shall be in the range [20,88]</text>
                                                                    </errorMessage>)"
                                              R"(
                                                                    <requiredCovariates>
                                                                        <covariateId>age</covariateId>
                                                                    </requiredCovariates>
                                                                    <checkOperation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>age</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return true;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </checkOperation>
                                                                    <comments/>
                                                                </constraint>
                                                                <constraint>
                                                                    <constraintType>soft</constraintType>
                                                                    <errorMessage>
                                                                        <text lang='en'>The weight should be in the range [44,110]</text>
                                                                    </errorMessage>
                                                                    <requiredCovariates>
                                                                        <covariateId>bodyweight</covariateId>
                                                                    </requiredCovariates>
                                                                    <checkOperation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </checkOperation>
                                                                    <comments/>
                                                                </constraint>
                                                            </constraints>
                                                        </domain>)"
                                              R"(
                                                        <covariates>
                                                            <covariate>
                                                                <covariateId>bodyweight</covariateId>
                                                                <covariateName>
                                                                    <name lang='en'>Total Body Weight</name>
                                                                    <name lang='fr'>Poids total</name>
                                                                </covariateName>
                                                                <description>
                                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                                </description>
                                                                <unit>kg</unit>
                                                                <covariateType>standard</covariateType>
                                                                <dataType>double</dataType>
                                                                <interpolationType>linear</interpolationType>
                                                                <refreshPeriod>
                                                                  <unit>d</unit>
                                                                  <value>30</value>
                                                                </refreshPeriod>
                                                                <covariateValue>
                                                                    <standardValue>70</standardValue>
                                                                </covariateValue>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </covariate>
                                                            <covariate>
                                                                <covariateId>gist</covariateId>
                                                                <covariateName>
                                                                    <name lang='en'>GIST</name>
                                                                    <name lang='fr'>GIST</name>
                                                                </covariateName>)"
                                              R"(
                                                                <description>
                                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                                </description>
                                                                <unit>-</unit>
                                                                <covariateType>standard</covariateType>
                                                                <dataType>bool</dataType>
                                                                <interpolationType>direct</interpolationType>
                                                                <refreshPeriod>
                                                                  <unit>y</unit>
                                                                  <value>1</value>
                                                                </refreshPeriod>
                                                                <covariateValue>
                                                                    <standardValue>0</standardValue>
                                                                </covariateValue>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>Always correct.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>gist</id>
                                                                                    <type>bool</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return true;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </covariate>
                                                            <covariate>
                                                                <covariateId>sex</covariateId>
                                                                <covariateName>
                                                                    <name lang='en'>Sex</name>
                                                                    <name lang='fr'>Sexe</name>
                                                                </covariateName>
                                                                <description>
                                                                    <desc lang='en'>Sex of the patient</desc>
                                                                    <desc lang='fr'>Sexe du patient</desc>
                                                                </description>)"
                                              R"(
                                                                <unit>-</unit>
                                                                <covariateType>sex</covariateType>
                                                                <dataType>double</dataType>
                                                                <interpolationType>direct</interpolationType>
                                                                <refreshPeriod>
                                                                  <unit>y</unit>
                                                                  <value>1</value>
                                                                </refreshPeriod>
                                                                <covariateValue>
                                                                    <standardValue>0.5</standardValue>
                                                                </covariateValue>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </covariate>
                                                            <covariate>
                                                                <covariateId>age</covariateId>
                                                                <covariateName>
                                                                    <name lang='en'>Age</name>
                                                                    <name lang='fr'>Age</name>
                                                                </covariateName>
                                                                <description>
                                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                                    <desc lang='fr'>Âge du patient, en années</desc>
                                                                </description>
                                                                <unit>y</unit>
                                                                <covariateType>ageInYears</covariateType>
                                                                <dataType>double</dataType>
                                                                <interpolationType>direct</interpolationType>
                                                                <refreshPeriod>
                                                                  <unit>y</unit>
                                                                  <value>1</value>
                                                                </refreshPeriod>)"
                                              R"(
                                                                <covariateValue>
                                                                    <standardValue>50</standardValue>
                                                                </covariateValue>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>age</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </covariate>
                                                        </covariates>
                                                        <activeMoieties>
                                                            <activeMoiety>
                                                                <activeMoietyId>imatinib</activeMoietyId>
                                                                <activeMoietyName>
                                                                    <name lang='en'>Imatinib</name>
                                                                </activeMoietyName>
                                                                <unit>ug/l</unit>
                                                                <analyteIdList>
                                                                    <analyteId>imatinib</analyteId>
                                                                </analyteIdList>
                                                                <analytesToMoietyFormula>
                                                                    <hardFormula>direct</hardFormula>
                                                                    <comments/>
                                                                </analytesToMoietyFormula>
                                                                <targets>
                                                                    <target>
                                                                        <targetType>residual</targetType>
                                                                        <targetValues>
                                                                            <unit>ug/l</unit>
                                                                            <min>
                                                                                <standardValue>750</standardValue>
                                                                            </min>)"
                                              R"(
                                                                            <max>
                                                                                <standardValue>1500</standardValue>
                                                                            </max>
                                                                            <best>
                                                                                <standardValue>1000</standardValue>
                                                                            </best>
                                                                            <toxicityAlarm>
                                                                                <standardValue>10000.0</standardValue>
                                                                            </toxicityAlarm>
                                                                            <inefficacyAlarm>
                                                                                <standardValue>0.0</standardValue>
                                                                            </inefficacyAlarm>
                                                                        </targetValues>
                                                                        <comments>
                                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                                        </comments>
                                                                    </target>
                                                                </targets>
                                                            </activeMoiety>
                                                        </activeMoieties>
                                                        <analyteGroups>
                                                            <analyteGroup>
                                                                <groupId>imatinib</groupId>
                                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                                <analytes>
                                                                    <analyte>
                                                                        <analyteId>imatinib</analyteId>
                                                                        <unit>ug/l</unit>
                                                                        <molarMass>
                                                                            <value>493.603</value>
                                                                            <unit>g/mol</unit>
                                                                        </molarMass>
                                                                        <description>
                                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                                        </description>
                                                                        <errorModel>
                                                                            <errorModelType>proportional</errorModelType>
                                                                            <sigmas>
                                                                                <sigma>
                                                                                    <standardValue>0.3138</standardValue>
                                                                                </sigma>
                                                                            </sigmas>
                                                                            <comments/>
                                                                        </errorModel>
                                                                        <comments/>
                                                                    </analyte>
                                                                </analytes>)"
                                              R"(
                                                                <dispositionParameters>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>CL</parameterId>
                                                                            <unit>l/h</unit>
                                                                            <parameterValue>
                                                                                <standardValue>14.3</standardValue>
                                                                                <aprioriComputation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>CL_population</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                            <input>
                                                                                                <id>bodyweight</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                            <input>
                                                                                                <id>age</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                            <input>
                                                                                                <id>gist</id>
                                                                                                <type>bool</type>
                                                                                            </input>
                                                                                            <input>
                                                                                                <id>sex</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            theta1 = CL_population;
                                                                                            theta4 = 5.42;
                                                                                            theta5 = 1.49;
                                                                                            theta6 = -5.81;
                                                                                            theta7 = -0.806;

                                                                                            MEANBW = 70;
                                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                                            MEANAG = 50;
                                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                                            if (gist)
                                                                                              PATH = 1;
                                                                                            else
                                                                                              PATH = 0;

                                                                                            MALE = sex;)"
                                              R"(

                                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                                            return TVCL;
                                                                                                                     ]]>
                                                                                        </code>
                                                                                    </softFormula>
                                                                                    <comments/>
                                                                                </aprioriComputation>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>exponential</bsvType>
                                                                                <stdDevs>
                                                                                    <stdDev>0.356</stdDev>
                                                                                </stdDevs>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>CL</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return CL < 300.0 and CL > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>V</parameterId>
                                                                                <unit>l</unit>)"
                                              R"(
                                                                                <parameterValue>
                                                                                    <standardValue>347</standardValue>
                                                                                    <aprioriComputation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>V_population</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                                <input>
                                                                                                    <id>sex</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                theta2 = V_population;
                                                                                                theta8 = 46.2;
                                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                                return tvv;
                                                                                                                                            ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </aprioriComputation>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>exponential</bsvType>
                                                                                    <stdDevs>
                                                                                        <stdDev>0.629</stdDev>
                                                                                    </stdDevs>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>V shall be positive.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>V</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return V < 300.0 and V > 0.0;
                                                                                                            ]]>
                                                                                                </code>
                                                                                            </softFormula>
                                                                                            <comments/>
                                                                                        </operation>)"
                                              R"(
                                                                                        <comments/>
                                                                                    </validation>
                                                                                    <comments/>
                                                                                </parameter>
                                                                            </parameters>
                                                                            <correlations>
                                                                                <correlation>
                                                                                    <param1>CL</param1>
                                                                                    <param2>V</param2>
                                                                                    <value>0.798</value>
                                                                                    <comments>
                                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                                    </comments>
                                                                                </correlation>
                                                                            </correlations>
                                                                        </dispositionParameters>
                                                                    </analyteGroup>
                                                                </analyteGroups>
                                                                <formulationAndRoutes default='id0'>
                                                                    <formulationAndRoute>
                                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                                        <formulation>parenteralSolution</formulation>
                                                                        <administrationName>champ libre</administrationName>
                                                                        <administrationRoute>oral</administrationRoute>
                                                                        <absorptionModel>extra</absorptionModel>
                                                                        <dosages>
                                                                            <analyteConversions>
                                                                                <analyteConversion>
                                                                                    <analyteId>imatinib</analyteId>
                                                                                    <factor>1</factor>
                                                                                </analyteConversion>
                                                                            </analyteConversions>
                                                                            <availableDoses>
                                                                                <unit>mg</unit>
                                                                                <default>
                                                                                    <standardValue>400</standardValue>
                                                                                </default>
                                                                                <rangeValues>
                                                                                    <from>
                                                                                        <standardValue>100</standardValue>
                                                                                    </from>
                                                                                    <to>
                                                                                        <standardValue>400</standardValue>
                                                                                    </to>
                                                                                    <step>
                                                                                        <standardValue>100</standardValue>
                                                                                    </step>
                                                                                </rangeValues>)"
                                              R"(
                                                                                <fixedValues>
                                                                                    <value>600</value>
                                                                                    <value>800</value>
                                                                                </fixedValues>
                                                                            </availableDoses>
                                                                            <availableIntervals>
                                                                                <unit>h</unit>
                                                                                <default>
                                                                                    <standardValue>24</standardValue>
                                                                                </default>
                                                                                <fixedValues>
                                                                                    <value>12</value>
                                                                                    <value>24</value>
                                                                                </fixedValues>
                                                                            </availableIntervals>
                                                                            <comments/>
                                                                        </dosages>
                                                                        <absorptionParameters>
                                                                            <parameterSetAnalyteGroup>
                                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                                <absorptionModel>extra</absorptionModel>
                                                                                <parameterSet>
                                                                                    <parameters>
                                                                                        <parameter>
                                                                                            <parameterId>F</parameterId>
                                                                                            <unit>%</unit>
                                                                                            <parameterValue>
                                                                                                <standardValue>1</standardValue>
                                                                                            </parameterValue>
                                                                                            <bsv>
                                                                                                <bsvType>none</bsvType>
                                                                                            </bsv>
                                                                                            <validation>
                                                                                                <errorMessage>
                                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                                </errorMessage>
                                                                                                <operation>
                                                                                                    <softFormula>
                                                                                                        <inputs>
                                                                                                            <input>
                                                                                                                <id>F</id>
                                                                                                                <type>double</type>
                                                                                                            </input>
                                                                                                        </inputs>
                                                                                                        <code>
                                                                                                            <![CDATA[
                                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                                        ]]>
                                                                                                            </code>
                                                                                                        </softFormula>
                                                                                                        <comments/>
                                                                                                    </operation>
                                                                                                    <comments/>)"
                                              R"(
                                                                                                </validation>
                                                                                                <comments/>
                                                                                            </parameter>
                                                                                            <parameter>
                                                                                                <parameterId>Ka</parameterId>
                                                                                                <unit>h-1</unit>
                                                                                                <parameterValue>
                                                                                                    <standardValue>0.609</standardValue>
                                                                                                </parameterValue>
                                                                                                <bsv>
                                                                                                    <bsvType>none</bsvType>
                                                                                                </bsv>
                                                                                                <validation>
                                                                                                    <errorMessage>
                                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                                    </errorMessage>
                                                                                                    <operation>
                                                                                                        <softFormula>
                                                                                                            <inputs>
                                                                                                                <input>
                                                                                                                    <id>Ka</id>
                                                                                                                    <type>double</type>
                                                                                                                </input>
                                                                                                            </inputs>
                                                                                                            <code>
                                                                                                                <![CDATA[
                                                                                                                return true;
                                                                                                                ]]>
                                                                                                            </code>
                                                                                                        </softFormula>
                                                                                                        <comments/>
                                                                                                    </operation>
                                                                                                    <comments/>
                                                                                                </validation>
                                                                                                <comments/>
                                                                                            </parameter>
                                                                                        </parameters>
                                                                                        <correlations/>
                                                                                    </parameterSet>
                                                                                </parameterSetAnalyteGroup>
                                                                            </absorptionParameters>
                                                                        </formulationAndRoute>
                                                                    </formulationAndRoutes>
                                                                    <timeConsiderations>
                                                                        <halfLife>
                                                                            <unit>h</unit>
                                                                            <duration>
                                                                                <standardValue>12</standardValue>
                                                                            </duration>)"
                                              R"(
                                                                            <multiplier>20</multiplier>
                                                                            <comments>
                                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                                            </comments>
                                                                        </halfLife>
                                                                        <outdatedMeasure>
                                                                            <unit>d</unit>
                                                                            <duration>
                                                                                <standardValue>100</standardValue>
                                                                            </duration>
                                                                            <comments>
                                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                                            </comments>
                                                                        </outdatedMeasure>
                                                                    </timeConsiderations>
                                                                    <comments/>
                                                                </drugModel>
                                                            </model>)";


    std::string noAgeImatinibModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>)"
                                           R"(
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>
                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>imatinib</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.imatinib.gotta2012</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                                <constraint>
                                                    <constraintType>soft</constraintType>
                                                    <errorMessage>
                                                        <text lang='en'>The weight should be in the range [44,110]</text>
                                                    </errorMessage>)"
                                           R"(
                                                    <requiredCovariates>
                                                        <covariateId>bodyweight</covariateId>
                                                    </requiredCovariates>
                                                    <checkOperation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </checkOperation>
                                                    <comments/>
                                                </constraint>
                                            </constraints>
                                        </domain>
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight</name>
                                                    <name lang='fr'>Poids total</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                    <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>)"
                                           R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                    <name lang='fr'>GIST</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                    <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>)"
                                           R"(
                                                            <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                    <name lang='fr'>Sexe</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                    <desc lang='fr'>Sexe du patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>)"
                                           R"(
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>)"
                                           R"(
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>)"
                                           R"(
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (50 - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>)"
                                           R"(
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>)"
                                           R"(
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>)"
                                           R"(
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>)"
                                           R"(
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>)"
                                           R"(
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";


    std::string onlyEnglishImatinibModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
                                <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                    <history>
                                        <revisions>
                                            <revision>
                                                <revisionAction>creation</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-10-30</date>
                                                <comments>
                                                    <comment lang='en'>This file is based on the first version of
                                                                                                                                imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                            <revision>
                                                <revisionAction>modification</revisionAction>
                                                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                <institution>HEIG-VD // REDS</institution>
                                                <email>yann.thoma@heig-vd.ch</email>
                                                <date>2018-11-07</date>
                                                <comments>
                                                    <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                </comment>
                                                </comments>
                                            </revision>
                                        </revisions>
                                    </history>
                                    <head>
                                        <drug>
                                            <atcs>
                                                <atc>L01XE01</atc>
                                            </atcs>
                                            <activeSubstances>
                                                <activeSubstance>imatinib</activeSubstance>
                                            </activeSubstances>
                                            <drugName>
                                                <name lang='en'>Imatinib</name>
                                                <name lang='fr'>Imatinib</name>
                                            </drugName>
                                            <drugDescription>
                                                <desc lang='en'>TODO : Add a description here</desc>
                                            </drugDescription>
                                            <tdmStrategy>
                                                <text lang='en'>TODO : Add a TDM strategy</text>
                                            </tdmStrategy>
                                        </drug>)"
                                                 R"(
                                        <study>
                                            <studyName>
                                                <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                Bayesian and Alternative Methods to Predict Trough Levels</name>
                                            </studyName>
                                            <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                            <description>
                                                <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                            </description>
                                            <references>
                                                <reference type='bibtex'>
                                                    <![CDATA[@article{Gotta2012,
                                                    author={Gotta, Verena
                                                    and Widmer, Nicolas
                                                    and Montemurro, Michael
                                                    and Leyvraz, Serge
                                                    and Haouala, Amina
                                                    and Decosterd, Laurent A.
                                                    and Csajka, Chantal
                                                    and Buclin, Thierry},
                                                    title={Therapeutic Drug Monitoring of Imatinib},
                                                    journal={Clinical Pharmacokinetics},
                                                    year={2012},
                                                    month={Mar},
                                                    day={01},
                                                    volume={51},
                                                    number={3},
                                                    pages={187--201},
                                                    abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                    issn={1179-1926},
                                                    doi={10.2165/11596990-000000000-00000},
                                                    url={https://doi.org/10.2165/11596990-000000000-00000}
                                                    }
                                ]]>
                                                </reference>
                                            </references>
                                        </study>
                                        <comments/>
                                    </head>
                                    <drugModel>
                                        <drugId>en_imatinib</drugId>
                                        <configId>imatinib.gotta2012</configId>
                                        <drugModelId>ch.tucuxi.en_imatinib.gotta2012</drugModelId>
                                        <domain>
                                            <description>
                                                <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                            </description>
                                            <constraints>
                                            </constraints>
                                        </domain>
                                        <covariates>
                                            <covariate>
                                                <covariateId>bodyweight</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Total Body Weight</name>
                                                </covariateName>)"
                                                 R"(
                                                <description>
                                                    <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                </description>
                                                <unit>kg</unit>
                                                <covariateType>standard</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>linear</interpolationType>
                                                <refreshPeriod>
                                                  <unit>d</unit>
                                                  <value>30</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>70</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>bodyweight</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>gist</covariateId>
                                                <covariateName>
                                                    <name lang='en'>GIST</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>standard</covaPATIENTpe>
                                                <dataType>bool</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>)"
                                                 R"(
                                                <covariateValue>
        MODEL                                       <standardValue>0</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>Always correct.</text>
                                                    </errorMessage>
                                                    <operation>
         MODEL                                          <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>gist</id>
                                                                    <type>bool</type>
                                                                </input>
                                                            </inputs>
   PATIENT                                                  <code>
                                                                <![CDATA[return true;
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
        PATIENT                                     <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>sex</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Sex</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Sex of the patient</desc>
                                                </description>
                                                <unit>-</unit>
                                                <covariateType>sex</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>0.5</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                    </errorMessage>)"
                                                 R"(
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>sex</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>
                                                            <code>
                                                                <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                            <covariate>
                                                <covariateId>age</covariateId>
                                                <covariateName>
                                                    <name lang='en'>Age</name>
                                                </covariateName>
                                                <description>
                                                    <desc lang='en'>Age of the patient, in years</desc>
                                                </description>
                                                <unit>y</unit>
                                                <covariateType>ageInYears</covariateType>
                                                <dataType>double</dataType>
                                                <interpolationType>direct</interpolationType>
                                                <refreshPeriod>
                                                  <unit>y</unit>
                                                  <value>1</value>
                                                </refreshPeriod>
                                                <covariateValue>
                                                    <standardValue>50</standardValue>
                                                </covariateValue>
                                                <validation>
                                                    <errorMessage>
                                                        <text lang='en'>The age shall be in the interval [20,88].</text>
                                                    </errorMessage>
                                                    <operation>
                                                        <softFormula>
                                                            <inputs>
                                                                <input>
                                                                    <id>age</id>
                                                                    <type>double</type>
                                                                </input>
                                                            </inputs>)"
                                                 R"(
                                                            <code>
                                                                <![CDATA[return ((age >= 20) && (age <= 88));
                                                                ]]>
                                                            </code>
                                                        </softFormula>
                                                        <comments/>
                                                    </operation>
                                                    <comments/>
                                                </validation>
                                                <comments/>
                                            </covariate>
                                        </covariates>
                                        <activeMoieties>
                                            <activeMoiety>
                                                <activeMoietyId>imatinib</activeMoietyId>
                                                <activeMoietyName>
                                                    <name lang='en'>Imatinib</name>
                                                </activeMoietyName>
                                                <unit>ug/l</unit>
                                                <analyteIdList>
                                                    <analyteId>imatinib</analyteId>
                                                </analyteIdList>
                                                <analytesToMoietyFormula>
                                                    <hardFormula>direct</hardFormula>
                                                    <comments/>
                                                </analytesToMoietyFormula>
                                                <targets>
                                                    <target>
                                                        <targetType>residual</targetType>
                                                        <targetValues>
                                                            <unit>ug/l</unit>
                                                            <min>
                                                                <standardValue>750</standardValue>
                                                            </min>
                                                            <max>
                                                                <standardValue>1500</standardValue>
                                                            </max>
                                                            <best>
                                                                <standardValue>1000</standardValue>
                                                            </best>
                                                            <toxicityAlarm>
                                                                <standardValue>10000.0</standardValue>
                                                            </toxicityAlarm>
                                                            <inefficacyAlarm>
                                                                <standardValue>0.0</standardValue>
                                                            </inefficacyAlarm>
                                                        </targetValues>
                                                        <comments>
                                                            <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                        </comments>
                                                    </target>
                                                </targets>
                                            </activeMoiety>
                                        </activeMoieties>)"
                                                 R"(
                                        <analyteGroups>
                                            <analyteGroup>
                                                <groupId>imatinib</groupId>
                                                <pkModelId>linear.1comp.macro</pkModelId>
                                                <analytes>
                                                    <analyte>
                                                        <analyteId>imatinib</analyteId>
                                                        <unit>ug/l</unit>
                                                        <molarMass>
                                                            <value>493.603</value>
                                                            <unit>g/mol</unit>
                                                        </molarMass>
                                                        <description>
                                                            <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                        </description>
                                                        <errorModel>
                                                            <errorModelType>proportional</errorModelType>
                                                            <sigmas>
                                                                <sigma>
                                                                    <standardValue>0.3138</standardValue>
                                                                </sigma>
                                                            </sigmas>
                                                            <comments/>
                                                        </errorModel>
                                                        <comments/>
                                                    </analyte>
                                                </analytes>
                                                <dispositionParameters>
                                                    <parameters>
                                                        <parameter>
                                                            <parameterId>CL</parameterId>
                                                            <unit>l/h</unit>
                                                            <parameterValue>
                                                                <standardValue>14.3</standardValue>
                                                                <aprioriComputation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL_population</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>bodyweight</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>age</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>gist</id>
                                                                                <type>bool</type>
                                                                            </input>
                                                                            <input>
                                                                                <id>sex</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>)"
                                                 R"(
                                                                        <code>
                                                                            <![CDATA[
                                                                            theta1 = CL_population;
                                                                            theta4 = 5.42;
                                                                            theta5 = 1.49;
                                                                            theta6 = -5.81;
                                                                            theta7 = -0.806;

                                                                            MEANBW = 70;
                                                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                                                            MEANAG = 50;
                                                                            FAGE = (age - MEANAG) / MEANAG;

                                                                            if (gist)
                                                                              PATH = 1;
                                                                            else
                                                                              PATH = 0;

                                                                            MALE = sex;

                                                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                            return TVCL;
                                                                                                     ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </aprioriComputation>
                                                            </parameterValue>
                                                            <bsv>
                                                                <bsvType>exponential</bsvType>
                                                                <stdDevs>
                                                                    <stdDev>0.356</stdDev>
                                                                </stdDevs>
                                                            </bsv>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>CL</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return CL < 300.0 and CL > 0.0;
                                                                                        ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>)"
                                                 R"(
                                                            <parameter>
                                                                <parameterId>V</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>347</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V_population</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[
                                                                                theta2 = V_population;
                                                                                theta8 = 46.2;
                                                                                tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                return tvv;
                                                                                                                            ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>exponential</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.629</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='en'>V shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>)"
                                                 R"(
                                                                            <code>
                                                                                <![CDATA[
                                                                                return V < 300.0 and V > 0.0;
                                                                                            ]]>
                                                                                </code>
                                                                            </softFormula>
                                                                            <comments/>
                                                                        </operation>
                                                                        <comments/>
                                                                    </validation>
                                                                    <comments/>
                                                                </parameter>
                                                            </parameters>
                                                            <correlations>
                                                                <correlation>
                                                                    <param1>CL</param1>
                                                                    <param2>V</param2>
                                                                    <value>0.798</value>
                                                                    <comments>
                                                                        <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                    </comments>
                                                                </correlation>
                                                            </correlations>
                                                        </dispositionParameters>
                                                    </analyteGroup>
                                                </analyteGroups>
                                                <formulationAndRoutes default='id0'>
                                                    <formulationAndRoute>
                                                        <formulationAndRouteId>id0</formulationAndRouteId>
                                                        <formulation>parenteralSolution</formulation>
                                                        <administrationName>champ libre</administrationName>
                                                        <administrationRoute>oral</administrationRoute>
                                                        <absorptionModel>extra</absorptionModel>
                                                        <dosages>
                                                            <analyteConversions>
                                                                <analyteConversion>
                                                                    <analyteId>imatinib</analyteId>
                                                                    <factor>1</factor>
                                                                </analyteConversion>
                                                            </analyteConversions>
                                                            <availableDoses>
                                                                <unit>mg</unit>
                                                                <default>
                                                                    <standardValue>400</standardValue>
                                                                </default>
                                                                <rangeValues>
                                                                    <from>
                                                                        <standardValue>100</standardValue>
                                                                    </from>)"
                                                 R"(
                                                                    <to>
                                                                        <standardValue>400</standardValue>
                                                                    </to>
                                                                    <step>
                                                                        <standardValue>100</standardValue>
                                                                    </step>
                                                                </rangeValues>
                                                                <fixedValues>
                                                                    <value>600</value>
                                                                    <value>800</value>
                                                                </fixedValues>
                                                            </availableDoses>
                                                            <availableIntervals>
                                                                <unit>h</unit>
                                                                <default>
                                                                    <standardValue>24</standardValue>
                                                                </default>
                                                                <fixedValues>
                                                                    <value>12</value>
                                                                    <value>24</value>
                                                                </fixedValues>
                                                            </availableIntervals>
                                                            <comments/>
                                                        </dosages>
                                                        <absorptionParameters>
                                                            <parameterSetAnalyteGroup>
                                                                <analyteGroupId>imatinib</analyteGroupId>
                                                                <absorptionModel>extra</absorptionModel>
                                                                <parameterSet>
                                                                    <parameters>
                                                                        <parameter>
                                                                            <parameterId>F</parameterId>
                                                                            <unit>%</unit>
                                                                            <parameterValue>
                                                                                <standardValue>1</standardValue>
                                                                            </parameterValue>
                                                                            <bsv>
                                                                                <bsvType>none</bsvType>
                                                                            </bsv>
                                                                            <validation>
                                                                                <errorMessage>
                                                                                    <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                </errorMessage>
                                                                                <operation>
                                                                                    <softFormula>
                                                                                        <inputs>
                                                                                            <input>
                                                                                                <id>F</id>
                                                                                                <type>double</type>
                                                                                            </input>
                                                                                        </inputs>)"
                                                 R"(
                                                                                        <code>
                                                                                            <![CDATA[
                                                                                            return F <= 1.0 and F > 0.0;
                                                                                                        ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                            <parameter>
                                                                                <parameterId>Ka</parameterId>
                                                                                <unit>h-1</unit>
                                                                                <parameterValue>
                                                                                    <standardValue>0.609</standardValue>
                                                                                </parameterValue>
                                                                                <bsv>
                                                                                    <bsvType>none</bsvType>
                                                                                </bsv>
                                                                                <validation>
                                                                                    <errorMessage>
                                                                                        <text lang='en'>No check on Ka now.</text>
                                                                                    </errorMessage>
                                                                                    <operation>
                                                                                        <softFormula>
                                                                                            <inputs>
                                                                                                <input>
                                                                                                    <id>Ka</id>
                                                                                                    <type>double</type>
                                                                                                </input>
                                                                                            </inputs>
                                                                                            <code>
                                                                                                <![CDATA[
                                                                                                return true;
                                                                                                ]]>
                                                                                            </code>
                                                                                        </softFormula>
                                                                                        <comments/>
                                                                                    </operation>
                                                                                    <comments/>
                                                                                </validation>
                                                                                <comments/>
                                                                            </parameter>
                                                                        </parameters>
                                                                        <correlations/>
                                                                    </parameterSet>
                                                                </parameterSetAnalyteGroup>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                    </formulationAndRoutes>
                                                    <timeConsiderations>
                                                        <halfLife>
                                                            <unit>h</unit>
                                                            <duration>
                                                                <standardValue>12</standardValue>
                                                            </duration>
                                                            <multiplier>20</multiplier>
                                                            <comments>
                                                                <comment lang='en'>TODO : Find the half life</comment>
                                                            </comments>
                                                        </halfLife>
                                                        <outdatedMeasure>
                                                            <unit>d</unit>
                                                            <duration>
                                                                <standardValue>100</standardValue>
                                                            </duration>
                                                            <comments>
                                                                <comment lang='en'>TODO : This value is not set now</comment>
                                                            </comments>
                                                        </outdatedMeasure>
                                                    </timeConsiderations>
                                                    <comments/>
                                                </drugModel>
                                            </model>)";

    std::string incompatibleImatinibModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
                                         <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                             <history>
                                                 <revisions>
                                                     <revision>
                                                         <revisionAction>creation</revisionAction>
                                                         <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                         <institution>HEIG-VD // REDS</institution>
                                                         <email>yann.thoma@heig-vd.ch</email>
                                                         <date>2018-10-30</date>
                                                         <comments>
                                                             <comment lang='en'>This file is based on the first version of
                                                                                                                                         imatinib : ch.heig-vd.ezechiel.imatinib.xml
                                                                                                                         </comment>
                                                         </comments>
                                                     </revision>
                                                     <revision>
                                                         <revisionAction>modification</revisionAction>
                                                         <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                         <institution>HEIG-VD // REDS</institution>
                                                         <email>yann.thoma@heig-vd.ch</email>
                                                         <date>2018-11-07</date>
                                                         <comments>
                                                             <comment lang='en'>Now the Ka and F parameters are absorption parameters.
                                                                                                                         </comment>
                                                         </comments>
                                                     </revision>
                                                 </revisions>
                                             </history>
                                             <head>
                                                 <drug>
                                                     <atcs>
                                                         <atc>L01XE01</atc>
                                                     </atcs>
                                                     <activeSubstances>
                                                         <activeSubstance>imatinib</activeSubstance>
                                                     </activeSubstances>
                                                     <drugName>
                                                         <name lang='en'>Imatinib</name>
                                                         <name lang='fr'>Imatinib</name>
                                                     </drugName>
                                                     <drugDescription>
                                                         <desc lang='en'>TODO : Add a description here</desc>
                                                     </drugDescription>
                                                     <tdmStrategy>
                                                         <text lang='en'>TODO : Add a TDM strategy</text>
                                                     </tdmStrategy>
                                                 </drug>)"
                                                  R"(
                                                 <study>
                                                     <studyName>
                                                         <name lang='en'>Therapeutic Drug Monitoring of Imatinib.
                                         Bayesian and Alternative Methods to Predict Trough Levels</name>
                                                     </studyName>
                                                     <studyAuthors>Verena Gotta, Nicolas Widmer, Michael Montemurro, Serge Leyvraz, Amina Haouala, Laurent A. Decosterd, Chantal Csajka and Thierry Buclin</studyAuthors>
                                                     <description>
                                                         <desc lang='en'>Based on Widmer et al. Br J Clin Pharmacol 2006, validated by Gotta et al. Clin Pharamcokinet 2012. Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                         <desc lang='fr'>Basé sur Widmer et al. Br J Clin Pharmacol 2006, validé par Gotta et al. Clin Pharamcokinet 2012</desc>
                                                     </description>
                                                     <references>
                                                         <reference type='bibtex'>
                                                             <![CDATA[@article{Gotta2012,
                                                             author={Gotta, Verena
                                                             and Widmer, Nicolas
                                                             and Montemurro, Michael
                                                             and Leyvraz, Serge
                                                             and Haouala, Amina
                                                             and Decosterd, Laurent A.
                                                             and Csajka, Chantal
                                                             and Buclin, Thierry},
                                                             title={Therapeutic Drug Monitoring of Imatinib},
                                                             journal={Clinical Pharmacokinetics},
                                                             year={2012},
                                                             month={Mar},
                                                             day={01},
                                                             volume={51},
                                                             number={3},
                                                             pages={187--201},
                                                             abstract={The imatinib trough plasma concentration (Cmin) correlates with clinical response in cancer patients. Therapeutic drug monitoring (TDM) of plasma Cmin is therefore suggested. In practice, however, blood sampling for TDM is often not performed at trough. The corresponding measurement is thus only remotely informative about Cmin exposure.},
                                                             issn={1179-1926},
                                                             doi={10.2165/11596990-000000000-00000},
                                                             url={https://doi.org/10.2165/11596990-000000000-00000}
                                                             }
                                         ]]>
                                                         </reference>
                                                     </references>
                                                 </study>
                                                 <comments/>
                                             </head>
                                             <drugModel>
                                                 <drugId>imatinib</drugId>
                                                 <configId>imatinib.gotta2012</configId>
                                                 <drugModelId>ch.tucuxi.imatinib.gotta2012_bad</drugModelId>
                                                 <domain>
                                                     <description>
                                                         <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                     </description>
                                                     <constraints>
                                                         <constraint>
                                                             <constraintType>hard</constraintType>
                                                             <errorMessage>
                                                                 <text lang='en'>The age shall be in the range [20,88]</text>
                                                             </errorMessage>)"
                                                  R"(
                                                             <requiredCovariates>
                                                                 <covariateId>age</covariateId>
                                                             </requiredCovariates>
                                                             <checkOperation>
                                                                 <softFormula>
                                                                     <inputs>
                                                                         <input>
                                                                             <id>age</id>
                                                                             <type>double</type>
                                                                         </input>
                                                                     </inputs>
                                                                     <code>
                                                                         <![CDATA[return ((age >= 20) && (age <= 88));
                                                                         ]]>
                                                                     </code>
                                                                 </softFormula>
                                                                 <comments/>
                                                             </checkOperation>
                                                             <comments/>
                                                         </constraint>
                                                         <constraint>
                                                             <constraintType>soft</constraintType>
                                                             <errorMessage>
                                                                 <text lang='en'>The weight should be in the range [44,110]</text>
                                                             </errorMessage>
                                                             <requiredCovariates>
                                                                 <covariateId>bodyweight</covariateId>
                                                             </requiredCovariates>
                                                             <checkOperation>
                                                                 <softFormula>
                                                                     <inputs>
                                                                         <input>
                                                                             <id>bodyweight</id>
                                                                             <type>double</type>
                                                                         </input>
                                                                     </inputs>
                                                                     <code>
                                                                         <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                         ]]>
                                                                     </code>
                                                                 </softFormula>
                                                                 <comments/>
                                                             </checkOperation>
                                                             <comments/>
                                                         </constraint>
                                                     </constraints>
                                                 </domain>
                                                 <covariates>
                                                     <covariate>
                                                         <covariateId>bodyweight</covariateId>
                                                         <covariateName>)"
                                                  R"(
                                                             <name lang='en'>Total Body Weight</name>
                                                             <name lang='fr'>Poids total</name>
                                                         </covariateName>
                                                         <description>
                                                             <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                         <desc lang='fr'>Poids total du patient, en kilogramme</desc>
                                                         </description>
                                                         <unit>kg</unit>
                                                         <covariateType>standard</covariateType>
                                                         <dataType>double</dataType>
                                                         <interpolationType>linear</interpolationType>
                                                         <refreshPeriod>
                                                           <unit>d</unit>
                                                           <value>30</value>
                                                         </refreshPeriod>
                                                         <covariateValue>
                                                             <standardValue>70</standardValue>
                                                         </covariateValue>
                                                         <validation>
                                                             <errorMessage>
                                                                 <text lang='en'>The body weight shall be in the interval [44,100].</text>
                                                             </errorMessage>
                                                             <operation>
                                                                 <softFormula>
                                                                     <inputs>
                                                                         <input>
                                                                             <id>bodyweight</id>
                                                                             <type>double</type>
                                                                         </input>
                                                                     </inputs>
                                                                     <code>
                                                                         <![CDATA[return ((bodyweight >= 44) && (bodyweight <= 110));
                                                                         ]]>
                                                                     </code>
                                                                 </softFormula>
                                                                 <comments/>
                                                             </operation>
                                                             <comments/>
                                                         </validation>
                                                         <comments/>
                                                     </covariate>
                                                     <covariate>
                                                         <covariateId>gist</covariateId>
                                                         <covariateName>
                                                             <name lang='en'>GIST</name>
                                                             <name lang='fr'>GIST</name>
                                                         </covariateName>
                                                         <description>
                                                             <desc lang='en'>Gastrointestinal stromal tumour</desc>
                                                             <desc lang='fr'>Tumeur stromale gastro-intestinale</desc>
                                                         </description>)"
                                                  R"(
                                                         <unit>-</unit>
                                                         <covariateType>standard</covariateType>
                                                         <dataType>bool</dataType>
                                                         <interpolationType>direct</interpolationType>
                                                         <refreshPeriod>
                                                           <unit>y</unit>
                                                           <value>1</value>
                                                         </refreshPeriod>
                                                         <covariateValue>
                                                             <standardValue>0</standardValue>
                                                         </covariateValue>
                                                         <validation>
                                                             <errorMessage>
                                                                 <text lang='en'>Always correct.</text>
                                                             </errorMessage>
                                                             <operation>
                                                                 <softFormula>
                                                                     <inputs>
                                                                         <input>
                                                                             <id>gist</id>
                                                                             <type>bool</type>
                                                                         </input>
                                                                     </inputs>
                                                                     <code>
                                                                         <![CDATA[return true;
                                                                         ]]>
                                                                     </code>
                                                                 </softFormula>
                                                                 <comments/>
                                                             </operation>
                                                             <comments/>
                                                         </validation>
                                                         <comments/>
                                                     </covariate>
                                                     <covariate>
                                                         <covariateId>sex</covariateId>
                                                         <covariateName>
                                                             <name lang='en'>Sex</name>
                                                             <name lang='fr'>Sexe</name>
                                                         </covariateName>
                                                         <description>
                                                             <desc lang='en'>Sex of the patient</desc>
                                                             <desc lang='fr'>Sexe du patient</desc>
                                                         </description>
                                                         <unit>-</unit>
                                                         <covariateType>sex</covariateType>
                                                         <dataType>double</dataType>
                                                         <interpolationType>direct</interpolationType>
                                                         <refreshPeriod>
                                                           <unit>y</unit>
                                                           <value>1</value>
                                                         </refreshPeriod>)"
                                                  R"(
                                                         <covariateValue>
                                                             <standardValue>0.5</standardValue>
                                                         </covariateValue>
                                                         <validation>
                                                             <errorMessage>
                                                                 <text lang='en'>The sex is a double within the range [0,1]. 0 for female, 1 for male</text>
                                                             </errorMessage>
                                                             <operation>
                                                                 <softFormula>
                                                                     <inputs>
                                                                         <input>
                                                                             <id>sex</id>
                                                                             <type>double</type>
                                                                         </input>
                                                                     </inputs>
                                                                     <code>
                                                                         <![CDATA[return ((sex >= 0.0) && (sex <= 1.0));
                                                                         ]]>
                                                                     </code>
                                                                 </softFormula>
                                                                 <comments/>
                                                             </operation>
                                                             <comments/>
                                                         </validation>
                                                         <comments/>
                                                     </covariate>
                                                     <covariate>
                                                         <covariateId>age</covariateId>
                                                         <covariateName>
                                                             <name lang='en'>Age</name>
                                                             <name lang='fr'>Age</name>
                                                         </covariateName>
                                                         <description>
                                                             <desc lang='en'>Age of the patient, in years</desc>
                                                             <desc lang='fr'>Âge du patient, en années</desc>
                                                         </description>
                                                         <unit>y</unit>
                                                         <covariateType>ageInYears</covariateType>
                                                         <dataType>double</dataType>
                                                         <interpolationType>direct</interpolationType>
                                                         <refreshPeriod>
                                                           <unit>y</unit>
                                                           <value>1</value>
                                                         </refreshPeriod>
                                                         <covariateValue>
                                                             <standardValue>50</standardValue>
                                                         </covariateValue>
                                                         <validation>
                                                             <errorMessage>
                                                                 <text lang='en'>The age shall be in the interval [20,88].</text>
                                                             </errorMessage>)"
                                                  R"(
                                                             <operation>
                                                                 <softFormula>
                                                                     <inputs>
                                                                         <input>
                                                                             <id>age</id>
                                                                             <type>double</type>
                                                                         </input>
                                                                     </inputs>
                                                                     <code>
                                                                         <![CDATA[return ((age >= 20) && (age <= 88));
                                                                         ]]>
                                                                     </code>
                                                                 </softFormula>
                                                                 <comments/>
                                                             </operation>
                                                             <comments/>
                                                         </validation>
                                                         <comments/>
                                                     </covariate>
                                                 </covariates>
                                                 <activeMoieties>
                                                     <activeMoiety>
                                                         <activeMoietyId>imatinib</activeMoietyId>
                                                         <activeMoietyName>
                                                             <name lang='en'>Imatinib</name>
                                                         </activeMoietyName>
                                                         <unit>ug/l</unit>
                                                         <analyteIdList>
                                                             <analyteId>imatinib</analyteId>
                                                         </analyteIdList>
                                                         <analytesToMoietyFormula>
                                                             <hardFormula>direct</hardFormula>
                                                             <comments/>
                                                         </analytesToMoietyFormula>
                                                         <targets>
                                                             <target>
                                                                 <targetType>residual</targetType>
                                                                 <targetValues>
                                                                     <unit>ug/l</unit>
                                                                     <min>
                                                                         <standardValue>750</standardValue>
                                                                     </min>
                                                                     <max>
                                                                         <standardValue>1500</standardValue>
                                                                     </max>
                                                                     <best>
                                                                         <standardValue>1000</standardValue>
                                                                     </best>
                                                                     <toxicityAlarm>
                                                                         <standardValue>10000.0</standardValue>
                                                                     </toxicityAlarm>)"
                                                  R"(
                                                                     <inefficacyAlarm>
                                                                         <standardValue>0.0</standardValue>
                                                                     </inefficacyAlarm>
                                                                 </targetValues>
                                                                 <comments>
                                                                     <comment lang='en'>A Toxicity and inefficacyAlarm should be added</comment>
                                                                 </comments>
                                                             </target>
                                                         </targets>
                                                     </activeMoiety>
                                                 </activeMoieties>
                                                 <analyteGroups>
                                                     <analyteGroup>
                                                         <groupId>imatinib</groupId>
                                                         <pkModelId>linear.1comp.macro</pkModelId>
                                                         <analytes>
                                                             <analyte>
                                                                 <analyteId>imatinib</analyteId>
                                                                 <unit>ug/l</unit>
                                                                 <molarMass>
                                                                     <value>493.603</value>
                                                                     <unit>g/mol</unit>
                                                                 </molarMass>
                                                                 <description>
                                                                     <desc lang='en'>There is only a single analyte : imatinib.</desc>
                                                                 </description>
                                                                 <errorModel>
                                                                     <errorModelType>proportional</errorModelType>
                                                                     <sigmas>
                                                                         <sigma>
                                                                             <standardValue>0.3138</standardValue>
                                                                         </sigma>
                                                                     </sigmas>
                                                                     <comments/>
                                                                 </errorModel>
                                                                 <comments/>
                                                             </analyte>
                                                         </analytes>
                                                         <dispositionParameters>
                                                             <parameters>
                                                                 <parameter>
                                                                     <parameterId>CL</parameterId>
                                                                     <unit>l/h</unit>
                                                                     <parameterValue>
                                                                         <standardValue>14.3</standardValue>
                                                                         <aprioriComputation>
                                                                             <softFormula>
                                                                                 <inputs>
                                                                                     <input>
                                                                                         <id>CL_population</id>
                                                                                         <type>double</type>)"
                                                  R"(
                                                                                     </input>
                                                                                     <input>
                                                                                         <id>bodyweight</id>
                                                                                         <type>double</type>
                                                                                     </input>
                                                                                     <input>
                                                                                         <id>age</id>
                                                                                         <type>double</type>
                                                                                     </input>
                                                                                     <input>
                                                                                         <id>gist</id>
                                                                                         <type>bool</type>
                                                                                     </input>
                                                                                     <input>
                                                                                         <id>sex</id>
                                                                                         <type>double</type>
                                                                                     </input>
                                                                                 </inputs>
                                                                                 <code>
                                                                                     <![CDATA[
                                                                                     theta1 = CL_population;
                                                                                     theta4 = 5.42;
                                                                                     theta5 = 1.49;
                                                                                     theta6 = -5.81;
                                                                                     theta7 = -0.806;

                                                                                     MEANBW = 70;
                                                                                     FBW = (bodyweight - MEANBW) / MEANBW;

                                                                                     MEANAG = 50;
                                                                                     FAGE = (age - MEANAG) / MEANAG;

                                                                                     if (gist)
                                                                                       PATH = 1;
                                                                                     else
                                                                                       PATH = 0;

                                                                                     MALE = sex;

                                                                                     TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                                                                     return TVCL;
                                                                                                              ]]>
                                                                                 </code>
                                                                             </softFormula>
                                                                             <comments/>
                                                                         </aprioriComputation>
                                                                     </parameterValue>
                                                                     <bsv>
                                                                         <bsvType>exponential</bsvType>
                                                                         <stdDevs>)"
                                                  R"(
                                                                             <stdDev>0.356</stdDev>
                                                                         </stdDevs>
                                                                     </bsv>
                                                                     <validation>
                                                                         <errorMessage>
                                                                             <text lang='en'>Clearance shall be in the range [0, 300].</text>
                                                                         </errorMessage>
                                                                         <operation>
                                                                             <softFormula>
                                                                                 <inputs>
                                                                                     <input>
                                                                                         <id>CL</id>
                                                                                         <type>double</type>
                                                                                     </input>
                                                                                 </inputs>
                                                                                 <code>
                                                                                     <![CDATA[
                                                                                     return CL < 300.0 and CL > 0.0;
                                                                                                 ]]>
                                                                                     </code>
                                                                                 </softFormula>
                                                                                 <comments/>
                                                                             </operation>
                                                                             <comments/>
                                                                         </validation>
                                                                         <comments/>
                                                                     </parameter>
                                                                     <parameter>
                                                                         <parameterId>V</parameterId>
                                                                         <unit>l</unit>
                                                                         <parameterValue>
                                                                             <standardValue>347</standardValue>
                                                                             <aprioriComputation>
                                                                                 <softFormula>
                                                                                     <inputs>
                                                                                         <input>
                                                                                             <id>V_population</id>
                                                                                             <type>double</type>
                                                                                         </input>
                                                                                         <input>
                                                                                             <id>sex</id>
                                                                                             <type>double</type>
                                                                                         </input>
                                                                                     </inputs>
                                                                                     <code>
                                                                                         <![CDATA[
                                                                                         theta2 = V_population;
                                                                                         theta8 = 46.2;
                                                                                         tvv = theta2 + theta8 * sex - theta8 * (1 - sex);
                                                                                         return tvv;
                                                                                                                                     ]]>)"
                                                  R"(
                                                                                     </code>
                                                                                 </softFormula>
                                                                                 <comments/>
                                                                             </aprioriComputation>
                                                                         </parameterValue>
                                                                         <bsv>
                                                                             <bsvType>exponential</bsvType>
                                                                             <stdDevs>
                                                                                 <stdDev>0.629</stdDev>
                                                                             </stdDevs>
                                                                         </bsv>
                                                                         <validation>
                                                                             <errorMessage>
                                                                                 <text lang='en'>V shall be positive.</text>
                                                                             </errorMessage>
                                                                             <operation>
                                                                                 <softFormula>
                                                                                     <inputs>
                                                                                         <input>
                                                                                             <id>V</id>
                                                                                             <type>double</type>
                                                                                         </input>
                                                                                     </inputs>
                                                                                     <code>
                                                                                         <![CDATA[
                                                                                         return V < 300.0 and V > 0.0;
                                                                                                     ]]>
                                                                                         </code>
                                                                                     </softFormula>
                                                                                     <comments/>
                                                                                 </operation>
                                                                                 <comments/>
                                                                             </validation>
                                                                             <comments/>
                                                                         </parameter>
                                                                     </parameters>
                                                                     <correlations>
                                                                         <correlation>
                                                                             <param1>CL</param1>
                                                                             <param2>V</param2>
                                                                             <value>0.798</value>
                                                                             <comments>
                                                                                 <comment lang='fr'>coefficient de correlation correspondant à omega2=0.179</comment>
                                                                             </comments>
                                                                         </correlation>
                                                                     </correlations>
                                                                 </dispositionParameters>
                                                             </analyteGroup>
                                                         </analyteGroups>)"
                                                  R"(
                                                         <formulationAndRoutes default='id0'>
                                                             <formulationAndRoute>
                                                                 <formulationAndRouteId>id0</formulationAndRouteId>
                                                                 <formulation>parenteralSolution</formulation>
                                                                 <administrationName>champ libre</administrationName>
                                                                 <administrationRoute>intravenousDrip</administrationRoute>
                                                                 <absorptionModel>bolus</absorptionModel>
                                                                 <dosages>
                                                                     <analyteConversions>
                                                                         <analyteConversion>
                                                                             <analyteId>imatinib</analyteId>
                                                                             <factor>1</factor>
                                                                         </analyteConversion>
                                                                     </analyteConversions>
                                                                     <availableDoses>
                                                                         <unit>mg</unit>
                                                                         <default>
                                                                             <standardValue>400</standardValue>
                                                                         </default>
                                                                         <rangeValues>
                                                                             <from>
                                                                                 <standardValue>100</standardValue>
                                                                             </from>
                                                                             <to>
                                                                                 <standardValue>400</standardValue>
                                                                             </to>
                                                                             <step>
                                                                                 <standardValue>100</standardValue>
                                                                             </step>
                                                                         </rangeValues>
                                                                         <fixedValues>
                                                                             <value>600</value>
                                                                             <value>800</value>
                                                                         </fixedValues>
                                                                     </availableDoses>
                                                                     <availableIntervals>
                                                                         <unit>h</unit>
                                                                         <default>
                                                                             <standardValue>24</standardValue>
                                                                         </default>
                                                                         <fixedValues>
                                                                             <value>12</value>
                                                                             <value>24</value>
                                                                         </fixedValues>
                                                                     </availableIntervals>
                                                                     <comments/>
                                                                 </dosages>)"
                                                  R"(
                                                                 <absorptionParameters>
                                                                     <parameterSetAnalyteGroup>
                                                                         <analyteGroupId>imatinib</analyteGroupId>
                                                                         <absorptionModel>extra</absorptionModel>
                                                                         <parameterSet>
                                                                             <parameters>
                                                                                 <parameter>
                                                                                     <parameterId>F</parameterId>
                                                                                     <unit>%</unit>
                                                                                     <parameterValue>
                                                                                         <standardValue>1</standardValue>
                                                                                     </parameterValue>
                                                                                     <bsv>
                                                                                         <bsvType>none</bsvType>
                                                                                     </bsv>
                                                                                     <validation>
                                                                                         <errorMessage>
                                                                                             <text lang='en'>F shall be in the interval [0,1].</text>
                                                                                         </errorMessage>
                                                                                         <operation>
                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>
                                                                                                         <id>F</id>
                                                                                                         <type>double</type>
                                                                                                     </input>
                                                                                                 </inputs>
                                                                                                 <code>
                                                                                                     <![CDATA[
                                                                                                     return F <= 1.0 and F > 0.0;
                                                                                                                 ]]>
                                                                                                     </code>
                                                                                                 </softFormula>
                                                                                                 <comments/>
                                                                                             </operation>
                                                                                             <comments/>
                                                                                         </validation>
                                                                                         <comments/>
                                                                                     </parameter>
                                                                                     <parameter>
                                                                                         <parameterId>Ka</parameterId>
                                                                                         <unit>h-1</unit>
                                                                                         <parameterValue>
                                                                                             <standardValue>0.609</standardValue>
                                                                                         </parameterValue>
                                                                                         <bsv>
                                                                                             <bsvType>none</bsvType>
                                                                                         </bsv>
                                                                                         <validation>
                                                                                             <errorMessage>
                                                                                                 <text lang='en'>No check on Ka now.</text>
                                                                                             </errorMessage>)"
                                                  R"(
                                                                                             <operation>
                                                                                                 <softFormula>
                                                                                                     <inputs>
                                                                                                         <input>
                                                                                                             <id>Ka</id>
                                                                                                             <type>double</type>
                                                                                                         </input>
                                                                                                     </inputs>
                                                                                                     <code>
                                                                                                         <![CDATA[
                                                                                                         return true;
                                                                                                         ]]>
                                                                                                     </code>
                                                                                                 </softFormula>
                                                                                                 <comments/>
                                                                                             </operation>
                                                                                             <comments/>
                                                                                         </validation>
                                                                                         <comments/>
                                                                                     </parameter>
                                                                                 </parameters>
                                                                                 <correlations/>
                                                                             </parameterSet>
                                                                         </parameterSetAnalyteGroup>
                                                                     </absorptionParameters>
                                                                 </formulationAndRoute>
                                                             </formulationAndRoutes>
                                                             <timeConsiderations>
                                                                 <halfLife>
                                                                     <unit>h</unit>
                                                                     <duration>
                                                                         <standardValue>12</standardValue>
                                                                     </duration>
                                                                     <multiplier>20</multiplier>
                                                                     <comments>
                                                                         <comment lang='en'>TODO : Find the half life</comment>
                                                                     </comments>
                                                                 </halfLife>
                                                                 <outdatedMeasure>
                                                                     <unit>d</unit>
                                                                     <duration>
                                                                         <standardValue>100</standardValue>
                                                                     </duration>
                                                                     <comments>
                                                                         <comment lang='en'>TODO : This value is not set now</comment>
                                                                     </comments>
                                                                 </outdatedMeasure>
                                                             </timeConsiderations>
                                                             <comments/>
                                                         </drugModel>
                                                     </model>)";



    //  query strings

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

    std::string queryStringNoDrugModel = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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



    std::string queryStringDiffTreatmentFAndR = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                            <dosageTimeRange>
                                                                <start>2018-07-09T08:00:00</start>
                                                                <end>2018-07-11T08:00:00</end>
                                                                <dosage>
                                                                    <dosageLoop>
                                                                        <lastingDosage>)"
                                                R"(
                                                                            <interval>12:00:00</interval>
                                                                            <dose>
                                                                                <value>450</value>
                                                                                <unit>mg</unit>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>parenteralSolution</formulation>
                                                                                <administrationName>foo bar</administrationName>
                                                                                <administrationRoute>nasal</administrationRoute>
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
                                                    </samples>
                                                    <!-- Personalised targets -->
                                                    <targets>
                                                    </targets>
                                                </drug>
                                                <drug>
                                                    <drugId>rifampicin</drugId>
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
                                                                                <value>450</value>
                                                                                <unit>mg</unit>
                                                                                <infusionTimeInMinutes>60</infusionTimeInMinutes>
                                                                            </dose>
                                                                            <formulationAndRoute>
                                                                                <formulation>parenteralSolution</formulation>
                                                                                <administrationName>foo bar</administrationName>
                                                                                <administrationRoute>oral</administrationRoute>
                                                                                <absorptionModel>infusion</absorptionModel>
                                                                            </formulationAndRoute>
                                                                        </lastingDosage>)"
                                                R"(
                                                                    </dosageLoop>
                                                                </dosage>
                                                            </dosageTimeRange>
                                                            <dosageTimeRange>
                                                                <start>2018-07-09T08:00:00</start>
                                                                <end>2018-07-11T08:00:00</end>
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
                                                                                <administrationRoute>nasal</administrationRoute>
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
                                            <xpertRequest>
                                                <drugId>rifampicin</drugId>
                                                <configId>rifampicin.svensson2017</configId>
                                                <output>
                                                    <format>xml</format>
                                                    <language>en</language>
                                                </output>
                                            </xpertRequest>
                                        </requests>
                                    </query>)";


    std::string queryStringOnlyOneModelDiffFAndR = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                                                <administrationRoute>intravenousDrip</administrationRoute>
                                                                            </formulationAndRoute>
                                                                        </lastingDosage>
                                                                    </dosageLoop>
                                                                </dosage>
                                                            </dosageTimeRange>
                                                        </dosageHistory>)"
                                                   R"(
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


    std::string queryStringOneModelDiffOneModelSameFAndR = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                    </samples>
                                                    <!-- Personalised targets -->)"
                                                           R"(
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



    std::string queryStringOneModelHardConstraintNotMet = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <query version="1.0"
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                        <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                        <drugTreatment>
                                            <!-- All the information regarding the patient -->
                                            <patient>
                                                <covariates>
                                                    <covariate>
                                                        <covariateId>birthdate</covariateId>
                                                        <date>2018-07-11T10:45:30</date>
                                                        <value>1890-01-01T00:00:00</value>
                                                        <unit></unit>
                                                          <dataType>date</dataType>
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

    std::string queryStringOneModelHardConstraintNotMetAndOtherModelOk =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>birthdate</covariateId>
                                                    <date>2018-07-11T10:45:30</date>
                                                    <value>1890-01-01T00:00:00</value>
                                                    <unit></unit>
                                                      <dataType>date</dataType>
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


    std::string queryStringOneModelSoftConstraintNotMet = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>bodyweight</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>170</value>
                                                    <unit>kg</unit>
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
                                                <!-- Samples history -->
                                                <samples>
                                                </samples>
                                                <!-- Personalised targets -->
                                                <targets>
                                                </targets>
                                            </drug>
                                        </drugs>)"
                                                          R"(
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


    std::string queryStringMultipleBirthDateOneModelWithAgeOneModelWithout =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

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
                                                    <covariateId>birthdate</covariateId>
                                                    <date>2018-07-11T10:45:30</date>
                                                    <value>1980-01-01T00:00:00</value>
                                                    <unit></unit>
                                                      <dataType>date</dataType>
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
                                    </drugTreatment>)"
            R"(
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



    std::string queryStringBirthDateDataTypeOneModelWithAgeOneModelWithout =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>birthdate</covariateId>
                                                    <date>2018-07-11T10:45:30</date>
                                                    <value>1990-01-01T00:00:00</value>
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


    std::string queryStringGistUnitOneModelWithGistOneModelWithout =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>gist</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>40</value>
                                                    <unit>kg</unit>
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
                                                    </dosageHistory>)"
            R"(
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




    std::string queryStringThreeModelsAndNoTie = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>bodyweight3</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>70</value>
                                                    <unit>kg</unit>
                                                      <dataType>double</dataType>
                                                    <nature>discrete</nature>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>bodyweight4</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>70</value>
                                                    <unit>kg</unit>
                                                      <dataType>double</dataType>
                                                    <nature>discrete</nature>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>bodyweight5</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>70</value>
                                                    <unit>kg</unit>
                                                      <dataType>double</dataType>
                                                    <nature>discrete</nature>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>bodyweight6</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>70</value>
                                                    <unit>kg</unit>
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
                                                <atc>something</atc>)"
                                                 R"(
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


    std::string queryStringTwoModelsWithTie = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>bodyweight3</covariateId>
                                                    <date>2018-07-06T08:00:00</date>
                                                    <value>70</value>
                                                    <unit>kg</unit>
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
                                                </treatment>)"
                                              R"(
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


    std::string queryStringSelectedModelNotSupportingRequestedLanguageAndEnglish =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

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
                                                <drugId>imatinib0</drugId>
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
                                                </samples>)"
            R"(
                                                <!-- Personalised targets -->
                                                <targets>
                                                </targets>
                                            </drug>
                                            <drug>
                                                <drugId>imatinib1</drugId>
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
                                            <drug>
                                                <drugId>imatinib2</drugId>
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
                                            <drugId>imatinib0</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                            <output>
                                                <format>xml</format>
                                                <language>fr</language>
                                            </output>
                                        </xpertRequest>
                                        <xpertRequest>
                                            <drugId>imatinib1</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                            <output>
                                                <format>xml</format>
                                                <language>fr</language>
                                            </output>
                                        </xpertRequest>
                                        <xpertRequest>
                                            <drugId>imatinib2</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                            <output>
                                                <format>xml</format>
                                                <language>fr</language>
                                            </output>
                                        </xpertRequest>
                                    </requests>
                                </query>)";


    std::string queryStringModelThatSupportsRequestLanguageAndModelOnlyEnglish =
            R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                <query version="1.0"
                                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                    xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                    <date>2022-06-20T10:00:00</date> <!-- Date the xml has been sent -->

                                    <drugTreatment>
                                        <!-- All the information regarding the patient -->
                                        <patient>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>bodyweight</covariateId>
                                                    <date>2017-07-06T08:00:00</date>
                                                    <value>700</value>
                                                    <unit>kg</unit>
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
                                                    </dosageHistory>
                                                </treatment>
                                                <!-- Samples history -->
                                                <samples>
                                                </samples>
                                                <!-- Personalised targets -->
                                                <targets>
                                                </targets>
                                            </drug>
                                            <drug>
                                                <drugId>en_imatinib</drugId>
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
                                                <language>fr</language>
                                            </output>
                                        </xpertRequest>
                                        <xpertRequest>
                                            <drugId>en_imatinib</drugId>
                                                        <configId>imatinib.gotta2012</configId>
                                            <output>
                                                <format>xml</format>
                                                <language>fr</language>
                                            </output>
                                        </xpertRequest>
                                    </requests>
                                </query>)";


    std::string queryStringSuccess = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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


private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        Check that there is an error if the treatment of an XpertRequestResult is nullptr
///        in CovariateValidatorAndModelSelector.
///        The CovariateValidatorAndDrugModelSelector must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and the drug model must always be nullptr.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, NullptrTreatmentCVAMSelector)
{

    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoTreatment, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No treatment set.");
    EXPECT_EQ(xpertRequestResult.getDrugModel(), nullptr);
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        The test loads an xpertRequest that asks for a drug that is not in the drug model
///        repository. The CovariateValidatorAndDrugModelSelector must set the error in the XpertRequestResult,
///        shouldBeProcessed must return false and the drug model must always be nullptr.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, NoDrugModelForDrugIdCVAMSelector)
{

    // Prepare the XpertRequestResult
    vector<std::string> models{}; // We prepare the environment with 0 model.
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringNoDrugModel, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "The drug files directory does not contain a drug model for the given drug: imatinib");
    EXPECT_EQ(xpertRequestResult.getDrugModel(), nullptr);
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        Check that all formulations and routes of the xpert query are the same.
///        When it is not the case, the corresponding XpertRequestResult gets its error message set.
///        There are two requests, one that has a route that changes and the other has an absorption that changes.
///        Both must fail.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, TreatmentFormulationsAndRoutesAreDifferentCVAMSelector)
{

    // Import the query and get the xpertRequest results.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models;
    TestUtils::setupEnv(queryStringDiffTreatmentFAndR, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResultDiffRoute = xpertQueryResult->getXpertRequestResults()[0];
    XpertRequestResult& xpertRequestResultDiffAbsorption = xpertQueryResult->getXpertRequestResults()[1];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultDiffRoute);
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultDiffAbsorption);

    // Compare
    EXPECT_EQ(xpertRequestResultDiffRoute.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResultDiffRoute.getErrorMessage(), "All formulations and routes must be equal.");
    EXPECT_EQ(xpertRequestResultDiffAbsorption.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResultDiffAbsorption.getErrorMessage(), "All formulations and routes must be equal.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        If the model is incompatible in terms of formulation and route and is the only one
///        in the directory, check that the XpertRequestResult error is set and that no drug model is assigned.
///        Treatment absorption is defined as intravascular, which is not compatible with the
///        original imatinib drug model absorption.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, OnlyOneModelWithDifferentFormulationAndRouteCVAMSelector)
{

    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString};
    TestUtils::setupEnv(queryStringOnlyOneModelDiffFAndR, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No valid drug model found.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        If the model is incompatible in terms of formulation and route but there is another compatible
///        in the directory, check that the XpertRequestResult error is not set and that the second model is chosen.
///        The query has an absorption model defined as extravascular. The first model is set to bolus (modified model of the imatinib)
///        and the second model to extra (original model of the imatinib).
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, OneModelWithDifferentAndOneModelWithSameFormulationAndRouteCVAMSelector)
{
    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {incompatibleImatinibModelString, TestUtils::originalImatinibModelString};
    TestUtils::setupEnv(queryStringOneModelDiffOneModelSameFAndR, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        If the model is incompatible in terms of constraints and it is the only model
///        in the directory, check that the XpertRequestResult error is set and that no drug model is assigned.
///        In this case, the patient is too old for the hard constraint age of the original model of imatinib.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, OneModelHardConstraintNotMetCVAMSelector)
{
    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString};
    TestUtils::setupEnv(queryStringOneModelHardConstraintNotMet, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "No valid drug model found.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        If the model is incompatible in terms of constraints, but there is antoher compatible model
///        in the directory, check that the XpertRequestResult error is not set and that the second model is chosen.
///        The first model (original model of the imatinib) has the hard constraint of age not met. The second model
///        (modified model of the imatinib) hard age constraint has been modified to return true in all cases.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, OneModelHardConstraintNotMetAndOtherModelOkCVAMSelector)
{
    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString, modifiedImatinibModelString};
    TestUtils::setupEnv(queryStringOneModelHardConstraintNotMetAndOtherModelOk, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012_modified");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        If the model is partially compatible in terms of constraints and is the only one
///        in the directory, check that the XpertRequestResult error is not set and that the model is chosen.
///        In this case, the patient is overweighted for the soft body weight constraint of
///        the original imatinib model.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, OneModelSoftConstraintNotMetCVAMSelector)
{

    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString};
    TestUtils::setupEnv(queryStringOneModelSoftConstraintNotMet, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        The query defines several birth dates and the model requires an age.
///        This is not logical, so the XpertRequestResult gets its error message set and
///        is no more processable. It stops the search even if there are other models after.
///        The first model is the original imatinib with age
///        as a covariate and the second is a modified version without the age (which could have
///        been chosen). We need to return an error and stop the run because the first model may
///        be better than the second when corrected.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, MultipleBirthDateOneModelWithAgeOneModelWithoutCVAMSelector)
{

    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString, noAgeImatinibModelString};
    TestUtils::setupEnv(queryStringMultipleBirthDateOneModelWithAgeOneModelWithout, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "Patient covariate error found when handling model ch.tucuxi.imatinib.gotta2012, details: Multiple birthdate not allowed.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        The query sets one birth date with a wrong data type and the model requires an age.
///        This is not manageable, so it stops the search of the drug model even if there is a second
///        potentially valid model.
///        The first model is the original imatinib with age
///        as a covariate and the second is a modified version without the age (which could have
///        been chosen). We need to return an error and stop the run because the first model may
///        be better than the second when corrected.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, BadBirthDateDataTypeOneModelWithAgeOneModelWithoutCVAMSelector)
{

    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString, noAgeImatinibModelString};
    TestUtils::setupEnv(queryStringBirthDateDataTypeOneModelWithAgeOneModelWithout, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "Patient covariate error found when handling model ch.tucuxi.imatinib.gotta2012, details: Invalid data type of birthdate.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        covariateValidatorAndModelSelector_getTheBestModel_betweenThreeModelsAndNoTie one gist with the wrong unit and the model requires this covariate.
///        This is not manageable, so stop the search of drug model even if there is a second
///        potentially valid model. The first model is the original imatinib model and the second
///        is a modified imatinib model that does not use gist as a covariate (which probably would
///        have worked). We need to return an error and stop the run because the first model may
///        be better than the second when corrected.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, BadGistUnitOneModelWithGistOneModelWithoutCVAMSelector)
{

    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {TestUtils::originalImatinibModelString, noGistImatinibModelString};
    TestUtils::setupEnv(queryStringGistUnitOneModelWithGistOneModelWithout, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResult.getErrorMessage(),
            "Covariates extraction failed for drug model: ch.tucuxi.imatinib.gotta2012. It may be caused by covariates that could not be converted.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        There is one query with 4 covariates (bodyweight3, bodyweight4 ... bodyweight6) and three versions
///        of the imatinib model:
///         - The first has two out of two definitions missing from query covariates (score 2) (bodyweight 1 & 2)
///         - The second has one out of two definitions missing from query covariates. The present covariate is
///           respecting the definition validation. (score 1) (bodyweight 3 & 4)
///         - The third has two out of two definitions present in query covariates. The present covariates
///           don't respect the definitions validation. (score 2) (bodyweight 5 & 6)
///        The second model must be chosen.
///        The models are custom imatinib models that extend the original model with bodyweight1/2/3/4/5/6.
///        For simplicity, we leave the original covariate definitions but ignore them since none of their value
///        are defined in the query.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, BetweenThreeModelsAndNoTieCVAMSelector)
{
    // Import the query and get the xpertRequest result.
    unique_ptr<XpertQueryResult> xpertQueryResult = nullptr;
    vector<std::string> models = {model0String, model1String, model2String};
    TestUtils::setupEnv(queryStringThreeModelsAndNoTie, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012_2");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        There is one query with 1 covariate (bodyweight 3) and two versions of the imatinib model:
///         - The first has one definition missing from query covariates (score 1) (bodyweight1)
///         - The second has one out of two definitions missing from query covariates.
///           The present covariate is respecting the definition validation. (score 1) (bodyweight2 & 3)
///        There is a tie, but the second model is chosen because it has the most covariate definitions.
///        The models are custom imatinib models that extend the original model with bodyweight1/2/3.
///        For simplicity, we leave the original covariate definitions but ignore them since none of their value
///        are defined in the query.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, BetweenTwoModelsWithTieCVAMSelector)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{model0String, model1String};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringTwoModelsWithTie, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012_2");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        Check that the CovariateValidatiorAndModelSelector fails when the selected drug model does
///        not support the requested language in its covariate definitions.
///        This test makes 3 requests requiring French on 3 different drugs whose drug model
///        covariates does not support french.
///        There are 3 request to test that it fails on the covariate name, the covariate description
///        and the covariate error message.
///        The 3 drug models are based on the original imatinib model but the languages of the first covariate
///        has been changed in 3 different positions.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, SelectedModelNotSupportingRequestedLanguageAndEnglishCVAMSelector)
{
    // Prepare the XpertRequestResults
    vector<std::string> models{modelString0, modelString1, modelString2};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSelectedModelNotSupportingRequestedLanguageAndEnglish, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResultNameProblem = xpertQueryResult->getXpertRequestResults()[0];
    XpertRequestResult& xpertRequestResultDescProblem = xpertQueryResult->getXpertRequestResults()[1];
    XpertRequestResult& xpertRequestResultValErrorProblem = xpertQueryResult->getXpertRequestResults()[2];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultNameProblem);
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultDescProblem);
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultValErrorProblem);

    // Compare
    EXPECT_EQ(xpertRequestResultNameProblem.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResultNameProblem.getErrorMessage(),
            "Best drug model found but covariate definitions dont't support requested language.");

    EXPECT_EQ(xpertRequestResultDescProblem.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResultDescProblem.getErrorMessage(),
            "Best drug model found but covariate definitions dont't support requested language.");

    EXPECT_EQ(xpertRequestResultValErrorProblem.shouldContinueProcessing(), false);
    EXPECT_EQ(
            xpertRequestResultValErrorProblem.getErrorMessage(),
            "Best drug model found but covariate definitions dont't support requested language.");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        Check that the name, the description and the warning messages of a covariateValidationResult
///        are in the requested language or in English when the requested language is not available.
///        This test makes 2 requests requiring French for 2 different drugs.
///        Once time drug model supports French, one time it supports only English.
///        The first model is the original imatinib model and the second is a modified version.
///        This test only checks the body weight covariate validation.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, ModelThatSupportsRequestLanguageAndModelOnlyEnglishCVAMSelector)
{
    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString, onlyEnglishImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringModelThatSupportsRequestLanguageAndModelOnlyEnglish, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResultFrench = xpertQueryResult->getXpertRequestResults()[0];
    XpertRequestResult& xpertRequestResultEnglish = xpertQueryResult->getXpertRequestResults()[1];

    OutputLang Lang1 = xpertRequestResultFrench.getXpertRequest().getOutputLang();
    OutputLang Lang2 = xpertRequestResultEnglish.getXpertRequest().getOutputLang();
    ;

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultFrench);
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResultEnglish);

    // Compare

    // French supported
    EXPECT_EQ(xpertRequestResultFrench.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResultFrench.getErrorMessage(), "");
    EXPECT_EQ(
            getStringWithEnglishFallback(
                    xpertRequestResultFrench.getCovariateValidationResults()[1].getSource()->getName(), Lang1),
            "Poids total");
    EXPECT_EQ(
            getStringWithEnglishFallback(
                    xpertRequestResultFrench.getCovariateValidationResults()[1].getSource()->getDescription(), Lang1),
            "Poids total du patient, en kilogrammes");
    EXPECT_EQ(
            xpertRequestResultFrench.getCovariateValidationResults()[1].getWarning(),
            "Le poids total doit être compris dans l'intervalle [44,100].");

    // Only english supported
    EXPECT_EQ(xpertRequestResultEnglish.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResultEnglish.getErrorMessage(), "");
    EXPECT_EQ(
            getStringWithEnglishFallback(
                    xpertRequestResultEnglish.getCovariateValidationResults()[3].getSource()->getName(), Lang2),
            "Total Body Weight");
    EXPECT_EQ(
            getStringWithEnglishFallback(
                    xpertRequestResultEnglish.getCovariateValidationResults()[3].getSource()->getDescription(), Lang2),
            "Total body weight of patient, in kilogramms");
    EXPECT_EQ(
            xpertRequestResultEnglish.getCovariateValidationResults()[3].getWarning(),
            "The body weight shall be in the interval [44,100].");
}


/// \brief Test cases for the getCovariateValidatorAndModelSelector method.
///        Check that the CovariateValidationResults are correctly defined when a valid model is found.
///        Some coariates are present several times, others not even once. This test is based
///        on the original imatinib model. In addition, the covariate results must be sorted
///        by name (in English as requested in the query) and then by measurement date.
/// \param none but uses the related fixture
TEST_F(CovariateValidatorAndModelSelectorTest, SuccessCVAMSelector)
{

    // Prepare the XpertRequestResult
    vector<std::string> models{TestUtils::originalImatinibModelString};
    unique_ptr<XpertQueryResult> xpertQueryResult;
    TestUtils::setupEnv(queryStringSuccess, models, xpertQueryResult);

    XpertRequestResult& xpertRequestResult = xpertQueryResult->getXpertRequestResults()[0];

    // Execute
    TestUtils::flowStepProvider.getCovariateValidatorAndModelSelector()->perform(xpertRequestResult);

    // Compare
    EXPECT_EQ(xpertRequestResult.shouldContinueProcessing(), true);
    EXPECT_EQ(xpertRequestResult.getErrorMessage(), "");
    EXPECT_EQ(xpertRequestResult.getDrugModel()->getDrugModelId(), "ch.tucuxi.imatinib.gotta2012");

    const vector<CovariateValidationResult>& results = xpertRequestResult.getCovariateValidationResults();
    EXPECT_EQ(results.size(), 5); // 3 covariates ( 2 weights + 1 birthdate) + Gist definition + Sex definition

    EXPECT_EQ(results[0].getSource()->getId(), "age");
    EXPECT_EQ(results[0].getValue(), "1990-01-01 00:00:00");
    EXPECT_EQ(results[0].getUnit().toString(), "");
    EXPECT_EQ(dataTypeToString(results[0].getDataType()), "date");
    EXPECT_EQ(results[0].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[0].getWarning(), "");

    EXPECT_EQ(results[1].getSource()->getId(), "gist");
    EXPECT_EQ(results[1].getValue(), "0.000000");
    EXPECT_EQ(results[1].getUnit().toString(), "-");
    EXPECT_EQ(dataTypeToString(results[1].getDataType()), "bool");
    EXPECT_EQ(results[1].getType() == CovariateType::MODEL, true);
    EXPECT_EQ(results[1].getWarning(), "");

    EXPECT_EQ(results[2].getSource()->getId(), "sex");
    EXPECT_EQ(results[2].getValue(), "0.500000");
    EXPECT_EQ(results[2].getUnit().toString(), "-");
    EXPECT_EQ(dataTypeToString(results[2].getDataType()), "double");
    EXPECT_EQ(results[2].getType() == CovariateType::MODEL, true);
    EXPECT_EQ(results[2].getWarning(), "");

    EXPECT_EQ(results[3].getSource()->getId(), "bodyweight");
    EXPECT_EQ(results[3].getValue(), "70");
    EXPECT_EQ(results[3].getUnit().toString(), "kg");
    EXPECT_EQ(dataTypeToString(results[3].getDataType()), "double");
    EXPECT_EQ(results[3].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[3].getWarning(), "");

    EXPECT_EQ(results[4].getSource()->getId(), "bodyweight");
    EXPECT_EQ(results[4].getValue(), "150000");
    EXPECT_EQ(results[4].getUnit().toString(), "g");
    EXPECT_EQ(dataTypeToString(results[4].getDataType()), "double");
    EXPECT_EQ(results[4].getType() == CovariateType::PATIENT, true);
    EXPECT_EQ(results[4].getWarning(), "The body weight shall be in the interval [44,100].");
}



} // namespace Xpert
} // namespace Tucuxi
