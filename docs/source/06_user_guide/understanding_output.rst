.. SPDX-License-Identifier: AGPL-3.0-or-later

Understanding the Output
========================

Both the XML and HTML outputs contain the following sections:

Dosage adjustment proposal
   The primary result: the recommended dosing regimen (dose amount, interval, and
   optional loading dose).

Justification
   A set of standard sentences, translated to the configured language, explaining
   the rationale behind the recommendation (e.g., "dose increase to reach the
   target trough concentration").

Validation warnings and errors
   Any issues detected during data validation (covariate out of range,
   implausible sample, date inconsistency, etc.) are reported with a severity
   level. **Warnings** indicate potential data quality issues but do not prevent
   the computation. **Errors** flag conditions that prevent reliable computation.

Pharmacokinetic parameters
   Population (typical), a priori, and a posteriori (individual) parameter
   estimates. These are provided for context and quality-of-fit assessment.

Predicted concentrations and percentiles
   Concentration time-series and population percentile curves are included to
   visualize the expected drug exposure under the recommended regimen. These are
   by-products of the adjustment computation, not standalone outputs.
