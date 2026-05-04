.. SPDX-License-Identifier: AGPL-3.0-or-later

Introduction
############

Purpose of This Document
************************

This documentation covers the architecture, configuration, deployment,
and operational use of **Tucuxi-CDSS**, an open-source Clinical Decision
Support System (|CDSS|) designed to operationalize Model-Informed Precision
Dosing (|MIPD|).

It serves as the primary reference for system integrators, clinical
pharmacologists, and pharmacists who deploy or configure the system,
and for software developers who integrate it into their applications.

Intended Audience
*****************

This document is addressed to:

- **Clinical pharmacologists and pharmacists** responsible for deploying
  and configuring the system within an institution, including the
  selection and tuning of drug models, therapeutic target ranges, and
  plausibility thresholds.
- **Software engineers and IT staff** integrating Tucuxi-CDSS
  as a backend service within existing information systems or
  web applications.
- **Researchers and developers** extending the system with new |popPK|
  drug models, language files, or reporting templates.

Clinical Background
*******************

Therapeutic Drug Monitoring (TDM)
==================================

|TDM| supports individualized pharmacotherapy by using measured
drug concentrations — typically in blood or plasma — to tailor dosing
to the individual patient :cite:`Buclin2020,Kang2009`. By relating
observed concentrations to therapeutic targets derived from
Pharmacokinetic-Pharmacodynamic (|PKPD|) relationships,
practitioners can secure efficacy while limiting toxicity across a
wide range of medical fields including oncology, infectious diseases,
immunology, and psychiatry.

Traditional |TDM| is subject to significant operational limitations.
Conventional approaches rely on sparse sampling at fixed steady-state
conditions (most often trough levels) and derive dose adjustments from
simple proportional rules :cite:`Petermann2024`. This "rule-of-three"
law is unsuited to drugs with non-linear kinetics, where proportional
dose changes can produce disproportionate shifts in exposure.
Furthermore, the |TDM| workflow is fragmented across sample collection,
transport, and laboratory analysis, making it highly vulnerable to
pre-analytical errors :cite:`PeckPalmer2021`:

- Up to 60% of drug concentrations are drawn at inappropriate sampling
  times.
- Between 22% and 31% of samples are collected before steady state is
  reached.
- Omission of precise dosing histories and incomplete patient records
  further undermines the reliability of resulting recommendations.

Model-Informed Precision Dosing (MIPD)
=======================================

|MIPD| addresses the limitations of traditional |TDM| by combining
population pharmacokinetic (|popPK|) models with Bayesian forecasting
:cite:`Minichmayr2024,Guidi2022`. |popPK| models characterize the
typical pharmacokinetic behaviour of a drug within a reference
population while quantifying inter- and intra-individual variability
through patient-specific covariates (age, body weight, etc.).

Bayesian *a posteriori* estimation integrates observed drug
concentrations with the population prior to yield Maximum A Posteriori
(|MAP|) estimates of individual |PK| parameters. This approach
removes the requirement for strict steady-state sampling conditions and
accommodates non-linear or time-varying kinetics :cite:`Guidi2022`.

A critical risk of |MIPD| is its sensitivity to erroneous input data.
Incorrect timestamps, implausible concentration measurements, or
incomplete dosing records may prevent the mathematical framework from
converging, or — in the worst case — produce inappropriate dosing
recommendations ("garbage in, garbage out") :cite:`PeckPalmer2021`.
In settings without |TDM| specialist oversight, this risk underscores
the need for an automated data-validation layer upstream of any
computational engine.

Clinical Decision Support Systems (CDSS)
=========================================

A |CDSS| provides the architectural layer that bridges |MIPD|
computation and safe clinical use :cite:`Sutton2020,Bates2003`. Rather
than merely executing pharmacokinetic calculations, a |CDSS| actively
scrutinizes inputs — sampling timestamps, dosing history, measured
concentrations — before forwarding them to the computational engine,
and translates numerical outputs into standardized, actionable
recommendations.

Key design goals for an effective |CDSS| include
:cite:`Middleton2016,Bates2003`:

- detecting and flagging data entry and pre-analytical errors before
  they propagate to the dosing recommendation;
- minimizing "alert fatigue" by calibrating warning thresholds to
  clinical relevance;
- delivering clear, institution-specific output reports that can be
  acted upon without specialist pharmacological expertise.

Tucuxi-CDSS fulfills these goals by wrapping the Tucuxi-Core
computational engine :cite:`TucuxiCore2025` with a configurable
validation and decision layer. It was originally developed for the
**TuberXpert** project, which targets rifampicin dose optimization for
tuberculosis treatment in resource-limited settings in Tanzania
:cite:`Thoma2024,Petermann2024`. It has since been applied to
imatinib monitoring in the context of decentralized, Point-of-Care
(|POC|) biosensor-based oncology workflows :cite:`Goutelle2023`.

Related Documents
*****************

The following documents provide complementary information:

- **Companion performance paper**: an in-silico safety and performance
  evaluation of Tucuxi-CDSS against a human |TDM| expert for imatinib
  :cite:`Petermann2026`.
- **TuberXpert protocol**: the clinical implementation study in
  Tanzania :cite:`Thoma2024`.
- **Tucuxi-Core**: the underlying open-source computing engine for |MIPD|
  :cite:`TucuxiCore2025`.
- **Numerical verification of Tucuxi**: independent numerical
  verification of the Bayesian adaptation results :cite:`Ravix2025`.
- **Source code**: :cite:`TucuxiCDSSGitHub`.
