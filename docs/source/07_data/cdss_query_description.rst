.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _queryFormat:

************
Query format
************

This section describes the structure of a Tucuxi-CDSS query, that is, the
structure of the XML file that is fed as input to :program:`tucuxi_cdss_cli` in
order to obtain the desired adjustment.

We will start by outlining the global structure of a query, and then delve into
the individual fields. Dosage-related fields, though also presented here for completeness, are detailed in length in section :ref:`dosage`.

.. contents:: Contents
	:depth: 1
	:backlinks: top
	:local:


.. _query:

Query
==========

.. code-block:: xml
    :linenos:

    <query version="1.0">
        <queryId>12345</queryId>
        <clientId>client123</clientId>
        <date>2024-05-15T14:30:00</date>
        <language>en</language>
        <admin>

        </admin>
        <drugTreatment>

        </drugTreatment>
        <requests>
            <xpertRequest>

            </xpertRequest>
            <request>

            </request>
            <request>

            </request>
        </requests>
    </query>


.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
 	:widths: 15, 10, 5, 30

	"<query>", "", "1:1", "Data about the query"
	"__<queryId>", "xs:string", "0:1", "Query ID"
	"__<clientId>", "xs:string", "0:1", "Client ID"
	"__<date>", "xs:dateTime", "1", "Date of the request"
	"__<language>", "xs:string", "0:1", "Language of the report generated"
	"__<admin>", "adminType", "0:1", "Administrative information"
	"__<drugTreatment>", "parametersType", "1", "Drug treatment"
	"__<requests>", "", "1", "All requests"
	"____<xpertRequest>", "xperRequestType", "0:N", "Xpert request (primary request type for the CDSS, see :ref:`xpertRequestType`)"
	"____<request>", "requestType", "0:N", "Tucuxi-compatible request (not processed by :program:`tucuxi_cdss_cli`, see :ref:`requestType`)"

* The <language> tag specifies the desired language for the generated report.  Currently supported values are "en" and "fr".  Actual availability depends on the translation files present on the system (see :doc:`../05_configuration/configuration`).
* The version attribute must follow a rule of two numbers separated by a dot.

.. _adminType:

Administrative
=================


.. code-block:: xml
    :linenos:

    <admin>
        <mandator>
            <person>

            </person>
            <institute>

            </institute>
        </mandator>
        <patient>
            <person>

            </person>
            <institute>

            </institute>
        </patient>
        <clinicalDatas>
            <clinicalData key="BloodType">O+</clinicalData>
            <clinicalData key="Allergies">None</clinicalData>
        </clinicalDatas>
    </admin>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
 	:widths: 15, 10, 5, 30

	"<admin>", "", "1:1", "Data about administrative"
	"__<mandator>", "", "0:1", "Mandator information"
	"____<person>", "personType", "1:1", "Person information"
	"____<institute>", "instituteType", "0:1", "Institute information"
	"__<patient>", "", "0:1", "Patient information"
	"____<person>", "personType", "1:1", "Person information"
	"____<institute>", "instituteType", "0:1", "Institute information"
	"__<clinicalDatas>", "", "0:N", "Clinical data"

Key attribute of clinicalData are required and must be strings.

.. _personType:

Person
========

.. code-block:: xml
    :linenos:

    <person>
        <id>123</id>
        <title>Dr</title>
        <firstName>John</firstName>
        <lastName>Doe</lastName>
        <address>

        </address>
        <phone>

        </phone>
        <email>

        </email>
    </person>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
 	:widths: 15, 10, 5, 30

	"<personType>", "", "1:1", "Data about person"
	"__<id>", "xs:string", "0:1", "Id of the person"
	"__<title>", "xs:string", "0:1", "Title of the person"
	"__<firstName>", "xs:string", "1:1", "First name of the person"
	"__<lastName>", "xs:string", "1:1", "Last name of the person"
	"__<address>", "addressType", "0:1", "Address information"
    "__<phone>", "phoneType", "0:1", "Phone information"
    "__<email>", "emailType", "0:1", "Email information"


.. _instituteType:

Institute
=========

.. code-block:: xml
    :linenos:

    <institute>
        <id>456</id>
        <name>Health Institute</name>
        <address>

        </address>
        <phone>

        </phone>
        <email>

        </email>
    </institute>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<instituteType>", "", "1:1", "Data about the institute"
    "__<id>", "xs:string", "0:1", "Id of the institute"
    "__<name>", "xs:string", "1:1", "Name of the institute"
    "__<address>", "addressType", "0:1", "Address information"
    "__<phone>", "phoneType", "0:1", "Phone information"
    "__<email>", "emailType", "0:1", "Email information"


.. _addressType:

Address
=======

.. code-block:: xml
    :linenos:

    <address>
        <street>This Street 20</street>
        <postalCode>2345</postalCode>
        <city>Smallville</city>
        <state>KS</state>
        <country>USA</country>
    </address>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<address>", "", "1:1", "Data about the address"
    "__<street>", "xs:string", "1:1", "Street address"
    "__<postalCode>", "xs:string", "1:1", "Postal code"
    "__<city>", "xs:string", "1:1", "City"
    "__<state>", "xs:string", "0:1", "State"
    "__<country>", "xs:string", "0:1", "Country"

The <postalCode> must be a 4 digit number not starting with 0.


.. _phoneType:

Phone
=====

.. code-block:: xml
    :linenos:

    <phone>
        <number>+1122334455</number>
        <type>private</type>
    </phone>
.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<phone>", "", "1:1", "Data about the phone number"
    "__<number>", "xs:string", "1:1", "Phone number"
    "__<type>", "coordinateType", "0:1", "Type of phone number"

The <type> tag is a enumerate string. It can be "professional" or "private".

.. _emailType:

Email
=====

.. code-block:: xml
    :linenos:

    <email>
        <address>jane.smith@example.com</address>
        <type>professional</type>
    </email>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<email>", "", "1:1", "Data about the email address"
    "__<address>", "xs:string", "1:1", "Email address"
    "__<type>", "coordinateType", "0:1", "Type of email address"

* The <address> must follow the email pattern of xxx@aaa.yy
* The <type> tag is a enumerate string. It can be "professional" or "private".

.. _parametersTypedrug:

Drug Treatment
===============

.. code-block:: xml
    :linenos:

    <drugTreatment>
        <patient>
            <covariates>
                <covariate>
                </covariate>
            </covariates>
        </patient>
        <drugs>
            <drug>
            </drug>
        </drugs>
    </drugTreatment>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<drugTreatment>", "", "1:1", "Data about the parameters"
    "__<patient>", "", "1:1", "Patient parameters"
    "____<covariates>", "", "1:1", "Covariates"
    "______<covariate>", "covariateType", "0:N", "List of covariates"
    "__<drugs>", "", "1:1", "Drug treatments"
    "____<drug>", "drugType", "1:N", "List of drugs"

.. _covariateType:

Covariate
=========
.. code-block:: xml
    :linenos:

    <covariate>
        <covariateId>cov1</covariateId>
        <date>2024-05-14T10:00:00</date>
        <value>45</value>
        <unit>years</unit>
        <dataType>int</dataType>
        <nature>continuous</nature>
    </covariate>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<covariate>", "", "1:1", "Data about a covariate"
    "__<covariateId>", "xs:string", "1:1", "ID of the covariate"
    "__<date>", "xs:dateTime", "1:1", "Date of the covariate measurement"
    "__<value>", "xs:string", "1:1", "Value of the covariate"
    "__<unit>", "unitType", "1:1", "Unit of the covariate value"
    "__<dataType>", "xs:string", "1:1", "Type of data"
    "__<nature>", "xs:string", "0:1", "Nature of the covariate"

* The <dataType> tag is a string enumerate. It can be "int", "double", "bool" or "date".
* The <nature> tag is a string enumerate. It can be "continuous", "discrete" or "categorical".

.. _drugType:

Drug
====

.. code-block:: xml
    :linenos:

    <drug>
        <drugId>imatinib</drugId>
        <activePrinciple>something</activePrinciple>
        <brandName>brandName</brandName>
        <atc>thing</atc>
        <treatment>
            <dosageHistory></dosageHistory>
        </treatment>
        <samples>
            <sample></sample>
        </samples>
    </drug>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<drug>", "", "1:1", "Data about a drug"
    "__<drugId>", "xs:string", "1:1", "ID of the drug"
    "__<activePrinciple>", "xs:string", "1:1", "Active principle of the drug"
    "__<brandName>", "xs:string", "1:1", "Brand name of the drug"
    "__<atc>", "xs:string", "1:1", "ATC code of the drug"
    "__<treatment>", "treatmentType", "1:1", "Treatment details"
    "__<samples>", "", "0:1", "List of samples"
    "____<sample>", "sampleType", "1:N", "Samples related to the drug"
    "__<targets>", "", "0:1", "List of targets"
    "____<target>", "targetType", "1:N", "Targets related to the drug"

.. _treatmentType:

Treatment
=========

.. code-block:: xml
    :linenos:

    <treatment>
        <dosageHistory></dosageHistory>
    </treatment>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<treatment>", "", "1:1", "Data about the treatment"
    "__<dosageHistory>", "dosageHistoryType", "0:1", "History of dosages"


.. _dosageHistoryType:

Dosage History
==============
.. code-block:: xml
    :linenos:

    <dosageHistory>
        <dosageTimeRange></dosageTimeRange>
    </dosageHistory>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageHistory>", "", "1:1", "History of dosages"
    "__<dosageTimeRange>", "dosageTimeRangeType", "1:N", "Time ranges for dosages"

.. _dosageTimeRangeType:

Dosage Time Range
=================
.. code-block:: xml
    :linenos:

    <dosageTimeRange>
        <start>2018-07-06T08:00:00</start>
        <end>2023-07-08T08:00:00</end>
        <dosage>

        </dosage>
    </dosageTimeRange>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageTimeRange>", "", "1:1", "A range of time for dosages"
    "__<start>", "xs:dateTime", "1:1", "Start date interval for the dosage time range"
    "__<end>", "xs:dateTime", "1:1", "End date interval for the dosage time range"
    "__<dosage>", "dosageType", "1:N", "List of dosages within the time range"

.. _dosageType:

Dosage
======
.. code-block:: xml
    :linenos:

    <dosage>
        <dosageLoop></dosageLoop>
        <dosageSteadyState></dosageSteadyState>
        <dosageRepeat></dosageRepeat>
        <dosageSequence></dosageSequence>
        <lastingDosage></lastingDosage>
        <dailyDosage></dailyDosage>
        <weeklyDosage></weeklyDosage>
        <singleDoseAtTimeList></singleDoseAtTimeList>
        <simpleDoseList></simpleDoseList>
    </dosage>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageType>", "", "1:1", "Data about the dosage"
    "__<dosageLoop>", "dosageLoopType", "0:1", "Looped dosage information"
    "__<dosageSteadyState>", "dosageSteadyStateType", "0:1", "Dosage information at steady state"
    "__<dosageRepeat>", "dosageRepeatType", "0:1", "Dosage information for repeated time"
    "__<dosageSequence>", "dosageSequenceType", "0:1", "Dosage information for sequenced time"
    "__<lastingDosage>", "lastingDosageType", "0:1", "Dosage information for lasting time"
    "__<dailyDosage>", "dailyDosageType", "0:1", "Dosage information for daily time"
    "__<weeklyDosage>", "weeklyDosageType", "0:1", "Dosage information for weekly time"
    "__<singleDoseAtTimeList>", "singleDoseAtTimeListType", "0:1", "List of individual doses, each with its own date, formulation and route"
    "__<simpleDoseList>", "simpleDoseListType", "0:1", "Compact list of individual doses sharing a common unit and formulation"

Tags <lastingDosage>, <dailyDosage> and <weeklyDosage> cannot be set simultaneously. You have to choose one of the three.

.. _dosageLoopType:

Dosage Loop
===========
.. code-block:: xml
    :linenos:

    <dosageLoop>
        <dosageRepeat></dosageRepeat>
        <dosageSequence></dosageSequence>
        <lastingDosage></lastingDosage>
        <dailyDosage></dailyDosage>
        <weeklyDosage></weeklyDosage>
    </dosageLoop>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageLoop>", "", "1:1", "Looped dosage information"
    "__<dosageRepeat>", "dosageRepeatType", "1:1", "Dosage information for repeated time"
    "__<dosageSequence>", "dosageSequenceType", "1:1", "Dosage information for sequenced time"
    "__<lastingDosage>", "lastingDosageType", "1:1", "Dosage information for lasting time"
    "__<dailyDosage>", "dailyDosageType", "1:1", "Dosage information for daily time"
    "__<weeklyDosage>", "weeklyDosageType", "1:1", "Dosage information for weekly time"

Tags <lastingDosage>, <dailyDosage> and <weeklyDosage> cannot be set simultaneously. You have to choose one of the three.

.. _dosageSteadyStateType:

Steady State Dosage
====================

.. code-block:: xml
    :linenos:

    <dosageSteadyState>
        <lastDoseDate>2018-07-01T08:00:00</lastDoseDate>
        <dosageRepeat></dosageRepeat>
        <dosageSequence></dosageSequence>
        <lastingDosage></lastingDosage>
        <dailyDosage></dailyDosage>
        <weeklyDosage></weeklyDosage>
    </dosageSteadyState>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageSteadyState>", "", "1:1", "Dosage information at steady state"
    "__<lastDoseDate>", "xs:dateTime", "1:1", "Date of the last dose"
    "__<dosageRepeat>", "dosageRepeatType", "1:1", "Dosage information for repeated time"
    "__<dosageSequence>", "dosageSequenceType", "1:1", "Dosage information for sequenced time"
    "__<lastingDosage>", "lastingDosageType", "1:1", "Dosage information for lasting time"
    "__<dailyDosage>", "dailyDosageType", "1:1", "Dosage information for daily time"
    "__<weeklyDosage>", "weeklyDosageType", "1:1", "Dosage information for weekly time"

Tags <lastingDosage>, <dailyDosage> and <weeklyDosage> cannot be set simultaneously. You have to choose one of the three.


.. _dosageRepeatType:

Repeated Dosage
===============

.. code-block:: xml
    :linenos:

    <dosageRepeat>
        <iterations>3</iterations>

        <dosageRepeat></dosageRepeat>
        <dosageSequence></dosageSequence>
        <lastingDosage></lastingDosage>
        <dailyDosage></dailyDosage>
        <weeklyDosage></weeklyDosage>
    </dosageRepeat>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageRepeat>", "", "1:1", "Repeated dosage information"
    "__<iterations>", "xs:int", "1:1", "Number of iterations"
    "__<dosageRepeat>", "dosageRepeatType", "1:1", "Dosage information for repeated time"
    "__<dosageSequence>", "dosageSequenceType", "1:1", "Dosage information for sequenced time"
    "__<lastingDosage>", "lastingDosageType", "1:1", "Dosage information for lasting time"
    "__<dailyDosage>", "dailyDosageType", "1:1", "Dosage information for daily time"
    "__<weeklyDosage>", "weeklyDosageType", "1:1", "Dosage information for weekly time"


* The <iterations> tag must be 1 at minimum.
* Tags <lastingDosage>, <dailyDosage> and <weeklyDosage> cannot be set simultaneously. You have to choose one of the three.

.. _dosageSequenceType:

Dosage Sequence
===============
.. code-block:: xml
    :linenos:

    <dosageSequence>
        <dosageRepeat></dosageRepeat>
        <dosageSequence></dosageSequence>
        <lastingDosage></lastingDosage>
        <dailyDosage></dailyDosage>
        <weeklyDosage></weeklyDosage>
    </dosageSequence>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dosageSequence>", "", "1:1", "Sequence of dosages"
    "__<dosageRepeat>", "dosageRepeatType", "1:N", "Dosage information for repeated time"
    "__<dosageSequence>", "dosageSequenceType", "1:N", "Dosage information for sequenced time"
    "__<lastingDosage>", "lastingDosageType", "1:N", "Dosage information for lasting time"
    "__<dailyDosage>", "dailyDosageType", "1:N", "Dosage information for daily time"
    "__<weeklyDosage>", "weeklyDosageType", "1:N", "Dosage information for weekly time"

Tags <lastingDosage>, <dailyDosage> and <weeklyDosage> cannot be set simultaneously. You have to choose one of the three.

.. _lastingDosageType:

Lasting Dosage
==============
.. code-block:: xml
    :linenos:

    <lastingDosage>
        <interval>10:00:00</interval>
        <dose></dose>
        <formulationAndRoute></formulationAndRoute>
    </lastingDosage>


.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<lastingDosage>", "", "1:1", "Dosage information for a lasting period"
    "__<interval>", "xs:time", "1:1", "Interval for the dosage"
    "__<dose>", "doseType", "1:1", "Dose information"
    "__<formulationAndRoute>", "formulationAndRouteType", "1:1", "Information about formulation and route for medicine."

.. _dailyDosageType:

Daily Dosage
============
.. code-block:: xml
    :linenos:

    <dailyDosage>
        <time>12:00:00</time>
        <dose></dose>
        <formulationAndRoute></formulationAndRoute>
    </dailyDosage>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dailyDosage>", "", "1:1", "Dosage information for daily intake"
    "__<time>", "xs:time", "1:1", "Time for intake"
    "__<dose>", "doseType", "1:1", "Dose information"
    "__<formulationAndRoute>", "formulationAndRouteType", "1:1", "Information about formulation and route for medicine."


.. _weeklyDosageType:

Weekly Dosage
=============
.. code-block:: xml
    :linenos:

    <weeklyDosage>
        <day>2</day>
        <time>8:00:00</time>
        <dose></dose>
        <formulationAndRoute></formulationAndRoute>
    </weeklyDosage>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<weeklyDosage>", "", "1:1", "Dosage information for weekly intake"
    "__<day>", "xs:int", "1:1", "Day of the week"
    "__<time>", "xs:time", "1:1", "Time for intake"
    "__<dose>", "doseType", "1:1", "Dose information"
    "__<formulationAndRoute>", "formulationAndRouteType", "1:1", "Information about formulation and route for medicine."

The <day> must be an integer between 0 and 6 included.

.. _singleDoseAtTimeListType:

Dose
====
.. code-block:: xml
    :linenos:

    <dose>
        <value>600</value>
        <unit>mg</unit>
        <infusionTimeInMinutes>60<infusionTimeInMinutes>
    </dose>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dose>", "", "1:1", "Data about the dose"
    "__<value>", "xs:decimal", "1:1", "Value of the dose"
    "__<unit>", "unitType", "1:1", "Unit of the dose"
    "__<infusionTimeInMinutes>", "xs:decimal", "0:1", "Infusion time in minutes"

.. _formulationAndRouteType:

Formulation and Route
=====================
.. code-block:: xml
    :linenos:

    <formulationAndRoute>
        <formulation>oralSolution</formulation>
        <administrationName>foo bar</administrationName>
        <administrationRoute>oral</administrationRoute>
        <absorptionModel>extravascular</absorptionModel>
    </formulationAndRoute>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<formulationAndRoute>", "", "1:1", "Data about formulation and administration route"
    "__<formulation>", "xs:string", "1:1", "Formulation of the drug"
    "__<administrationName>", "xs:string", "1:1", "Administration name"
    "__<administrationRoute>", "xs:string", "1:1", "Administration route"
    "__<absorptionModel>", "xs:string", "1:1", "Absorption model"

* The <formulation> tag is a string enumerate. It can be "undefined", "parenteralSolution", "oralSolution" or "test"
* The <administrationRoute> tag is a string enumerate. It can be "undefined", "intramuscular", "intravenousBolus", "intravenousDrip", "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal" or "vaginal".
* The <absorptionModel> tag is a string enumerate. It can be "undefined", "intravascular", "extravascular", "extravascularLag" or "infusion".

.. _sampleType:

Sample
======
.. code-block:: xml
    :linenos:

    <sample>
        <sampleId>12</sampleId>
        <sampleDate>2018-07-09T06:00:30</sampleDate>
        <concentrations>
            <concentration>
                <analyteId>imatinib</analyteId>
                <value>2.5</value>
                <unit>mg/l</unit>
            </concentration>
        </concentrations>
    </sample>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<sample>", "", "1:1", "Data about a sample"
    "__<sampleId>", "xs:positiveInteger", "1:1", "ID of the sample"
    "__<sampleDate>", "xs:dateTime", "1:1", "Date of the sample"
    "__<concentrations>", "", "1:1", "List of concentration measurements"
    "____<concentration>", "", "1:N", "Concentration measurements"
    "______<analyteId>", "xs:string", "1:1", "ID of the analyte"
    "______<value>", "xs:decimal", "1:1", "Value of the concentration"
    "______<unit>", "unitType", "1:1", "Unit of the concentration"

.. _mic:

MIC
===
.. code-block:: xml
    :linenos:

    <mic>
        <unit>mg</unit>
        <micValue>10</micValue>
    </mic>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<mic>", "", "0:1", "Minimum inhibitory concentration (MIC)"
    "__<unit>", "unitType", "1:1", "Unit of the MIC value"
    "__<micValue>", "xs:decimal", "1:1", "Value of the MIC"

.. _targetType:

Target
======
.. code-block:: xml
    :linenos:

    <target>
        <activeMoietyId>imatinib</activeMoietyId>
        <targetType>residual</targetType>
        <unit>mg/l</unit>
        <min>0.8</min>
        <best>1.05</best>
        <max>1.650</max>
        <inefficacyAlarm>0.6</inefficacyAlarm>
        <toxicityAlarm>20</toxicityAlarm>
        <mic></mic>
    </target>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<target>", "", "1:1", "Data about a drug target"
    "__<activeMoietyId>", "xs:string", "1:1", "ID of the active moiety"
    "__<targetType>", "xs:string", "1:1", "Type of the target"
    "__<unit>", "unitType", "1:1", "Unit of measurement for the target"
    "__<min>", "xs:decimal", "1:1", "Minimum value for the target"
    "__<best>", "xs:decimal", "1:1", "Best value for the target"
    "__<max>", "xs:decimal", "1:1", "Maximum value for the target"
    "__<inefficacyAlarm>", "xs:decimal", "0:1", "Inefficacy alarm threshold"
    "__<toxicityAlarm>", "xs:decimal", "0:1", "Toxicity alarm threshold"
    "__<mic>", "mic", "0:1", "Minimum inhibitory concentration (MIC)"

The <targetType> tag is a string enumerate. It can be "peak", "residual", "mean", "auc", "aucOverMic", "timeOverMic", "aucDividedByMic" or "peakDividedByMic".

.. _compartmentOption:

Compartment Option
==================
.. code-block:: xml
    :linenos:

    <compartmentOption>allActiveMoieties</compartmentOption>


.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<compartmentOption>", "", "1:1", "Options for compartments"
    "__<compartmentOption>", "xs:string", "1", "Option for compartments"

The <compartmentOption> tag is a string enumerate. It can be "allActiveMoieties", "allAnalytes", "allCompartments" or "specific".

.. _parametersType:

Parameters Type
===============
.. code-block:: xml
    :linenos:

    <parametersType>apriori</parametersType>


.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<parametersType>", "", "1:1", "Types of parameters"
    "__<parametersType>", "xs:string", "1", "Type of parameters"

The <parametersType> tag is a string enumerate. It can be "population", "apriori", "aposteriori" or "best".


.. _computingOption:

Computing Option
=================
.. code-block:: xml
    :linenos:

    <computingOption>
        <parametersType></parametersType>
        <compartmentOption></compartmentOption>
        <retrieveStatistics></retrieveStatistics>
        <retrieveParameters></retrieveParameters>
        <retrieveCovariates></retrieveCovariates>
    </computingOption>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<computingOption>", "", "1:1", "Options for computing"
    "__<parametersType>", "", "1", "Type of parameters"
    "__<compartmentOption>", "", "1", "Option for compartments"
    "__<retrieveStatistics>", "xs:boolean", "1", "Flag to retrieve statistics"
    "__<retrieveParameters>", "xs:boolean", "1", "Flag to retrieve parameters"
    "__<retrieveCovariates>", "xs:boolean", "1", "Flag to retrieve covariates"


.. _adjustmentTraits:

Adjustment Traits
==================
.. code-block:: xml
    :linenos:

    <adjustmentTraits>
        <computingOption></computingOption>
        <nbPointsPerHour>1</nbPointsPerHour>
        <start>2018-07-06T08:00:00</start>
        <end>2023-07-08T08:00:00</end>
        <adjustmentDate>2018-07-06T08:00:00</adjustmentDate>
        <options>
            <bestCandidatesOption>bestDosage</bestCandidatesOption>
            <loadingOption>loadingDoseAllowed</loadingOption>
            <restPeriodOption>noRestPeriod</restPeriodOption>
            <steadyStateTargetOption>atSteadyState</steadyStateTargetOption>
            <targetExtractionOption>populationValues</targetExtractionOption>
            <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
        </options>
    </adjustmentTraits>


.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<adjustmentTraits>", "", "1:1", "Traits for adjustment"
    "__<computingOption>", "", "1", "Options for computing"
    "__<nbPointsPerHour>", "xs:int", "0:1", "Number of points per hour"
    "__<start>", "xs:dateTime", "1:1", "Start date interval for the dosage time range"
    "__<end>", "xs:dateTime", "1:1", "End date interval for the dosage time range"
    "__<adjustmentDate>", "xs:dateTime", "1", "Date of adjustment"
    "__<options>", "", "0:1", "Options for adjustment"
    "____<bestCandidatesOption>", "xs:string", "1", "Option for best candidates"
    "____<loadingOption>", "xs:string", "0:1", "Option for loading"
    "____<restPeriodOption>", "xs:string", "0:1", "Option for rest period"
    "____<steadyStateTargetOption>", "xs:string", "0:1", "Option for steady state target"
    "____<targetExtractionOption>", "xs:string", "0:1", "Option for target extraction"
    "____<formulationAndRouteSelectionOption>", "xs:string", "0:1", "Option for formulation and route selection"


* The <bestCandidatesOption> tag is a string enumerate. It can be "bestDosage", "allDosages" or "bestDosagePerInterval".
* The <loadingOption> tag is a string enumerate. It can be "noLoadingDose" or "loadingDoseAllowed".
* The <restPeriodOption> tag is a string enumerate. It can be "noRestPeriod" or "restPeriodAllowed".
* The <steadyStateTargetOption> tag is a string enumerate. It can be "atSteadyState" or "withinTreatmentTimeRange".
* The <targetExtractionOption> tag is a string enumerate. It can be "populationValues", "aprioriValues", "individualTargets", "individualTargetsIfDefinitionExists", "definitionIfNoIndividualTarget" or "individualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget".
* The <formulationAndRouteSelectionOption> tag is a string enumerate. It can be "lastFormulationAndRoute", "defaultFormulationAndRoute" or "allFormulationAndRoutes".


.. _predictionAtTimesTraits:

Prediction at Times Traits
===========================
.. code-block:: xml
    :linenos:

    <predictionAtTimesTraits>
        <computingOption></computingOption>
        <dates>
            <date>2018-07-06T08:00:00</date>
            <date>2018-08-01T08:00:00</date>
        </dates>
    </predictionAtTimesTraits>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<predictionAtTimesTraits>", "", "1:1", "Traits for prediction at times"
    "__<computingOption>", "", "1", "Options for computing"
    "__<dates>", "", "1", "Dates for prediction"
    "____<date>", "xs:dateTime", "1+", "Date for prediction"

.. _predictionAtSampleTimesTraits:

Prediction at Sample Times Traits
==================================
.. code-block:: xml
    :linenos:

    <predictionAtSampleTimesTraits>
        <computingOption></computingOption>
    </predictionAtSampleTimesTraits>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<predictionAtSampleTimesTraits>", "", "1:1", "Traits for prediction at sample times"
    "__<computingOption>", "", "1", "Options for computing"

.. _predictionTraits:

Prediction Traits
==================
.. code-block:: xml
    :linenos:

    <predictionTraits>
        <computingOption></computingOption>
        <nbPointsPerHour>1</nbPointsPerHour>
        <start>2018-07-06T08:00:00</start>
        <end>2023-07-08T08:00:00</end>
    </predictionTraits>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<predictionTraits>", "", "1:1", "Traits for prediction"
    "__<computingOption>", "", "1", "Options for computing"
    "__<nbPointsPerHour>", "xs:int", "0:1", "Number of points per hour"
    "__<start>", "xs:dateTime", "1:1", "Start date interval for the dosage time range"
    "__<end>", "xs:dateTime", "1:1", "End date interval for the dosage time range"

.. _percentilesTraits:

Percentiles Traits
===================
.. code-block:: xml
    :linenos:

    <percentilesTraits>
        <computingOption></computingOption>
        <nbPointsPerHour>1</nbPointsPerHour>
        <start>2018-07-06T08:00:00</start>
        <end>2023-07-08T08:00:00</end>
        <ranks>
            <rank>66</rank>
        </ranks>
    </percentilesTraits>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<percentilesTraits>", "", "1:1", "Traits for percentiles"
    "__<computingOption>", "", "1", "Options for computing"
    "__<nbPointsPerHour>", "xs:int", "0:1", "Number of points per hour"
    "__<start>", "xs:dateTime", "1:1", "Start date interval for the dosage time range"
    "__<end>", "xs:dateTime", "1:1", "End date interval for the dosage time range"
    "__<ranks>", "", "0:1", "Ranks for percentiles"
    "____<rank>", "xs:int", "1:N", "Percentile rank value"

The <rank> tag must be between 0 and 100 included.


.. _requestType:

Request Type
============

.. note::

   The ``<request>`` element exists for compatibility with the Tucuxi query
   format (used by :program:`tucucli`).  The :program:`tucuxi_cdss_cli` binary
   does **not** process ``<request>`` elements; use ``<xpertRequest>`` instead
   (see :ref:`xpertRequestType`).

.. warning::

   Tucuxi-CDSS only computes **dosage adjustments**.  The standalone
   prediction, percentile, and prediction-at-times computation types listed
   below are features of the Tucuxi-Core engine and are **not available**
   through :program:`tucuxi_cdss_cli`.  They are documented here only because
   the XML query format is shared with the standalone Tucuxi application.

.. code-block:: xml
    :linenos:

    <request>
        <requestId>1</requestId>
        <drugId>imatinib</drugId>
        <drugModelId>imatinib.model</drugModelId>
        <predictionTraits></predictionTraits>
    </request>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<request>", "", "1:1", "Type of request"
    "__<requestId>", "xs:string", "1", "ID of the request"
    "__<drugId>", "xs:string", "1", "ID of the drug"
    "__<drugModelId>", "xs:string", "1", "ID of the drug model"
    "__<predictionTraits>", "", "0:1", "Traits for prediction"
    "__<predictionAtTimesTraits>", "", "0:1", "Traits for prediction at times"
    "__<predictionAtSampleTimesTraits>", "", "0:1", "Traits for prediction at sample times"
    "__<percentilesTraits>", "", "0:1", "Traits for percentiles"
    "__<adjustmentTraits>", "", "0:1", "Traits for adjustment"

Only one trait element should be supplied per ``<request>``.  If
multiple traits are present, only the first one encountered is used.

.. _xpertRequestType:

Xpert Request Type
===================

The ``<xpertRequest>`` element is the primary request type for the |CDSS|.
It encapsulates both the drug selection and the computation options that
control how the dosage adjustment is calculated.  These options act as a
**per-request configuration** of the computation (see also
:doc:`../05_configuration/configuration` for system-wide settings).

.. code-block:: xml
    :linenos:

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
            <restPeriodOption>noRestPeriod</restPeriodOption>
            <targetExtractionOption>populationValues</targetExtractionOption>
            <formulationAndRouteSelectionOption>lastFormulationAndRoute</formulationAndRouteSelectionOption>
        </options>
    </xpertRequest>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 20, 10, 5, 30

    "<xpertRequest>", "", "1:1", "Type of Xpert request"
    "__<drugId>", "xs:string", "1", "ID of the drug"
    "__<configId>", "xs:string", "1", "ID of the selected configuration"
    "__<output>", "", "1", "Output options"
    "____<format>", "xs:string", "1", "Format of the output (html, xml)"
    "____<language>", "xs:string", "1", "Language of the output (en, fr)"
    "__<adjustmentDate>", "xs:dateTime", "0:1", "Date of adjustment"
    "__<options>", "", "0:1", "Additional options for the request"
    "____<loadingOption>", "xs:string", "0:1", "Option for loading"
    "____<restPeriodOption>", "xs:string", "0:1", "Option for rest period"
    "____<targetExtractionOption>", "xs:string", "0:1", "Option for target extraction"
    "____<formulationAndRouteSelectionOption>", "xs:string", "0:1", "Option for formulation and route selection"

* The <loadingOption> tag is a string enumerate. It can be "noLoadingDose" or "loadingDoseAllowed".
* The <restPeriodOption> tag is a string enumerate. It can be "noRestPeriod" or "restPeriodAllowed".
* The <targetExtractionOption> tag is a string enumerate. It can be "populationValues", "aprioriValues", "individualTargets", "individualTargetsIfDefinitionExists", "definitionIfNoIndividualTarget" or "individualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget".
* The <formulationAndRouteSelectionOption> tag is a string enumerate. It can be "lastFormulationAndRoute", "defaultFormulationAndRoute" or "allFormulationAndRoutes".
* The <format> tag is a string enumerate. It can be "html" or "xml".
* The <language> tag is a string enumerate. It can be "en" or "fr".

Option details
~~~~~~~~~~~~~~

The ``<options>`` block provides per-request control over the adjustment
computation.  When an option is omitted, the system applies a sensible
default.

**loadingOption**
  Controls whether the algorithm may suggest a loading dose (a higher
  initial dose to reach therapeutic levels faster).

  - ``noLoadingDose``: no loading dose is considered.
  - ``loadingDoseAllowed``: a loading dose may be part of the recommended adjustment.

**restPeriodOption**
  Controls whether a rest period (temporary treatment interruption) may
  be suggested before a new dosing regimen begins.

  - ``noRestPeriod``: treatment continues without interruption.
  - ``restPeriodAllowed``: a rest period may be included in the adjustment.

**targetExtractionOption**
  Specifies how the therapeutic target used for adjustment scoring is
  determined.

  - ``populationValues``: use the population target defined in the drug model.
  - ``aprioriValues``: use targets based on a priori parameter estimates.
  - ``individualTargets``: use targets supplied in the patient query.
  - ``individualTargetsIfDefinitionExists``: use patient targets when available, otherwise skip.
  - ``definitionIfNoIndividualTarget``: use the drug-model target only if no individual target is provided.
  - ``individualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget``: prefer patient targets; fall back to drug-model targets.

**formulationAndRouteSelectionOption**
  Specifies which formulation and route to consider when generating
  candidate adjustments.

  - ``lastFormulationAndRoute``: reuse the formulation and route of the most recent dose in the treatment history.
  - ``defaultFormulationAndRoute``: use the formulation and route marked as default in the drug model.
  - ``allFormulationAndRoutes``: evaluate candidates for every formulation/route combination defined in the drug model.
