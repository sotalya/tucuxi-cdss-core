.. SPDX-License-Identifier: AGPL-3.0-or-later

Purpose and Scope
=================

Tucuxi-CDSS is a Clinical Decision Support System whose computational output is a
dosage adjustment proposal. Given a patient's dosing history, measured drug
concentrations, covariates, and therapeutic targets, the system identifies the
dosing regimen that best meets the target and produces a justified
recommendation.

The system **cannot** be used to:

- compute standalone concentration predictions or time-series;
- run percentile simulations outside the context of an adjustment;
- perform pharmacokinetic parameter estimation as an end in itself.

Concentration predictions, percentile curves, and pharmacokinetic parameter
estimates that appear in the output are computed internally as part of the
adjustment algorithm and are included to provide context for the recommendation
--- they are not independently requestable.
