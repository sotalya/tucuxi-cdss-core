.. SPDX-License-Identifier: AGPL-3.0-or-later

Architecture and Design
#######################

Tucuxi-CDSS is a modular system organized into three processing layers,
as illustrated in the data-flow description below.  The layers are
deliberately separated so that each can be independently tested and
configured.

Data Flow
*********

The end-to-end data flow is as follows:

.. uml::

    @startuml
    start
    partition "Layer 1 — Validation & Preprocessing\n(CDSS + Tucuxi-Core for plausibility)" #LightBlue {
      :Patient data (XML query);
      :covariate checks;
      :dose range checks;
      :sample percentile checks;
      :date consistency checks;
      :target consistency checks;
    }
    :validated data;
    partition "Layer 2 — Model-based Computation\n(Tucuxi-Core)" #LightGreen {
      :MAP parameter estimation;
      :concentration prediction;
      :candidate adjustments;
    }
    :candidate exposures;
    partition "Layer 3 — Scoring, Justification & Reporting\n(CDSS-specific)" #LightCoral {
      :adjustment scoring;
      :justification assembly;
      :report rendering (HTML);
    }
    stop
    @enduml

Processing Layers
*****************

Layer 1 — Data Validation and Preprocessing
============================================

All patient inputs undergo a two-stage validation before reaching the
computational engine.

*Stage 1 — General consistency checks* flag errors that are
drug-independent:

.. csv-table:: Data validation checks
   :header: "Check", "Error type", "Description"
   :widths: 20, 20, 60

   "Covariate values",    "data entry / model",    "Values must fall within the soft and hard boundaries defined in the drug model file.  A soft-boundary violation issues a warning; a hard-boundary violation raises an error."
   "Missing covariate",   "model",                 "Missing values are replaced by the model-specified population default and flagged with a warning."
   "Valid doses",         "data entry / model",    "Dose amounts must lie within the range of doses supported by the selected drug model."
   "Sample dates",        "data entry / procedure","Sample dates must be chronologically plausible: not before treatment start, and no more than five half-lives after the last administered dose.  Samples during the absorption phase also generate a warning."
   "Therapeutic targets", "data entry",            "Multiple targets supplied for a single patient must be mutually consistent (e.g., two trough targets with different values are flagged)."

*Stage 2 — Drug-specific plausibility checks* use the |popPK| model
(via the Tucuxi-Core engine :cite:`TucuxiCore2025`) to assess the
statistical plausibility of measured concentrations.  Observed
concentrations are compared to the percentiles predicted for a typical
patient under the current regimen:

- within the 10th-90th percentile: accepted without warning;
- outside the 10th-90th percentile: warning issued.

Layer 2 — Model-Based Computation (Tucuxi-Core)
=================================================

Validated data are forwarded to the Tucuxi-Core engine
:cite:`TucuxiCore2025`, which performs all pharmacometric computations.
This layer is entirely delegated to Tucuxi-Core and is independent of
any |CDSS|-specific logic:

- **MAP estimation**: the most likely individual |PK| parameters are
  estimated by combining the |popPK| prior with the patient's observed
  concentrations via Bayesian updating :cite:`Guidi2022`.
- **Concentration prediction**: the current and candidate regimens are
  evaluated over the forecast horizon.
- **Candidate generation**: all combinations of allowed dose values and
  dosing intervals are evaluated; only those that deliver predicted
  exposure within the therapeutic target range are retained.
- **Transient modulation**: for each retained candidate, a one-dose
  transient modulation is evaluated to assess whether it accelerates
  convergence to the target.
- **Goodness-of-fit statistics** can optionally be exposed to allow
  users to assess the quality of the model fit to the observed
  data.

Layer 3 — Scoring, Justification, and Reporting
=================================================

This layer contains the logic that is specific to the |CDSS| and is not
part of Tucuxi-Core.  It takes the candidate adjustments produced by
Layer 2 and turns them into a report.

Among the retained candidates, the one with the highest *adjustment
quality score* is selected.  The score is defined as:

.. math::

   S = 1 - \left(\frac{\ln(\hat{C} / C^\star)}{\ln(C_{\max} / C_{\min}) / 2}\right)^2

where :math:`\hat{C}` is the predicted exposure under the candidate
adjustment, :math:`C^\star` is the best-point target, and
:math:`C_{\max}` / :math:`C_{\min}` are the upper and lower bounds of
the therapeutic interval.  This is analogous to the :math:`R^2` statistic: a
score of 1 indicates perfect alignment with the target.

A clinical justification is assembled from a library of standard
sentences (configurable per language and institution) and inserted into
the institutional HTML template alongside the computed metrics and any
warnings raised in Layer 1.

Integration Modes
*****************

Because Tucuxi-CDSS is implemented as a C++ |CLI| tool, it
supports multiple integration patterns:

- **Standalone use**: the :program:`tucuxi_cdss_cli` binary is invoked
  directly from a shell or via a calling script.
- **Web application backend**: the binary is called by a web server
  process; HTML reports are returned directly to the browser.
- **Tucuxi GUI plugin**: the Tucuxi graphical interface can invoke the
  |CDSS| to complement its interactive dosing features.
- **Compiled library**: the core library can be linked into an existing
  C++ application, removing the subprocess overhead and enabling
  tighter integration with information systems.
