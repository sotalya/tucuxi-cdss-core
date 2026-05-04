.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _srsFunctionalRequirements:

Functional Requirements
***********************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:

Requirements are uniquely identified by the prefix **REQ-** followed by a
subsystem code and a sequence number. Each requirement states a verifiable
condition using the keyword **shall**.


CLI and Process Control
=======================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-CLI-01", "The software shall accept the drug model directory path as a mandatory command-line argument."
   "REQ-CLI-02", "The software shall accept the XML query file path as a mandatory command-line argument."
   "REQ-CLI-03", "The software shall accept the CDSS configuration file path as an optional command-line argument."
   "REQ-CLI-04", "The software shall accept the output directory path as a mandatory command-line argument."
   "REQ-CLI-05", "The software shall accept the optional HTML template directory path argument."
   "REQ-CLI-06", "The software shall accept the mandatory HTML template name argument."
   "REQ-CLI-07", "The software shall accept an optional language path argument for translation files."
   "REQ-CLI-08", "The software shall accept an optional language code argument to override the language specified in the query."
   "REQ-CLI-09", "The software shall accept an optional flag to enable |JSON| debug output."
   "REQ-CLI-10", "The software shall return exit code 0 when all requests in the query file succeed."
   "REQ-CLI-11", "The software shall return exit code 1 when at least one request succeeds and at least one fails."
   "REQ-CLI-12", "The software shall return exit code 2 when no request succeeds."
   "REQ-CLI-13", "The software shall return exit code 4 when the query file cannot be loaded."
   "REQ-CLI-14", "The software shall return exit code 3 when bad arguments are given."
   "REQ-CLI-15", "The software shall process all requests in the query file independently; failure of one request shall not prevent processing of subsequent requests."


Query Processing
================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-QRY-01", "The software shall parse the patient identifier and admin data from the XML query."
   "REQ-QRY-02", "The software shall parse the mandator admin data from the XML query."
   "REQ-QRY-03", "The software shall parse patient covariates from the XML query."
   "REQ-QRY-04", "The software shall validate patient covariates against the configuration file. Values exceeding a soft threshold shall trigger a warning, while values exceeding a hard threshold shall trigger an error."
   "REQ-QRY-05", "If a patient covariate is missing, the software shall emit a warning and use default values from the drug file."
   "REQ-QRY-06", "The software shall parse the complete dosage history, including dose amounts, routes of administration, administration times, and infusion durations, from the XML query."
   "REQ-QRY-07", "The software shall parse observed drug concentration samples and their collection times from the XML query."
   "REQ-QRY-08", "The software shall parse the drug identifier from the XML query."
   "REQ-QRY-09", "The software shall parse the adjustment requests and their parameters from the XML query."
   "REQ-QRY-10", "The software shall reject a query that is not well-formed XML and record an error in the response."
   "REQ-QRY-11", "The software shall reject a query that is missing mandatory fields and record an error in the response."
   "REQ-QRY-12", "The software shall reject a query whose date/time values are not internally consistent and record an error in the response."


Drug Model Loading
==================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-DM-01", "The software shall load the drug model whose identifier matches the drug identifier specified in the query."
   "REQ-DM-02", "The software shall validate the structure of the loaded drug model file before use."
   "REQ-DM-03", "The software shall record an error in the response and skip the affected request if no drug model matching the query's drug identifier is found."
   "REQ-DM-04", "The software shall record an error in the response and skip the affected request if the drug model file is malformed."


PK Computation
==============

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-CMP-01", "The software shall take into account the covariate values, as specified in the drug model, when performing |PK| computations."
   "REQ-CMP-02", "The software shall incorporate observed drug concentration samples (Bayesian estimation) into the individual prediction when samples are provided in the query."
   "REQ-CMP-03", "The software shall evaluate dosage adjustment candidates against the target type and range defined in the query."
   "REQ-CMP-04", "The software shall report a computation error in the response if a numerical error occurs during |PKPD| computation, without aborting other requests."
   "REQ-CMP-05", "The software shall report a failure if no dosage adjustment candidate falls within the specified target range, rather than silently returning an out-of-range candidate."


Response Generation
===================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-RSP-01", "The software shall produce an XML response file conforming to the defined :ref:`data-formats` when requested."
   "REQ-RSP-02", "The response shall include a status element (success or error with a descriptive message) for each request."
   "REQ-RSP-03", "The response shall include a query identifier in each response entry."
   "REQ-RSP-04", "The response shall include unit labels for all numeric output values."
   "REQ-RSP-05", "The adjustment response shall include the predicted concentration time-series computed during the adjustment process."
   "REQ-RSP-06", "The adjustment response shall include per-percentile concentration time-series computed during the adjustment process."
   "REQ-RSP-07", "The response shall include the list of dosage adjustment proposals with their scores."


Report Generation
=================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-RPT-01", "The software shall produce an HTML report by rendering a configurable HTML template with the computation results when HTML output is requested."
   "REQ-RPT-03", "The software shall support the selection of the output language via the command-line argument or the language field in the query."
   "REQ-RPT-04", "If the specified template is not found, the software shall raise an error."
   "REQ-RPT-05", "If the |JSON| debug flag is set, the software shall write the |JSON| data payload to the output directory."

..   "REQ-RPT-02", "The software shall produce a PDF file from the HTML report when PDF output is requested."


Configuration
=============

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "REQ-CFG-01", "The software shall load its runtime configuration from the XML configuration file specified on the command line."
   "REQ-CFG-02", "The software shall use built-in defaults for optional configuration parameters that are absent from the configuration file."
   "REQ-CFG-03", "The software shall abort with exit code 3 if the configuration file is missing."
   "REQ-CFG-04", "The software shall abort with exit code 4 if the configuration file content is invalid."
