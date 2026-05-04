.. SPDX-License-Identifier: AGPL-3.0-or-later

Overview
########

Functional Description
**********************

Tucuxi-CDSS is an open-source, cross-platform |CDSS| that acts
as a control and validation layer on top of the Tucuxi-Core
computational engine :cite:`TucuxiCore2025`.  Given patient-specific
information and a drug model, the system:

1. imports and validates all input data (doses, samples, covariates,
   targets);
2. forwards validated data to Tucuxi-Core for Bayesian computation;
3. interprets the computed exposure metrics against configured
   therapeutic targets;
4. selects the highest-scoring dosage adjustment candidate;
5. assembles a justified recommendation and renders a report.

The system is implemented in C++ and exposed as a command-line tool
(|CLI|), making it straightforward to integrate into pre-existing
workflows.  It can be called from a web application (returning
HTML reports directly), invoked by a graphical front-end such as the
Tucuxi GUI, or linked as an external C++ library.

Supported Drugs
***************

Drug-specific behaviour is fully defined by |XML| drug model files
:cite:`Ravix2025`.  Each model specifies:

- the |popPK| model parameters and inter/intra-individual variability;
- the patient covariates required by the model (with soft and hard
  plausibility thresholds);
- the allowed dose values and dosing intervals;
- the default therapeutic targets (which can be overridden per patient).

Two drug contexts have already been validated in the published literature:

**Imatinib** (chronic myeloid leukaemia) :cite:`vanderKleij2023,Goutelle2023`.
   Used as the
   primary use case for system testing and for the companion in-silico
   performance evaluation :cite:`Petermann2026`.  The |popPK| model of
   Gotta et al. is supported :cite:`Gotta2012`.

**Rifampicin** (tuberculosis)
   Non-linear pharmacokinetics, for which the standard proportional
   "rule-of-three" dose adjustment is inaccurate :cite:`Petermann2024`.
   Deployed in routine care in Tanzania under the TuberXpert project
   :cite:`Thoma2024`.

However, a large number of drugs is supported --- since the drug model file
format is shared with the standalone Tucuxi application, any model valid for
Tucuxi can be used with Tucuxi-CDSS.
For instance, for Vancomycin (a glycopeptide antibiotic administered by intravenous
infusion to treat severe infections), multiple |popPK| models are available,
covering patient populations from premature neonates to adults.

More information on the available drug models and how further drugs could be
added is available `here <https://www.tucuxi.ch/drugmodels/>`__.

Workflow
********

The operational workflow comprises three sequential phases.

Data import and validation
==========================

All patient data are supplied in an |XML| query file. The system
performs the following checks before any computation is attempted (see
also :doc:`../03_architecture/architecture` for details on each check):

- **Covariate plausibility**: values are checked against soft and hard
  thresholds defined in the drug model file and in the configuration file.
  Soft violations raise a warning; hard violations raise an error.
  Missing covariates are replaced by model-specified defaults and flagged.
- **Dose validity**: dose values must lie within the ranges supported
  by the selected model.
- **Sample plausibility**: observed concentrations are compared to the
  10th-90th percentile range predicted by the |popPK| model.  Values
  outside the 10th-90th percentile range trigger a warning.
- **Date consistency**: sample and dose dates are checked to be
  chronologically plausible (sample not before treatment start, not
  more than five half-lives after the last dose).
- **Target non-contradiction**: multiple therapeutic targets supplied
  for a single patient must not be mutually inconsistent.

Bayesian computation
====================

Validated data are gathered and then Tucuxi-Core is invoked to perform Maximum A
Posteriori (|MAP|) parameter estimation and to compute predicted
concentration profiles for the current regimen and for a set of
candidate adjustments :cite:`TucuxiCore2025`.  These concentration
predictions are an intermediate step of the adjustment computation and
are not independently requestable.  Candidates are filtered
to retain only those that bring the predicted exposure within the
target range; a transient modulation of the next dose is evaluated if
it would improve convergence.

Adjustment scoring and report generation
=========================================

Each candidate adjustment receives a score that quantifies its squared
relative departure from the best therapeutic target, normalized by the
half-width of the target interval (expressed on a logarithmic scale).
The highest-scoring candidate is selected.

A clinical justification is assembled from a set of standard sentences
(translated according to the configured language) and inserted into the
chosen institutional HTML template.  The report is written to the
configured output directory as an HTML or |XML| file.
