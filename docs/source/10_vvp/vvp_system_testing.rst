.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpSystemTesting:

System Testing Plan
*******************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


System tests verify the assembled system against the software requirements in
the :ref:`srs`, including functional, interface, reliability, and safety
requirements. All tests use the complete :program:`tucuxi_cdss_cli` executable in
the environment.

Test case identifiers follow the pattern **ST-NNN**.


Functional System Tests
=======================

CLI and Query Processing
------------------------

.. csv-table::
   :header: "TC ID", "Requirement(s)", "Description", "Method", "Expected Result"
   :widths: 10, 18, 32, 8, 32

   "ST-001", "REQ-CLI-01..09", "Invoke with all argument combinations; verify each accepted.", "T", "Process accepts all documented arguments."
   "ST-002", "REQ-CLI-10..14", "Verify exit codes for all outcome combinations.", "T", "Exit codes match |SRS| specification."
   "ST-003", "REQ-CLI-15", "Multi-request file with one bad request: others still processed.", "T", "Correct output for valid requests; error for invalid."
   "ST-004", "REQ-QRY-10", "Submit malformed XML query file: exit code 4, error on stderr.", "T", "Exit code 4; stderr error message."
   "ST-005", "REQ-QRY-11", "Submit query missing mandatory field: request error in response.", "T + I", "Error entry in XML response."
   "ST-006", "REQ-IF-CLI-02", "Invoke with ``--help``: usage printed, exit code 0.", "T", "Usage on stdout; exit code 0."
   "ST-007", "REQ-IF-CLI-03", "Trigger an error: message appears on stderr.", "T + I", "No error output on stdout."

Computation
-----------

.. csv-table::
   :header: "TC ID", "Requirement(s)", "Description", "Method", "Expected Result"
   :widths: 10, 18, 32, 8, 32

   "ST-008", "REQ-CMP-01", "Prediction for reference patient/drug: compare against reference values.", "T + A", "Concentrations within accepted tolerance."
   "ST-009", "REQ-CMP-03", "Adjustment for achievable target: at least one proposal within range.", "T", "Proposal within target range returned."
   "ST-010", "REQ-CMP-01", "Adjust patient weight in query: verify concentration changes.", "T + A", "Different concentration profile compared to default weight."
   "ST-011", "REQ-CMP-02", "Supply observed sample: verify individual estimate differs from population.", "T + A", "Individual prediction differs from no-sample baseline."
   "ST-012", "REQ-CMP-05", "Request adjustment with unreachable target: failure reported.", "T + I", "Failure status in response; no out-of-range proposal."

Response and Reports
--------------------

.. csv-table::
   :header: "TC ID", "Requirement(s)", "Description", "Method", "Expected Result"
   :widths: 10, 18, 32, 8, 32

   "ST-013", "REQ-RSP-01..04", "XML response: schema conformance, query ID, unit labels.", "T + I", "Response validates against schema; all fields present."
   "ST-014", "REQ-RSP-05..07", "XML adjustment response: prediction, percentile, and adjustment data present.", "T + I", "Prediction, percentile, cycle, and adjustment data present in the adjustment response."
   "ST-015", "REQ-RPT-01", "HTML report generated and rendered (no broken references).", "T + I", "Valid HTML; no 404 references."
   "ST-016", "REQ-RPT-03", "Report language matches requested language code.", "T + I", "Report text in correct language."
   "ST-017", "REQ-RPT-04", "Non-existent template name: error on stderr.", "T + I", "XML file; stderr error."
   "ST-018", "REQ-RPT-05", "|JSON| output requested.", "T", "|JSON| file generated in the output directory."
   "ST-019", "REQ-CFG-03", "Missing config file: exit code 3.", "T", "Exit code 3."


Safety System Tests
===================

The following tests directly verify the safety requirements defined in the :ref:`srs`.

.. csv-table::
   :header: "TC ID", "Safety Req.", "Description", "Method", "Expected Result"
   :widths: 10, 10, 38, 8, 34

   "ST-S01", "SAF-01", "Submit covariate value outside plausibility range: report contains an explicit error.", "T + I", "Error in report."
   "ST-S02", "SAF-02", "Provide drug model with missing mandatory section: request fails with explicit error.", "T + I", "Error in response; no computation result."
   "ST-S03", "SAF-03", "First request fails; second request produces correct result.", "T + I", "Exit code 1; second request result correct."
   "ST-S04", "SAF-04", "Truncated computation result: error reported; no silent partial output.", "T + I", "Error status; no partial result data."


Reliability Tests
=================

.. csv-table::
   :header: "TC ID", "Requirement", "Description", "Method", "Expected Result"
   :widths: 10, 14, 32, 8, 36

   "ST-R01", "REQ-REL-01", "Submit 100 valid queries of varying complexity: no crash.", "T", "All queries complete; exit code 0."
   "ST-R02", "REQ-REL-02", "Submit binary file as query: exit code 4; no crash.", "T", "Exit code 4; error on stderr."
   "ST-R03", "REQ-REL-03", "Point ``--drugpath`` to empty directory: error in response; no crash.", "T", "Exit code 2; error in response."
   "ST-R04", "REQ-REL-04", "Simulate output directory write failure: no partial file left.", "T + I", "No partial files in output directory."


Portability Tests
=================

.. csv-table::
   :header: "TC ID", "Requirement", "Description", "Method", "Expected Result"
   :widths: 10, 14, 32, 8, 36

   "ST-PT01", "REQ-PORT-01", "Run full system test suite on 64-bit Linux.", "T", "All tests pass."
   "ST-PT02", "REQ-PORT-02", "Run full system test suite on 64-bit Windows.", "T", "All tests pass."
   "ST-PT03", "REQ-PORT-03", "Run full system test suite on 64-bit MacOS.", "T", "All tests pass."
   "ST-PT04", "REQ-PORT-05", "Submit query and model paths using OS-native path separators.", "T", "Process succeeds on both platforms."
