.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpTraceability:

Requirements to Test Case Traceability
***************************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


This section maps every requirement from the :ref:`srs` to the test cases
defined in this plan that verify it. Each requirement must be covered by at
least one test case. A dash (—) indicates that no test case currently covers
the requirement; the gap must be resolved before the corresponding
verification activity can be closed.

Abbreviations: **UT** = Unit Test, **IT** = Integration Test,
**ST** = System Test.


Functional Requirements
========================

CLI and Process Control
-----------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-CLI-01", "Accept drug model directory as mandatory argument", "UT-SI01-002, ST-001"
   "REQ-CLI-02", "Accept XML query file as mandatory argument", "UT-SI01-003, ST-001"
   "REQ-CLI-03", "Accept |CDSS| configuration file as optional argument", "UT-SI01-004, ST-001"
   "REQ-CLI-04", "Accept output directory as mandatory argument", "UT-SI01-005, ST-001"
   "REQ-CLI-05", "Accept optional HTML template directory path argument", "ST-001"
   "REQ-CLI-06", "Accept mandatory HTML template name argument", "UT-SI01-006, ST-001"
   "REQ-CLI-07", "Accept optional language path argument", "ST-001"
   "REQ-CLI-08", "Accept optional language code argument", "ST-001, ST-016"
   "REQ-CLI-09", "Accept optional |JSON| debug output flag", "UT-SI01-014, ST-018"
   "REQ-CLI-10", "Return exit code 0 when all requests succeed", "UT-SI01-008, ST-002"
   "REQ-CLI-11", "Return exit code 1 when some requests succeed and some fail", "UT-SI01-009, ST-002"
   "REQ-CLI-12", "Return exit code 2 when no request succeeds", "UT-SI01-010, ST-002"
   "REQ-CLI-13", "Return exit code 4 for query file parsing errors", "UT-SI01-011, ST-004"
   "REQ-CLI-14", "Return exit code 3 when bad arguments are given", "UT-SI01-002, UT-SI01-003, UT-SI01-005, UT-SI01-006, ST-002"
   "REQ-CLI-15", "Process all requests independently; failure of one shall not prevent others", "UT-SI01-013, IT-012, ST-003"


Query Processing
----------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-QRY-01", "Parse patient identifier and admin data from query", "UT-SI03-002, ST-005"
   "REQ-QRY-02", "Parse mandator admin data from query", "—"
   "REQ-QRY-03", "Parse patient covariates from query", "UT-SI03-003"
   "REQ-QRY-04", "Validate patient covariates against configuration file", "UT-SI06-002, UT-SI06-003, ST-S01"
   "REQ-QRY-05", "Emit warning and use drug-model default when covariate is missing", "UT-SI06-004"
   "REQ-QRY-06", "Parse complete dosage history from query", "UT-SI03-004"
   "REQ-QRY-07", "Parse observed drug concentration samples from query", "UT-SI03-005"
   "REQ-QRY-08", "Parse drug identifier from query", "UT-SI03-006"
   "REQ-QRY-09", "Parse the adjustment request and its parameters", "UT-SI03-007, UT-SI03-008, UT-SI03-009"
   "REQ-QRY-10", "Reject malformed XML query; record error in response", "UT-SI03-010, ST-004"
   "REQ-QRY-11", "Reject query missing mandatory fields; record error in response", "UT-SI03-011, UT-SI03-012, UT-SI03-013, ST-005"
   "REQ-QRY-12", "Reject query with internally inconsistent date/time values", "UT-SI06-011"


Drug Model Loading
------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-DM-01", "Load the drug model whose identifier matches the query's drug identifier", "UT-SI04-004, IT-002"
   "REQ-DM-02", "Validate the structure of the loaded drug model file before use", "UT-SI04-003, ST-S02"
   "REQ-DM-03", "Record error and skip request when no matching drug model is found", "UT-SI04-001, UT-SI04-002, IT-005"
   "REQ-DM-04", "Record error and skip request when the drug model file is malformed", "UT-SI04-003, ST-S02"


PK/PD Computation
-----------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-CMP-01", "Apply covariate effects in |PK| computation as defined in the drug model", "IT-003, ST-010"
   "REQ-CMP-02", "Incorporate observed concentration samples via Bayesian estimation", "IT-004, ST-011"
   "REQ-CMP-03", "Evaluate dosage adjustment candidates against the target type and range", "UT-SI07-002, IT-002, ST-009"
   "REQ-CMP-04", "Report computation error without aborting other requests", "UT-SI07-003, UT-SI07-005, IT-012, ST-003"
   "REQ-CMP-05", "Report failure when no dosage adjustment candidate falls within target range", "UT-SI07-002, ST-012"


Response Generation
-------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-RSP-01", "Produce XML response file conforming to the defined format", "UT-SI08-001, IT-006, ST-013"
   "REQ-RSP-02", "Include a status element (success or error) for each request", "UT-SI08-008, ST-013"
   "REQ-RSP-03", "Include the query identifier in each response entry", "IT-006, ST-013"
   "REQ-RSP-04", "Include unit labels on all numeric output values", "UT-SI08-007, ST-013"
   "REQ-RSP-05", "Include predicted concentration time-series (part of adjustment)", "UT-SI07-006, ST-014"
   "REQ-RSP-06", "Include per-percentile concentration time-series (part of adjustment)", "UT-SI07-007, ST-014"
   "REQ-RSP-07", "Include dosage adjustment proposals with their scores", "UT-SI07-008, ST-014"


Report Generation
-----------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-RPT-01", "Produce HTML report by rendering a configurable template", "UT-SI08-002, IT-007, ST-015"
   "REQ-RPT-03", "Support output language selection via |CLI| argument or query field", "UT-SI08-006, ST-016"
   "REQ-RPT-04", "Raise an error when the specified template is not found", "UT-SI08-014, UT-SI08-015, IT-008, ST-017"
   "REQ-RPT-05", "Write |JSON| debug payload to output directory when debug flag is set", "UT-SI01-014, UT-SI08-005, ST-018"


Configuration
-------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-CFG-01", "Load runtime configuration from the XML configuration file", "UT-SI05-001, IT-001"
   "REQ-CFG-02", "Use built-in defaults for absent optional configuration parameters", "UT-SI05-001"
   "REQ-CFG-03", "Abort with exit code 3 when the configuration file is missing", "UT-SI05-002, ST-019"
   "REQ-CFG-04", "Abort with exit code 4 when the configuration file content is invalid", "UT-SI05-003, UT-SI05-004"


Interface Requirements
=======================

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-IF-CLI-01", "Expose a command-line interface", "ST-001"
   "REQ-IF-CLI-02", "Print usage message and exit when invoked with ``--help``", "UT-SI01-007, ST-006"
   "REQ-IF-CLI-03", "Write error and warning messages to stderr", "UT-SI01-015, ST-007"
   "REQ-IF-CLI-04", "Keep a log of operations performed in a text file", "UT-SI01-016"
   "REQ-IF-QRY-01", "XML query format conforms to the defined schema", "UT-SI03-001"
   "REQ-IF-QRY-02", "Support multiple drug requests within a single query file", "UT-SI03-014, IT-011"
   "REQ-IF-RSP-01", "XML response format conforms to the defined schema", "ST-013"
   "REQ-IF-RSP-02", "Response file written to the output directory specified on the |CLI|", "UT-SI08-001, IT-009"
   "REQ-IF-RSP-03", "Output file name includes the query identifier and a timestamp", "UT-SI08-003"
   "REQ-IF-DM-01", "Drug model files are XML files in the ``--drugpath`` directory", "UT-SI04-001"
   "REQ-IF-DM-02", "Scan all files in drug model directory; no subdirectory traversal required", "UT-SI04-004"
   "REQ-IF-DM-03", "Drug model format conforms to the Drug Model Language specification", "UT-SI04-001"
   "REQ-IF-RPT-01", "HTML templates reside in a subdirectory named after the template", "UT-SI08-009"
   "REQ-IF-RPT-02", "Template main file is named ``index.html``", "UT-SI08-009"
   "REQ-IF-RPT-03", "Inject |JSON| payload at the |DOM| injection point defined in the contract", "UT-SI08-010"
   "REQ-IF-RPT-04", "CSS, JavaScript, and image assets are inlined at build time", "UT-SI08-011, UT-SI08-012, UT-SI08-013"


Non-Functional Requirements
============================

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Test Case(s)"
   :widths: 16, 52, 22

   "REQ-REL-01", "No crash on any syntactically valid query", "ST-R01"
   "REQ-REL-02", "No crash on malformed XML query; exit code 4", "UT-SI03-010, ST-R02"
   "REQ-REL-03", "No crash when drug model directory is empty or has no matching model", "UT-SI04-001, UT-SI04-002, ST-R03"
   "REQ-REL-04", "No partial output files on write failure", "UT-SI08-004, ST-R04"
   "REQ-MAINT-01", "Add new drug models without recompiling", "UT-SI04-004, IT-002"
   "REQ-MAINT-02", "Add new HTML templates without recompiling", "UT-SI08-009, IT-007"
   "REQ-MAINT-03", "Extend output languages without recompiling", "UT-SI08-006, ST-016"
   "REQ-PORT-01", "Build and run on 64-bit Linux", "ST-PT01"
   "REQ-PORT-02", "Build and run on 64-bit Windows", "ST-PT02"
   "REQ-PORT-03", "Build and run on 64-bit MacOS", "ST-PT03"
   "REQ-PORT-04", "No dependency on platform-specific libraries unavailable on all targets", "ST-PT01, ST-PT02, ST-PT03"
   "REQ-PORT-05", "Platform-independent file path handling", "ST-PT04"


Safety Requirements
====================

.. csv-table::
   :header: "Safety Req. ID", "Safety Requirement Summary", "Test Case(s)"
   :widths: 13, 52, 25

   "SAF-01", "Validate patient covariate values against plausibility ranges; out-of-range raises an error", "UT-SI06-002, UT-SI06-003, ST-S01"
   "SAF-02", "Validate drug model structural integrity; missing mandatory section causes request failure", "UT-SI04-003, ST-S02"
   "SAF-03", "Process each request independently; failure of one shall not affect others", "UT-SI01-013, UT-SI07-005, IT-012, ST-S03"
   "SAF-04", "Never silently discard or truncate computation results; report data loss as error", "UT-SI07-012, ST-S04"
