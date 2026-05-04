.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _sdsSafetyDesign:

Safety and Risk Control Design
*******************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Software Safety Class
=====================

If a certification were to be performed, Tucuxi-CDSS would be classified as
**Software Safety Class B** (|IEC62304| §4.3):
software failure could contribute to a hazardous situation that results in
unacceptable risk to the patient, but death or serious injury is not the probable
outcome.


Safety-Relevant Software Items
================================

The following software items have been identified as safety-relevant and require
heightened verification effort (see :ref:`sdsArchitecture` — Safety Requirements
Allocation):

.. csv-table::
   :header: "Software Item", "Hazard Contribution", "Risk Control Measure in Design"
   :widths: 20, 40, 40

   "SI-03 Query Parser", "Incorrect patient data (covariates, dosage history) leads to wrong |PK| computation.", "Strict schema validation; explicit error on missing mandatory fields."
   "SI-04 Drug Model Loader", "Wrong or corrupted drug model produces incorrect |PK| results.", "Model identifier matching; schema validation; detection of missing mandatory sections."
   "SI-06 Data Checker", "Undetected data errors (covariate out-of-range, inconsistent dates) propagate to computation.", "Range checks on covariate values; dose and sample plausibility checks; date consistency checks."
   "SI-07 Adjustment Computer", "Numerical error or wrong algorithm leads to an incorrect dosage recommendation.", "Unit tests covering boundary conditions; comparison against reference results; propagation of uncertainty."
   "SI-08 Output Generator", "Misrepresentation of results in the response (wrong units, truncation).", "Unit conversion validation; explicit type mapping from internal to XML schema."


Risk Control Measures Implemented in Design
============================================

Input Validation (SI-03, SI-04, SI-05, SI-06)
--------------------------------------------------

- All XML inputs are validated against their respective schemas before use.
- Out-of-range covariate values trigger a warning or error rather than silent
  clamping to avoid masking data entry errors.
- A missing mandatory field causes the affected request to fail with an explicit
  error message; it does not silently use a default.

Computation Integrity (SI-07)
------------------------------

- If the computation engine returns no candidate adjustment, the system records
  an error (``No adjustment found``) and continues to generate the report with
  this error included in the output.

Output Traceability (SI-08)
-----------------------------

- Unit labels are included in all numeric output fields.
- Error responses include a human-readable message describing the failure cause.

Defensive Error Propagation (SI-01)
-------------------------------------

- The |CLI| controller treats each request independently; a failure in one request
  does not abort processing of subsequent requests in the same query file.
- The exit code distinguishes between partial success and total failure, allowing
  the calling system to detect and handle errors appropriately.


Verification of Safety Design
==============================

.. csv-table::
   :header: "Verification Item", "Method"
   :widths: 60, 30

   "Input validation rejects malformed XML", "Unit test"
   "Out-of-range covariate values are flagged", "Unit test"
   "Out-of-range covariate values are reported in the generated reports", "System test"
   "Missing drug model causes request failure (not crash)", "Integration test"
   "Exit code correctly reflects partial vs. total failure", "Integration test"
