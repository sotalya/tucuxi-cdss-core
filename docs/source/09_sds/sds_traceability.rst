.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _sdsTraceability:

Traceability
************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


This section provides the traceability between software requirements (from the
:ref:`srs`) and the software design elements defined in this document.


Requirements to Software Items
================================

CLI and Process Control
-----------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-CLI-01", "Accept drug model directory as mandatory argument", "SI-01, SI-04"
   "REQ-CLI-02", "Accept XML query file as mandatory argument", "SI-01, SI-03"
   "REQ-CLI-03", "Accept configuration file as an optional argument", "SI-01, SI-05"
   "REQ-CLI-04", "Accept output directory as mandatory argument", "SI-01, SI-08"
   "REQ-CLI-05", "Accept optional HTML template directory argument", "SI-01, SI-08"
   "REQ-CLI-06", "Accept mandatory HTML template name argument", "SI-01, SI-08"
   "REQ-CLI-07", "Accept optional language path argument", "SI-01, SI-08"
   "REQ-CLI-08", "Accept optional language code argument", "SI-01, SI-08"
   "REQ-CLI-09", "Accept optional |JSON| debug dump flag", "SI-01, SI-08"
   "REQ-CLI-10", "Return exit code 0 when all requests succeed", "SI-01"
   "REQ-CLI-11", "Return exit code 1 when some requests succeed", "SI-01"
   "REQ-CLI-12", "Return exit code 2 when no request succeeds", "SI-01"
   "REQ-CLI-13", "Return exit code 4 when query file cannot be loaded", "SI-01"
   "REQ-CLI-14", "Return exit code 3 when bad arguments are given", "SI-01"
   "REQ-CLI-15", "Process all requests independently", "SI-01, SI-02"

Query Processing
----------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-QRY-01", "Parse patient admin data from query", "SI-03"
   "REQ-QRY-02", "Parse mandator admin data from query", "SI-03"
   "REQ-QRY-03", "Parse patient covariates from query", "SI-03"
   "REQ-QRY-04", "Validate patient covariates from query against configuration file", "SI-06, SI-05"
   "REQ-QRY-05", "Missing covariates shall result in a warning and default value used", "SI-06, SI-04"
   "REQ-QRY-06", "Parse complete dosage history from query", "SI-03"
   "REQ-QRY-07", "Parse observed drug concentration samples from query", "SI-03"
   "REQ-QRY-08", "Parse drug identifier from query", "SI-03"
   "REQ-QRY-09", "Parse adjustment request and parameters", "SI-03"
   "REQ-QRY-10", "Reject malformed XML query with error", "SI-03"
   "REQ-QRY-11", "Reject query missing mandatory fields with error", "SI-03"
   "REQ-QRY-12", "Reject query with inconsistent date/time values", "SI-06"

Drug Model Loading
------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-DM-01", "Load drug model matching drug identifier", "SI-04"
   "REQ-DM-02", "Validate drug model structure on load", "SI-04"
   "REQ-DM-03", "Report error if no matching drug model found", "SI-04"
   "REQ-DM-04", "Report error and skip if drug model file is malformed", "SI-04"

PK/PD Computation
-----------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-CMP-01", "Apply covariate effects in |PK| computation", "SI-07"
   "REQ-CMP-02", "Incorporate observed samples (Bayesian estimation)", "SI-07"
   "REQ-CMP-03", "Evaluate adjustment candidates against target type and range", "SI-07"
   "REQ-CMP-04", "Report computation error without aborting other requests", "SI-07, SI-01"
   "REQ-CMP-05", "Report failure when no adjustment candidate in target range", "SI-07"

Response Generation
-------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-RSP-01", "Produce XML response file when requested", "SI-08"
   "REQ-RSP-02", "Include status per request in response", "SI-08"
   "REQ-RSP-03", "Include a query identifier in all responses", "SI-08"
   "REQ-RSP-04", "Include unit labels on all numeric output values", "SI-08"
   "REQ-RSP-05", "Include predicted concentration time-series (part of adjustment)", "SI-07, SI-08"
   "REQ-RSP-06", "Include per-percentile concentration time-series (part of adjustment)", "SI-07, SI-08"
   "REQ-RSP-07", "Include dosage adjustment proposals with scores", "SI-07, SI-08"

Report Generation
-----------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-RPT-01", "Produce HTML report via configurable template", "SI-08"
   "REQ-RPT-03", "Support multiple output languages", "SI-08, SI-01"
   "REQ-RPT-04", "Raise an error if template not found", "SI-01, SI-08"
   "REQ-RPT-05", "Write |JSON| debug payload when debug flag is set", "SI-08, SI-01"

Configuration
-------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 16, 50, 24

   "REQ-CFG-01", "Load runtime configuration from XML file", "SI-05"
   "REQ-CFG-02", "Use built-in defaults for optional parameters", "SI-05"
   "REQ-CFG-03", "Abort with exit code 3 if configuration file missing", "SI-05, SI-01"
   "REQ-CFG-04", "Abort with exit code 4 if configuration file invalid", "SI-05, SI-01"

Interface Requirements
----------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 18, 50, 24

   "REQ-IF-CLI-01", "Expose |CLI| interface", "SI-01"
   "REQ-IF-CLI-02", "Print usage message and exit on --help flag", "SI-01"
   "REQ-IF-CLI-03", "Write errors and warnings to stderr", "SI-01"
   "REQ-IF-CLI-04", "Keep a log of the operations performed", "SI-01"
   "REQ-IF-QRY-01", "XML query format conforms to :ref:`data-formats`", "SI-03"
   "REQ-IF-QRY-02", "Support multiple drug requests in single query file", "SI-03"
   "REQ-IF-RSP-01", "XML response format conforms to :ref:`data-formats`", "SI-08"
   "REQ-IF-RSP-02", "Response file written to output directory", "SI-08"
   "REQ-IF-RSP-03", "Output file name includes query ID and timestamp", "SI-08"
   "REQ-IF-DM-01", "Drug model files are XML in the --drugpath directory", "SI-04"
   "REQ-IF-DM-02", "Scan all files in drug model directory (no subdirectory traversal)", "SI-04"
   "REQ-IF-DM-03", "Drug model format conforms to Drug Model Language specification", "SI-04"
   "REQ-IF-RPT-01", "HTML templates reside in a subdirectory named after the template", "SI-08"
   "REQ-IF-RPT-02", "Template main file is named ``index.html``", "SI-08"
   "REQ-IF-RPT-03", "Inject |JSON| payload at |DOM| injection point", "SI-08"
   "REQ-IF-RPT-04", "CSS, JS, and image assets inlined at build time", "SI-08"

Non-Functional Requirements
----------------------------

.. csv-table::
   :header: "Requirement ID", "Requirement Summary", "Software Item(s)"
   :widths: 18, 50, 24

   "REQ-REL-01", "No crash on any syntactically valid query", "SI-01, SI-03, SI-04, SI-07"
   "REQ-REL-02", "No crash on malformed XML query; exit code 4", "SI-01, SI-03"
   "REQ-REL-03", "No crash when drug model directory empty or no matching model", "SI-01, SI-04"
   "REQ-REL-04", "No partial output files on write failure", "SI-08"
   "REQ-MAINT-01", "Add new drug models without recompiling", "SI-04"
   "REQ-MAINT-02", "Add new HTML templates without recompiling", "SI-08"
   "REQ-MAINT-03", "Extend output languages without recompiling", "SI-08"
   "REQ-PORT-01", "Build and run on 64-bit Linux", "SI-01 ... SI-08"
   "REQ-PORT-02", "Build and run on 64-bit Windows", "SI-01 ... SI-08"
   "REQ-PORT-03", "Build and run on 64-bit MacOS", "SI-01 ... SI-08"
   "REQ-PORT-04", "No dependency on platform-specific libraries", "SI-01 ... SI-08"
   "REQ-PORT-05", "Platform-independent file path handling", "SI-01, SI-04, SI-08"


Requirements to Design Sections
=================================

.. csv-table::
   :header: "Requirement Group", "Design Section"
   :widths: 30, 60

   "REQ-CLI-01 ... REQ-CLI-15", ":ref:`sdsDetailedDesign` — SI-01 CLI Controller"
   "REQ-QRY-01 ... REQ-QRY-12", ":ref:`sdsDetailedDesign` — SI-03 Query Parser, SI-06 Data Checker; :ref:`sdsInterfaceDesign` — XML Query Interface"
   "REQ-DM-01 ... REQ-DM-04", ":ref:`sdsDetailedDesign` — SI-04 Drug Model Loader; :ref:`sdsInterfaceDesign` — Drug Model File Interface"
   "REQ-CMP-01 ... REQ-CMP-05", ":ref:`sdsDetailedDesign` — SI-07 Adjustment Computer"
   "REQ-RSP-01 ... REQ-RSP-07", ":ref:`sdsDetailedDesign` — SI-07 Adjustment Computer, SI-08 Output Generator; :ref:`sdsInterfaceDesign` — XML Response Interface"
   "REQ-RPT-01 ... REQ-RPT-05", ":ref:`sdsDetailedDesign` — SI-08 Output Generator; :ref:`sdsInterfaceDesign` — HTML Report Interface"
   "REQ-CFG-01 ... REQ-CFG-04", ":ref:`sdsDetailedDesign` — SI-05 Configuration Loader"
   "REQ-IF-CLI-01 ... REQ-IF-CLI-04", ":ref:`sdsInterfaceDesign` — CLI Interface"
   "REQ-IF-QRY-01 ... REQ-IF-QRY-02", ":ref:`sdsInterfaceDesign` — XML Query Interface"
   "REQ-IF-RSP-01 ... REQ-IF-RSP-03", ":ref:`sdsInterfaceDesign` — XML Response Interface"
   "REQ-IF-DM-01 ... REQ-IF-DM-03", ":ref:`sdsInterfaceDesign` — Drug Model File Interface"
   "REQ-IF-RPT-01 ... REQ-IF-RPT-04", ":ref:`sdsInterfaceDesign` — HTML Report Interface"
   "REQ-REL-01 ... REQ-REL-04", ":ref:`sdsDetailedDesign` — SI-01, SI-03, SI-04, SI-08"
   "REQ-MAINT-01 ... REQ-MAINT-03", ":ref:`sdsDetailedDesign` — SI-04, SI-08"
   "REQ-PORT-01 ... REQ-PORT-05", ":ref:`sdsArchitecture` — Architectural Overview"


Safety Requirements Traceability
==================================

.. csv-table::
   :header: "Safety Req. ID", "Safety Requirement Summary", "Software Item(s)", "Design Section"
   :widths: 13, 42, 18, 27

   "SAF-01", "Validate patient covariate values against plausibility ranges; out-of-range raises an error", "SI-06, SI-04, SI-05", ":ref:`sdsSafetyDesign` — Input Validation"
   "SAF-02", "Validate drug model structural integrity; missing mandatory section causes request failure", "SI-04", ":ref:`sdsSafetyDesign` — Input Validation"
   "SAF-03", "Process each request independently; failure of one shall not affect others", "SI-01, SI-02", ":ref:`sdsSafetyDesign` — Defensive Error Propagation"
   "SAF-04", "Never silently discard or truncate computation results; any data loss is reported as an error", "SI-08", ":ref:`sdsSafetyDesign` — Output Traceability"
