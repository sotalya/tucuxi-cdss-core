.. SPDX-License-Identifier: AGPL-3.0-or-later


TucuCDSS XML Response
*********************

.. contents:: Contents
	:depth: 1
	:backlinks: top
	:local:


.. _r_tuberxpertResult:

tuberxpertResult
================

.. code-block:: xml
    :linenos:

    <tuberxpertResult>
        <computationTime>2024-05-15T14:30:00</computationTime>
        <language>fr</language>
        <drug></drug>
        <admin></admin>
        <covariates>
            <covariate></covariate>
        </covariates>
        <treatment></treatment>
        <samples>
            <sample></sample>
        </samples>
        <dataAdjustment></dataAdjustment>
        <parameters>
            <typical>
                <parameter></parameter>
            </typical>
            <apriori>
                <parameter></parameter>
            </apriori>
            <aposteriori>
                <parameter></parameter>
            </aposteriori>
        </parameters>
        <statistics></statistics>
        <computationCovariates>
            <computationCovariate></computationCovariate>
        </computationCovariates>
    </tuberxpertResult>


.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<tuberxpertResult>", "", "1:1", "All result data"
    "__<computationTime>", "xs:dateTime", "1:1", "Computation time"
    "__<language>", "xs:string", "1:1", "Language of the results"
    "__<drug>", "drugIntroType", "1:1", "Information about drug and dose"
    "__<admin>", "adminType", "0:1", "Administrative information about patient, mandator and institute"
    "__<covariates>", "", "1:1", "Covariates given to the computation core"
    "____<covariate>", "covariateType", "1:N", "Covariate information"
    "__<treatment>", "treatmentType", "1:1", "Treatment information"
    "__<samples>", "", "0:1", "Samples used for the calculation"
    "____<sample>", "sampleType", "0:N", "Samples information"
    "__<dataAdjustment>", "dataAdjustmentType", "1:1", "Data adjustment information after calculation"
    "__<parameters>", "", "1:1", "Parameters information"
    "____<typical>", "", "1:1", "Parameters typical"
    "______<parameter>", "parameterType", "1:1", "Parameter information"
    "____<apriori>", "", "1:1", "Parameters apriori"
    "______<parameter>", "parameterType", "1:1", "Parameter information"
    "____<aposteriori>", "", "0:1", "Parameters aposteriori"
    "______<parameter>", "parameterType", "1:1", "Parameter information"
    "__<statistics>", "statisticsType", "1:1", "Statistics of the calculation"
    "__<computationCovariates>", "", "1:1", "Covariates used by the computation core"
    "____<computationCovariate>", "computationCovariateType", "1:N", "Computation covariate information"


* The ``<language>`` tag indicates the language used in the response.
  Its value corresponds to the language requested in the query (via
  ``<language>`` or ``<output><language>``), provided a matching
  translation file is available.  If the requested language is not
  available, the system falls back to the default language.
  Currently shipped translations are "en" and "fr".


.. _r_drugIntroType:

Drug Intro
==========

.. code-block:: xml
    :linenos:

    <drug>
        <drugId>imatinib</drugId>
        <lastDose>
            <value>3</value>
            <unit>mg/L</unit>
        </lastDose>
        <drugModelId>imatinib.model</drugModelId>
    </drug>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<drug>", "", "1:1", "All drug information"
    "__<drugId>", "xs:string", "1:1", "Id of the drug"
    "__<lastDose>", "", "1:1", "Last dose of drug information"
    "____<value>", "xs:decimal", "0:1", "Value of the last dose"
    "____<unit>", "unitType", "0:1", "Unit of the value"
    "__<drugModelId>", "xs:string", "1:1", "Drug model used by the computation core"


.. _r_adminType:

Administration data
===================

.. code-block:: xml
    :linenos:

    <admin>
        <mandator>
            <person></person>
            <institute></institute>
        </mandator>
        <patient>
            <person></person>
            <institute></institute>
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

* The ``key`` attribute of ``<clinicalData>`` is required and must be a string.


.. _r_personType:

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


.. _r_instituteType:

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

.. _r_addressType:

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

* The ``<postalCode>`` must be a 4-digit number not starting with 0.


.. _r_phoneType:

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

* The ``<type>`` tag is a string enumerate. It can be "professional" or "private".

.. _r_emailType:

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

* The ``<address>`` must follow the email pattern ``xxx@aaa.yy``.
* The ``<type>`` tag is a string enumerate. It can be "professional" or "private".


.. _r_covariateType:

Covariate
=========

.. code-block:: xml
    :linenos:

    <covariate>
        <covariateId>cov123</covariateId>
        <date>2023-07-16T14:30:00</date>
        <name>Heart Rate</name>
        <value>75</value>
        <unit>bpm</unit>
        <dataType>int</dataType>
        <desc>Heart rate measured at rest</desc>
        <source>patient</source>
        <warning level="normal">Value within normal range</warning>
    </covariate>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<covariate>", "", "1:1", "Covariate information"
    "__<covariateId>", "xs:string", "1:1", "Id of the covariate"
    "__<date>", "xs:dateTime", "1:1", "Date of the measure of the covariate"
    "__<name>", "xs:string", "1:1", "Name of the covariate"
    "__<value>", "xs:string", "1:1", "Value of the covariate"
    "__<unit>", "unitType", "1:1", "Unit of the covariate value"
    "__<dataType>", "xs:string", "1:1", "Data type of the value given"
    "__<desc>", "xs:string", "1:1", "Description of the covariate"
    "__<source>", "xs:string", "1:1", "Source of the covariate"
    "__<warning>", "warningType", "0:1", "Warning associated with the covariate; indicates a related problem."

* The ``<dataType>`` tag is a string enumerate. It can be "int", "double", "bool" or "date".
* The ``<source>`` tag is a string enumerate. It can be "patient" or "default".
* The ``<warning>`` tag requires a ``level`` attribute with value "critical" or "normal", followed by a description.

.. _r_treatmentType:

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

.. _r_dosageHistoryType:

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
    "__<dosageTimeRange>", "dosageTimeRangeType", "0:N", "Time ranges for dosages"


.. _r_dosageTimeRangeType:

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

.. _r_dosageType:

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

* ``<lastingDosage>``, ``<dailyDosage>`` and ``<weeklyDosage>`` cannot be set simultaneously — choose exactly one.

.. _r_dosageLoopType:

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

* ``<lastingDosage>``, ``<dailyDosage>`` and ``<weeklyDosage>`` cannot be set simultaneously — choose exactly one.


.. _r_dosageSteadyStateType:

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

* ``<lastingDosage>``, ``<dailyDosage>`` and ``<weeklyDosage>`` cannot be set simultaneously — choose exactly one.


.. _r_dosageRepeatType:

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


* The ``<iterations>`` tag must be 1 at minimum.
* ``<lastingDosage>``, ``<dailyDosage>`` and ``<weeklyDosage>`` cannot be set simultaneously — choose exactly one.


.. _r_dosageSequenceType:

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

* ``<lastingDosage>``, ``<dailyDosage>`` and ``<weeklyDosage>`` cannot be set simultaneously — choose exactly one.


.. _r_lastingDosageType:

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

.. _r_dailyDosageType:

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


.. _r_weeklyDosageType:

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
    "__<day>", "xs:int", "1:1", "Day of the week (0 = Monday … 6 = Sunday)"
    "__<time>", "xs:time", "1:1", "Time for intake"
    "__<dose>", "doseType", "1:1", "Dose information"
    "__<formulationAndRoute>", "formulationAndRouteType", "1:1", "Information about formulation and route for medicine."

* The ``<day>`` tag must be an integer between 0 and 6 included.


.. _r_doseType:

Dose
====
.. code-block:: xml
    :linenos:

    <dose>
        <value>600</value>
        <unit>mg</unit>
        <infusionTimeInMinutes>60</infusionTimeInMinutes>
        <warning level="critical">Problem with dose</warning>
    </dose>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dose>", "", "1:1", "Data about the dose"
    "__<value>", "xs:decimal", "1:1", "Value of the dose"
    "__<unit>", "unitType", "1:1", "Unit of the dose"
    "__<infusionTimeInMinutes>", "xs:decimal", "0:1", "Infusion time in minutes"
    "__<warning>", "warningType", "0:1", "Warning associated with the dose; indicates a related problem."

* The ``<warning>`` tag requires a ``level`` attribute with value "critical" or "normal", followed by a description.


.. _r_formulationAndRouteType:

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

* The ``<formulation>`` tag is a string enumerate. It can be "undefined", "parenteralSolution", "oralSolution" or "test".
* The ``<administrationRoute>`` tag is a string enumerate. It can be "undefined", "intramuscular", "intravenousBolus", "intravenousDrip", "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal" or "vaginal".
* The ``<absorptionModel>`` tag is a string enumerate. It can be "undefined", "intravascular", "extravascular", "extravascularLag" or "infusion".


.. _r_sampleType:

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
                <percentile>2</percentile>
                <value>2.5</value>
                <unit>mg/l</unit>
                <warning level="normal">No problem</warning>
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
    "______<percentile>", "xs:decimal", "1:1", "Percentile value"
    "______<value>", "xs:decimal", "1:1", "Value of the concentration"
    "______<unit>", "unitType", "1:1", "Unit of the concentration"
    "______<warning>", "warningType", "0:1", "Warning associated with the concentration; indicates a related problem."

* The ``<warning>`` tag requires a ``level`` attribute with value "critical" or "normal", followed by a description.


.. _r_dataAdjustmentType:

Data Adjustment
===============

.. code-block:: xml
    :linenos:

    <dataAdjustment>
        <analyteIds>
            <analyteId>Analyte ID1</analyteId>
            <analyteId>Analyte ID2</analyteId>
        </analyteIds>
        <adjustments>
            <adjustment></adjustment>
        </adjustments>
    </dataAdjustment>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<dataAdjustment>", "", "1:1", "Data adjustment for dosage"
    "__<analyteIds>", "", "0:1", "All analyte"
    "____<analyteId>", "xs:string", "1:N", "Id of the analyte"
    "__<adjustments>", "", "1:1", "All adjustment information"
    "____<adjustment>", "adjustmentType", "1:N", "Adjustment information"


.. _r_adustmentType:

Adjustment
==========
.. code-block:: xml
    :linenos:

    <adjustment>
        <score>2</score>
        <targetEvaluations>
            <targetEvaluation></targetEvaluation>
        </targetEvaluations>
        <dosageHistory></dosageHistory>
        <cycleDatas>
            <cycleData></cycleData>
        </cycleDatas>
    </adjustment>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<adjustment>", "", "1:1", "Adjustment details"
    "__<score>", "xs:decimal", "1:1", "Score of the target evaluation"
    "__<targetEvaluations>", "", "1:1", "Target evaluation lists"
    "____<targetEvaluation>", "targetEvaluationType", "1:N", "Target evaluation information"
    "__<dosageHistory>", "dosageHistoryType", "1:1", "History of dosages"
    "__<cycleDatas>", "", "1:1", "Cycle data information"
    "____<cycleData>", "cycleDataType", "1:N", "Cycle data"


.. _r_targetEvaluationType:

Target Evaluation
=================
.. code-block:: xml
    :linenos:

    <targetEvaluation>
        <targetType>peak</targetType>
        <unit>mg/l</unit>
        <value>0.9</value>
        <min>0.8</min>
        <best>1.05</best>
        <max>1.650</max>
        <inefficacyAlarm>0.6</inefficacyAlarm>
        <toxicityAlarm>20</toxicityAlarm>
    </targetEvaluation>

.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<targetEvaluation>", "", "1:1", "Target Evalutation"
    "__<targetType>", "xs:string", "1:1", "Type of the evaluation"
    "__<unit>", "unitType", "1:1", "Unit of the evalutation"
    "__<value>", "xs:decimal", "1:1", "Value of the evaluation"
    "__<min>", "xs:decimal", "1:1", "Minimum value for the target"
    "__<best>", "xs:decimal", "1:1", "Best value for the target"
    "__<max>", "xs:decimal", "1:1", "Maximum value for the target"
    "__<inefficacyAlarm>", "xs:decimal", "0:1", "Inefficacy alarm threshold"
    "__<toxicityAlarm>", "xs:decimal", "0:1", "Toxicity alarm threshold"


* The ``<targetType>`` tag is a string enumerate. It can be "peak", "residual", "mean", "auc", "aucOverMic", "timeOverMic", "aucDividedByMic" or "peakDividedByMic".


.. _r_cycledatatype:

Cycle Data
==========
.. code-block:: xml
    :linenos:

    <cycleData>
        <start>2018-07-06T08:00:00</start>
        <end>2023-07-08T08:00:00</end>
        <unit>mg/l</unit>
        <times></times>
        <values></values>
    </cycleData>


.. csv-table::
    :header: "Tag name", "Format", "Occ.", "Description"
    :widths: 15, 10, 5, 30

    "<cycleData>", "", "1:1", "Cycle Data"
    "__<start>", "xs:dateTime", "1:1", "Start date interval for cycle data"
    "__<end>", "xs:dateTime", "1:1", "End date interval for cycle data"
    "__<unit>", "unitType", "1:1", "Unit of the cycle data"
    "__<times>", "xs:string", "1:1", "Comma-separated list of times in hours from start"
    "__<values>", "xs:string", "1:1", "Comma-separated list of concentration values"
