// SPDX-License-Identifier: AGPL-3.0-or-later
#include "testutils.h"

#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"
#include "tucucore/pkmodel.h"

#include "tucucdss-core/language/languagemanager.h"
#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/result/xpertrequestresult.h"


using namespace std;

const Tucuxi::Xpert::GeneralXpertFlowStepProvider TestUtils::flowStepProvider;

const std::string TestUtils::date_format = "%Y-%m-%dT%H:%M:%S";

const std::string TestUtils::originalImatinibModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
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
                                                           R"(            </studyName>
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
                                                                 <drugModelId>ch.tucuxi.imatinib.gotta2012</drugModelId>
                                                                 <domain>
                                                                     <description>
                                                                         <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                                     </description>
                                                                     <constraints>
                                                                         <constraint>
                                                                             <constraintType>hard</constraintType>
                                                                             <errorMessage>
                                                                                 <text lang='en'>The age shall be in the range [20,88]</text>
                                                                             </errorMessage>
                                                                             <requiredCovariates>
                                                                                 <covariateId>age</covariateId>
                                                                             </requiredCovariates>)"
                                                           R"(
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
                                                                         <covariateName>
                                                                             <name lang='en'>Total Body Weight</name>
                                                                             <name lang='fr'>Poids total</name>
                                                                         </covariateName>)"
                                                           R"(
                                                                         <description>
                                                                             <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                                             <desc lang='fr'>Poids total du patient, en kilogrammes</desc>
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
                                                                                 <text lang='fr'>Le poids total doit être compris dans l'intervalle [44,100].</text>
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
                                                                         <covariateType>standard</covariateType>)"
                                                           R"(
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
                                                                                 <text lang='fr'>Toujours correct.</text>
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
                                                                                 <text lang='fr'>Le sexe est un double de l'intervalle [0,1]. 0 pour une femme, 1 pour un homme.</text>
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
                                                                             <name lang='fr'>Âge</name>
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
                                                                                 <text lang='en'>The age shall be in the interval [20,88].</text>)"
                                                           R"(
                                                                                 <text lang='fr'>L'âge doit se situer dans l'intervalle [20,88].</text>
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
                                                                                     </max>
                                                                                     <best>
                                                                                         <standardValue>1000</standardValue>
                                                                                     </best>
                                                                                     <toxicityAlarm>)"
                                                           R"(
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
                                                                                         <standardValue>14.3</standardValue>
                                                                                         <aprioriComputation>
                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>)"
                                                           R"(
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
                                                                                     </parameterValue>
                                                                                     <bsv>)"
                                                           R"(
                                                                                         <bsvType>exponential</bsvType>
                                                                                         <stdDevs>
                                                                                             <stdDev>0.356</stdDev>
                                                                                         </stdDevs>
                                                                                     </bsv>
                                                                                     <validation>
                                                                                         <errorMessage>
                                                                                             <text lang='fr'>Clearance shall be in the range [0, 300].</text>
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
                                                                                                         tvv = theta2 + theta8 * sex - theta8 * (1 - sex);)"
                                                           R"(
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
                                                                                                 <text lang='fr'>V shall be positive.</text>
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
                                                                                         <absorptionModel>extra</absorptionModel>)"
                                                           R"(
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
                                                                                                             <text lang='fr'>F shall be in the interval [0,1].</text>
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
                                                                                                                 <text lang='fr'>No check on Ka now.</text>
                                                                                                             </errorMessage>
                                                                                                             <operation>
                                                                                                                 <softFormula>
                                                                                                                     <inputs>)"
                                                           R"(
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


const std::string TestUtils::imatinibModelStringNoAge = R"(<?xml version="1.0" encoding="UTF-8"?>
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
                                                        R"(            </studyName>
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
                                                                 <drugModelId>ch.tucuxi.imatinib.gotta2012</drugModelId>
                                                                 <domain>
                                                                     <description>
                                                                         <desc lang='en'>Adult, (Disease: CML and GIST, Age range: 20-88 yrs, Weight range: 44-110kg, AGP plasma concentration range: 0.4–2.0 g/L)</desc>
                                                                     </description>
                                                                     <constraints>
                                                                         )"
                                                        R"(
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
                                                                         <covariateName>
                                                                             <name lang='en'>Total Body Weight</name>
                                                                             <name lang='fr'>Poids total</name>
                                                                         </covariateName>)"
                                                        R"(
                                                                         <description>
                                                                             <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                                                                             <desc lang='fr'>Poids total du patient, en kilogrammes</desc>
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
                                                                                 <text lang='fr'>Le poids total doit être compris dans l'intervalle [44,100].</text>
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
                                                                         <covariateType>standard</covariateType>)"
                                                        R"(
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
                                                                                 <text lang='fr'>Toujours correct.</text>
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
                                                                                 <text lang='fr'>Le sexe est un double de l'intervalle [0,1]. 0 pour une femme, 1 pour un homme.</text>
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
                                                                     </covariate>)"
                                                        R"(
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
                                                                                     <toxicityAlarm>)"
                                                        R"(
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
                                                                                         <standardValue>14.3</standardValue>
                                                                                         <aprioriComputation>
                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>)"
                                                        R"(
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
                                                                                     <bsv>)"
                                                        R"(
                                                                                         <bsvType>exponential</bsvType>
                                                                                         <stdDevs>
                                                                                             <stdDev>0.356</stdDev>
                                                                                         </stdDevs>
                                                                                     </bsv>
                                                                                     <validation>
                                                                                         <errorMessage>
                                                                                             <text lang='fr'>Clearance shall be in the range [0, 300].</text>
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
                                                                                                         tvv = theta2 + theta8 * sex - theta8 * (1 - sex);)"
                                                        R"(
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
                                                                                                 <text lang='fr'>V shall be positive.</text>
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
                                                                                         <absorptionModel>extra</absorptionModel>)"
                                                        R"(
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
                                                                                                             <text lang='fr'>F shall be in the interval [0,1].</text>
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
                                                                                                                 <text lang='fr'>No check on Ka now.</text>
                                                                                                             </errorMessage>
                                                                                                             <operation>
                                                                                                                 <softFormula>
                                                                                                                     <inputs>)"
                                                        R"(
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



const std::string TestUtils::originalBusulfanModelString = R"(<?xml version="1.0" encoding="UTF-8"?>
                                                         <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                                             <!-- Drug history -->
                                                             <history>
                                                                 <revisions>
                                                                     <revision>
                                                                         <revisionAction>creation</revisionAction>
                                                                         <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                         <institution>HEIG-VD // REDS</institution>
                                                                         <email>yann.thoma@heig-vd.ch</email>
                                                                         <date>2018-09-14</date>
                                                                         <comments>
                                                                             <comment lang="en">This file is based on the first version of
                                                                                 busulfan validated by Sylvain Goutelle : ch.heig-vd.ezechiel.busulfan_children.xml
                                                                             </comment>
                                                                         </comments>
                                                                     </revision>
                                                                     <revision>
                                                                         <revisionAction>modification</revisionAction>
                                                                         <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                         <institution>HEIG-VD</institution>
                                                                         <email>yann.thoma@heig-vd.ch</email>
                                                                         <date>2021-01-04</date>
                                                                         <comments>
                                                                             <comment lang='en'>Added the age covariate, for domain checking</comment>
                                                                         </comments>
                                                                     </revision>
                                                                 </revisions>
                                                             </history>
                                                             <!-- Drug description -->
                                                             <head>
                                                                 <drug>
                                                                     <atcs>
                                                                         <atc>L01AB01</atc>
                                                                     </atcs>
                                                                     <activeSubstances>
                                                                         <activeSubstance>busulfan</activeSubstance>
                                                                     </activeSubstances>
                                                                     <drugName>
                                                                         <name lang="en">Busulfan</name>
                                                                         <name lang="fr">Busulfan</name>
                                                                     </drugName>
                                                                     <drugDescription>
                                                                         <desc lang="en">TODO : Add a description here</desc>
                                                                     </drugDescription>
                                                                     <tdmStrategy>
                                                                         <text lang="en">TODO : Add a TDM strategy</text>
                                                                     </tdmStrategy>
                                                                 </drug>
                                                                 <study>
                                                                     <studyName>
                                                                         <name lang="en">Pharmacokinetic behavior and appraisal of intravenous busulfan dosing in infants and older children: the results of a population pharmacokinetic study from a large pediatric cohort undergoing hematopoietic stem-cell transplantation</name>
                                                                     </studyName>)"
                                                           R"(
                                                                     <studyAuthors>Paci A., Vassal G., Moshous D., Dalle J.H., Bleyzac N., Neven B., Galambrun C., Kemmel V., Abdi Z.D., Broutin S., Pétain A., Nguyen L.</studyAuthors>
                                                                     <description>
                                                                         <desc lang="en">Based on the article byPaci et al. Pharmacokinetic Behavior and Appraisal of Intravenous Busulfan Dosing in Infants and Older Children:
                                                                         The Results of a Population Pharmacokinetic Study From a Large Pediatric Cohort Undergoing Hematopoietic Stem-Cell Transplantation.
                                                                         Ther Drug Monit 2012, 34;:198-208</desc>
                                                                     </description>
                                                                     <references>
                                                                         <reference type="bibtex">TODO : Add bibtex
                                                                         </reference>
                                                                     </references>
                                                                 </study>
                                                                 <comments/>
                                                             </head>

                                                             <!-- Drug data -->
                                                             <drugModel>
                                                                 <drugId>busulfan</drugId> <!-- id d'une des substances actives -->
                                                                 <drugModelId>ch.tucuxi.busulfan.paci2012</drugModelId>

                                                                 <domain>
                                                                     <description>
                                                                         <desc lang="en">All children (Age range: 10 days - 15 years, Weight range: 3.5 - 62.5 kg)</desc>
                                                                     </description>
                                                                     <constraints>
                                                                         <constraint>
                                                                             <constraintType>hard</constraintType>
                                                                             <errorMessage>
                                                                                 <text lang="en">The age shall be positive</text>
                                                                             </errorMessage>
                                                                             <requiredCovariates>
                                                                                 <covariateId>age</covariateId>
                                                                             </requiredCovariates>
                                                                             <checkOperation>
                                                                                 <softFormula>
                                                                                     <inputs>
                                                                                         <input>
                                                                                             <id>age</id>
                                                                                             <type>int</type>
                                                                                         </input>
                                                                                     </inputs>
                                                                                     <code><![CDATA[return (age > 0);
                                                                                         ]]>
                                                                                     </code>
                                                                                 </softFormula>
                                                                                 <comments/>
                                                                             </checkOperation>
                                                                             <comments/>
                                                                         </constraint>
                                                                         <constraint>
                                                                             <constraintType>soft</constraintType>
                                                                             <errorMessage>)"
                                                           R"(
                                                                                 <text lang="en">The weight should not be too much</text>
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
                                                                                     <code><![CDATA[return (bodyweight < 100);
                                                                                         ]]>
                                                                                     </code>
                                                                                 </softFormula>
                                                                                 <comments/>
                                                                             </checkOperation>
                                                                             <comments/>
                                                                         </constraint>
                                                                     </constraints>
                                                                 </domain>



                                                                 <!-- Drug model covariates -->
                                                                 <covariates>

                                                                     <covariate>
                                                                         <covariateId>age</covariateId>
                                                                         <covariateName>
                                                                             <name lang='en'>Age</name>
                                                                             <name lang='fr'>Âge</name>
                                                                         </covariateName>
                                                                         <description>
                                                                             <desc lang='en'>Age of the patient, in years, only used for domain checking</desc>
                                                                             <desc lang='fr'>Âge du patient, en années, utilisé pour valider l'usage du modèle</desc>
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
                                                                             <standardValue>10</standardValue>
                                                                         </covariateValue>
                                                                         <validation> <!-- pourrait être une contrainte -->)"
                                                           R"(
                                                                             <errorMessage>
                                                                                <text lang="en">The age shall be positive.</text>
                                                                                <text lang="fr">L'âge doit être positif.</text>
                                                                             </errorMessage>
                                                                             <operation>
                                                                                 <softFormula>
                                                                                     <inputs>
                                                                                         <input>
                                                                                             <id>age</id>
                                                                                             <type>double</type>
                                                                                         </input>
                                                                                     </inputs>
                                                                                     <code><![CDATA[
                                                                                         return (age  > 0);
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
                                                                             <name lang="en">Total Body Weight</name>
                                                                             <name lang="fr">Poids total</name>
                                                                         </covariateName>
                                                                         <description>
                                                                             <desc lang="en">Total body weight of patient, in kilogramms</desc>
                                                                             <desc lang="fr">Poids total du patient, en kilogrammes</desc>
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
                                                                             <standardValue>9</standardValue>
                                                                         </covariateValue>
                                                                         <validation> <!-- pourrait être une contrainte -->
                                                                             <errorMessage>
                                                                                 <text lang="en">The body weight shall be in the interval [0,300].</text>
                                                                                 <text lang="fr">Le poids total doit être compris dans l'intervalle [0,300].</text>
                                                                             </errorMessage>
                                                                             <operation>
                                                                                 <softFormula>)"
                                                           R"(
                                                                                     <inputs>
                                                                                         <input>
                                                                                             <id>bodyweight</id>
                                                                                             <type>double</type>
                                                                                         </input>
                                                                                     </inputs>
                                                                                     <code><![CDATA[
                                                                                         return (bodyweight < 300) && (bodyweight > 0);
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
                                                                         <activeMoietyId>busulfan</activeMoietyId>
                                                                         <activeMoietyName>
                                                                             <name lang="en">Busulfan</name>
                                                                         </activeMoietyName>
                                                                         <unit>mg/l</unit>
                                                                         <analyteIdList>
                                                                             <analyteId>busulfan</analyteId>
                                                                         </analyteIdList>
                                                                         <analytesToMoietyFormula>
                                                                             <hardFormula>direct</hardFormula>
                                                                             <comments/>
                                                                         </analytesToMoietyFormula>
                                                                         <!-- Drug targets -->
                                                                         <targets>
                                                                             <target>
                                                                                 <targetType>peak</targetType>
                                                                                 <targetValues>
                                                                                 <unit>mg/l</unit>
                                                                                     <min>
                                                                                         <standardValue>1.0</standardValue>
                                                                                     </min>
                                                                                     <max>
                                                                                         <standardValue>1.4</standardValue>
                                                                                     </max>
                                                                                     <best>
                                                                                         <standardValue>1.2</standardValue>
                                                                                     </best>
                                                                                     <toxicityAlarm><standardValue>10000.0</standardValue></toxicityAlarm>
                                                                                     <inefficacyAlarm><standardValue>0.0</standardValue></inefficacyAlarm>)"
                                                           R"(
                                                                                 </targetValues>
                                                                                 <times>
                                                                                     <unit>h</unit>
                                                                                     <min>
                                                                                         <standardValue>0.9</standardValue>
                                                                                     </min>
                                                                                     <max>
                                                                                         <standardValue>3</standardValue>
                                                                                     </max>
                                                                                     <best>
                                                                                         <standardValue>2.5</standardValue>
                                                                                     </best>
                                                                                 </times>
                                                                                 <comments>
                                                                                     <comment lang="en">The time values shall be checked. They initially were not aligned with possible infusion times. A Toxicity and inefficacyAlarm should be added</comment>
                                                                                 </comments>
                                                                             </target>
                                                                             <target>
                                                                                 <targetType>cumulativeAuc</targetType>
                                                                                 <targetValues>
                                                                                     <unit>mg*h/l</unit>
                                                                                     <min>
                                                                                         <standardValue>59.0</standardValue>
                                                                                     </min>
                                                                                     <max>
                                                                                         <standardValue>99.0</standardValue>
                                                                                     </max>
                                                                                     <best>
                                                                                         <standardValue>72.0</standardValue>
                                                                                     </best>
                                                                                     <!-- TODO : Check that -->
                                                                                     <toxicityAlarm><standardValue>10000.0</standardValue></toxicityAlarm>
                                                                                     <inefficacyAlarm><standardValue>0.0</standardValue></inefficacyAlarm>
                                                                                 </targetValues>
                                                                                 <comments>
                                                                                     <comment lang="en">This is the target cumulative for a regimen with 16 doses over 4 days, i.e. a dose every 6h.</comment>
                                                                                 </comments>
                                                                             </target>
                                                                         </targets>
                                                                     </activeMoiety>
                                                                 </activeMoieties>

                                                                 <analyteGroups>
                                                                     <analyteGroup>
                                                                         <groupId>busulfan</groupId>
                                                                         <pkModelId>linear.1comp.macro</pkModelId>
                                                                         <analytes>
                                                                             <analyte>
                                                                                 <analyteId>busulfan</analyteId>
                                                                                 <unit>mg/l</unit>
                                                                                 <molarMass>)"
                                                           R"(
                                                                                   <value>246.292</value>
                                                                                     <unit>g/mol</unit>
                                                                                 </molarMass>
                                                                                 <description>
                                                                                     <desc lang="en"></desc>
                                                                                 </description> <!-- peut être vide -->

                                                                                 <errorModel> <!-- optional -->
                                                                                     <errorModelType>mixed</errorModelType>
                                                                                     <sigmas>
                                                                                         <sigma>
                                                                                             <standardValue>0.057</standardValue>
                                                                                         </sigma>
                                                                                         <sigma>
                                                                                             <standardValue>0.11</standardValue>
                                                                                         </sigma>
                                                                                     </sigmas>
                                                                                     <comments>
                                                                                         <comment lang='fr'>add: en mg/L; prop, en % ie 0.11 = 11.0%</comment>
                                                                                     </comments>
                                                                                 </errorModel>
                                                                                 <comments/>
                                                                             </analyte>
                                                                         </analytes>

                                                                         <!-- Drug parameters -->
                                                                         <dispositionParameters>
                                                                             <parameters>
                                                                                 <parameter>
                                                                                     <parameterId>CL</parameterId>
                                                                                     <unit>l/h</unit>
                                                                                     <parameterValue>
                                                                                         <standardValue>2.18</standardValue>
                                                                                         <aprioriComputation>

                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>
                                                                                                         <id>bodyweight</id>
                                                                                                         <type>double</type>
                                                                                                     </input>
                                                                                                 </inputs>
                                                                                                 <code><![CDATA[
                                                                                         theta_1 = 2.18;

                                                                                         if (bodyweight < 9.0)
                                                                                           theta_2=1.25;
                                                                                         else
                                                                                           theta_2=0.76;

                                                                                         return theta_1 * Math.pow((bodyweight/ 9),theta_2);)"
                                                           R"(

                                                                                  ]]>
                                                                                                 </code>
                                                                                             </softFormula>
                                                                                             <comments/>
                                                                                         </aprioriComputation>
                                                                                     </parameterValue>
                                                                                     <bsv>
                                                                                         <bsvType>proportional</bsvType> <!-- même chose que le modèle d'erreur -->
                                                                                         <stdDevs>
                                                                                             <stdDev>0.23</stdDev>
                                                                                         </stdDevs>
                                                                                     </bsv>
                                                                                     <validation>
                                                                                         <errorMessage><text lang="fr"></text></errorMessage>
                                                                                         <operation>
                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>
                                                                                                         <id>CL</id>
                                                                                                         <type>double</type>
                                                                                                     </input>
                                                                                                 </inputs>
                                                                                                 <code><![CDATA[
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
                                                                                         <standardValue>6.6</standardValue>
                                                                                         <aprioriComputation>

                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>
                                                                                                         <id>bodyweight</id>
                                                                                                         <type>double</type>
                                                                                                     </input>
                                                                                                 </inputs>
                                                                                                 <code><![CDATA[
                                                                                         theta_3=0.86;
                                                                         return Math.pow(bodyweight,theta_3);)"
                                                           R"(
                                                                                                     ]]>
                                                                                                 </code>
                                                                                             </softFormula>
                                                                                             <comments/>
                                                                                         </aprioriComputation>
                                                                                     </parameterValue>
                                                                                     <bsv>
                                                                                         <bsvType>proportional</bsvType>
                                                                                         <stdDevs>
                                                                                             <stdDev>0.22</stdDev>
                                                                                         </stdDevs>
                                                                                     </bsv>
                                                                                     <validation>
                                                                                         <errorMessage><text lang="fr"></text></errorMessage>
                                                                                         <operation>
                                                                                             <softFormula>
                                                                                                 <inputs>
                                                                                                     <input>
                                                                                                         <id>V</id>
                                                                                                         <type>double</type>
                                                                                                     </input>
                                                                                                 </inputs>
                                                                                                 <code><![CDATA[
                                                                                                     return V < 300.0 and V > 0.0;
                                                                                                     ]]>
                                                                                                 </code>
                                                                                             </softFormula>
                                                                                             <comments/>
                                                                                         </operation>
                                                                                         <comments/>
                                                                                     </validation>
                                                                                     <comments>
                                                                                         <comment lang="en">Typical volume calculated for a patients with weight = 75 kg</comment>
                                                                                     </comments>
                                                                                 </parameter>
                                                                             </parameters>

                                                                             <!-- elimination parameters correlations -->
                                                                             <correlations />
                                                                         </dispositionParameters>
                                                                     </analyteGroup>
                                                                 </analyteGroups>

                                                                 <!-- We can have various formulation and routes, and for each one a set of absorption parameters and available dosages -->

                                                                 <formulationAndRoutes default="id0">
                                                                     <formulationAndRoute>
                                                                         <formulationAndRouteId>id0</formulationAndRouteId>
                                                                         <formulation>parenteralSolution</formulation><!-- dictionnaire -->
                                                                         <administrationName>champ libre</administrationName>
                                                                         <administrationRoute>intravenousDrip</administrationRoute> <!-- dictionnaire -->)"
                                                           R"(
                                                                         <absorptionModel>infusion</absorptionModel>


                                                                         <!-- Drug dosages -->
                                                                         <dosages>


                                                                             <standardTreatment>
                                                                                 <isFixedDuration>true</isFixedDuration>
                                                                                 <timeValue>
                                                                                     <unit>d</unit>
                                                                                     <value>4</value>
                                                                                 </timeValue>
                                                                             </standardTreatment>

                                                                             <analyteConversions>
                                                                                 <analyteConversion>
                                                                                     <analyteId>busulfan</analyteId>
                                                                                     <factor>1</factor>
                                                                                 </analyteConversion>
                                                                             </analyteConversions>

                                                                             <availableDoses>
                                                                                 <unit>mg</unit>
                                                                                 <default>
                                                                                     <standardValue>10</standardValue>
                                                                                 </default>
                                                                                 <rangeValues>
                                                                                     <from>
                                                                                         <standardValue>1</standardValue>
                                                                                     </from>
                                                                                     <to>
                                                                                         <standardValue>400</standardValue>
                                                                                     </to>
                                                                                     <step>
                                                                                         <standardValue>1</standardValue>
                                                                                     </step>
                                                                                 </rangeValues>
                                                                             </availableDoses>

                                                                             <availableIntervals>
                                                                                 <unit>h</unit>
                                                                                 <default>
                                                                                     <standardValue>6</standardValue>
                                                                                 </default>
                                                                                 <fixedValues>
                                                                                     <value>6</value>
                                                                                     <value>24</value>
                                                                                 </fixedValues>
                                                                             </availableIntervals>
                                                                             <availableInfusions>)"
                                                           R"(
                                                                                 <unit>min</unit>
                                                                                 <default>
                                                                                     <standardValue>120</standardValue>
                                                                                 </default>
                                                                                 <fixedValues>
                                                                                     <value>60</value>
                                                                                     <value>120</value>
                                                                                     <value>180</value>
                                                                                 </fixedValues>
                                                                             </availableInfusions>
                                                                             <comments/>
                                                                         </dosages>

                                                                         <absorptionParameters/>

                                                                     </formulationAndRoute>
                                                                 </formulationAndRoutes>



                                                                 <timeConsiderations>
                                                                     <!-- Drug half-life -->
                                                                     <halfLife>
                                                                         <unit>h</unit>
                                                                         <duration>
                                                                             <standardValue>12</standardValue>
                                                                         </duration>
                                                                         <multiplier>10</multiplier>
                                                                         <comments>
                                                                             <comment lang="en">TODO : Find the half life</comment>
                                                                         </comments>
                                                                     </halfLife>


                                                                     <outdatedMeasure>
                                                                         <unit>d</unit>
                                                                         <duration>
                                                                             <standardValue>100</standardValue>
                                                                         </duration>
                                                                         <comments>
                                                                             <comment lang="en">TODO : This value is not set now</comment>
                                                                         </comments>
                                                                     </outdatedMeasure>
                                                                 </timeConsiderations>

                                                                 <!-- Drug general comments -->
                                                                 <comments>
                                                                 </comments>
                                                             </drugModel>
                                                         </model>
                                                         )";

const std::string TestUtils::englishTranslationFile = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                                        <translation key="man">Man</translation>)"
                                                      R"(
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
                                                        <translation key="unknown">Unknown</translation>)"
                                                      R"(
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

                                                    </translations>
                                                    )";


void TestUtils::setupEnv(
        const std::string& _queryString,
        const std::string& _modelString,
        unique_ptr<Tucuxi::Xpert::XpertQueryResult>& _xpertQueryResult)
{


    vector<std::string> models{_modelString};
    setupEnv(_queryString, models, _xpertQueryResult);

    Tucuxi::Xpert::XpertRequestResult& xpertRequestResult = _xpertQueryResult->getXpertRequestResults()[0];

    // Get the drug model repository
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    Tucuxi::Core::IDrugModelRepository* drugModelRepository =
            pCmpMgr->getComponent<Tucuxi::Core::IDrugModelRepository>("DrugModelRepository");

    // Attribute the drug model to the XpertRequestResult
    xpertRequestResult.setDrugModel(
            drugModelRepository->getDrugModelsByDrugId(xpertRequestResult.getXpertRequest().getDrugId())[0]);
    _xpertQueryResult->incrementRequestIndexBeingProcessed();

    // Loading the dictionary
    loadTranslationsFile(englishTranslationFile);
}

void TestUtils::setupEnv(
        const std::string& _queryString,
        const vector<std::string>& _modelStrings,
        unique_ptr<Tucuxi::Xpert::XpertQueryResult>& _xpertQueryResult)
{
    // Drug models repository creation
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();

    cleanEnv();

    auto drugModelRepository =
            dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

    pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);

    Tucuxi::Core::DrugModelImport drugModelImport;

    // Load each drug model
    for (const std::string& modelString : _modelStrings) {
        std::unique_ptr<Tucuxi::Core::DrugModel> drugModel;
        if (drugModelImport.importFromString(drugModel, modelString) == Tucuxi::Core::DrugModelImport::Status::Ok) {

            Tucuxi::Core::PkModelCollection pkCollection;

            if (!defaultPopulate(pkCollection)) {
                throw std::runtime_error("Could not populate the Pk models collection. No model will be available");
            }

            Tucuxi::Core::DrugModelChecker checker;
            Tucuxi::Core::DrugModelChecker::CheckerResult_t checkerResult =
                    checker.checkDrugModel(drugModel.get(), &pkCollection);
            if (!checkerResult.m_ok) {
                throw std::runtime_error("A drug file has internal errors : " + checkerResult.m_errorMessage);
            }
            drugModelRepository->addDrugModel(drugModel.release());
        }
        else {
            throw std::runtime_error("Failed to import drug file");
        }
    }

    // Query import
    std::unique_ptr<Tucuxi::Xpert::XpertQueryData> query = nullptr;
    Tucuxi::Xpert::XpertQueryImport importer;
    Tucuxi::Xpert::XpertQueryImport::Status importResult = importer.importFromString(query, _queryString);

    if (importResult != Tucuxi::Xpert::XpertQueryImport::Status::Ok) {
        throw std::runtime_error("Setup failed");
    }

    _xpertQueryResult = make_unique<Tucuxi::Xpert::XpertQueryResult>(std::move(query), "random/path");
}

void TestUtils::cleanEnv()
{
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    // Cleaning memory to avoid memory leaks
    auto oldRepo = pCmpMgr->getComponent<Tucuxi::Core::IDrugModelRepository>("DrugModelRepository");
    if (oldRepo != nullptr) {
        pCmpMgr->unregisterComponent("DrugModelRepository");
        delete oldRepo;
    }
}

void TestUtils::loadTranslationsFile(const std::string& _translationsFileXml)
{
    Tucuxi::Xpert::LanguageManager& languageManager = Tucuxi::Xpert::LanguageManager::getInstance();
    languageManager.loadTranslations(_translationsFileXml);
}
