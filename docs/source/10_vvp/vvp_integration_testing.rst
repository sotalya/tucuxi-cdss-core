.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpIntegrationTesting:

Integration Testing Plan
************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Integration tests verify that software items interact correctly across their
defined interfaces, using real implementations (no stubs) assembled
incrementally.

Test case identifiers follow the pattern **IT-NNN**.

Integration Order
=================

Software items are integrated in dependency order, adding one item at a time:

.. uml::
   :caption: Integration Order

   @startuml
   skinparam defaultFontName Arial
   skinparam backgroundColor white
   skinparam shadowing false
   skinparam ArrowColor #555555
   skinparam activity {
       BackgroundColor #DDEEFF
       BorderColor #336699
       FontColor Black
   }

   start
   :Step 1 — SI-05 Configuration Loader (no dependencies);
   :Step 2 — SI-03 Query Parser (no dependencies);
   :Step 3 — SI-04 Drug Model Loader (no dependencies);
   :Step 4 — SI-06 Data Checker (+ SI-03, SI-04, SI-05);
   :Step 5 — SI-07 Adjustment Computer (+ SI-03, SI-04, SI-05, SI-06);
   :Step 6 — SI-08 Output Generator (+ SI-07);
   :Step 7 — SI-02 CDSS Flow Manager (+ SI-03..SI-08);
   :Step 8 — SI-01 CLI Controller (full pipeline);
   stop
   @enduml


SI-05 + SI-03: Configuration and Query Parsing
===============================================

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result"
   :widths: 14, 42, 8, 36

   "IT-001", "Config loaded and query parsed in sequence: both succeed.", "T", "Configurations and XpertQueryData populated."


SI-06 + SI-07: Data Checking and Adjustment Computation
=========================================================

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result"
   :widths: 14, 42, 8, 36

   "IT-002", "End-to-end adjustment: query parsed, model loaded, data checked, proposals generated.", "T", "Computation result with adjustment proposals."
   "IT-003", "Covariate from query correctly validated and applied by engine.", "T + A", "Computation result reflects covariate effect."
   "IT-004", "Observed sample from query used in Bayesian estimation.", "T + A", "Individual result differs from population result."
   "IT-005", "Drug model not found: SI-04 error propagated; request fails cleanly.", "T", "Error result; no crash."


SI-08: Output Generation with Computation Results
====================================================

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result"
   :widths: 14, 42, 8, 36

   "IT-006", "XpertRequestResult serialised to XML.", "T + I", "Valid XML file containing all result data."
   "IT-007", "XpertRequestResult serialised to |JSON| and injected into HTML template.", "T + I", "HTML file with embedded |JSON| data."
   "IT-008", "Template not found: error propagated.", "T + I", "Error on stderr."


Full Pipeline: SI-01 orchestrating all items via SI-02
=======================================================

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result"
   :widths: 14, 42, 8, 36

   "IT-009", "Single request query file: full pipeline produces XML response.", "T + I", "Exit code 0; XML response file present."
   "IT-010", "Single request query file: full pipeline produces HTML report.", "T + I", "Exit code 0; HTML file present."
   "IT-011", "Multi-request query file: all requests processed; one response per request.", "T + I", "Exit code 0; correct number of output files."
   "IT-012", "Multi-request query file: one malformed request does not abort others.", "T + I", "Exit code 1; other requests produce output."
   "IT-013", "Query file with unresolvable drug ID: request fails; others succeed.", "T + I", "Exit code 1; error in response for failed request."
   "IT-014", "Error messages for all failures written to stderr.", "T + I", "stderr contains error entries; stdout clean."
