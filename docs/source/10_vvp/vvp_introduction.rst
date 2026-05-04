.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpIntroduction:

Introduction
************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Purpose
=======

This chapter presents the Verification and Validation Plan (VVP) for Tucuxi-CDSS.
It defines how the software will
be verified against its design (verification) and validated against its
requirements and intended use (validation), in accordance with |IEC62304| §5.5,
§5.6, and §5.7 (though Tucuxi-CDSS is not yet meant to be |IEC62304| compliant).

This document is intended for:

- Software developers writing and executing unit tests
- Integration and system testers
- Quality assurance engineers assessing |IEC62304| compliance


Scope
=====

This plan covers verification and validation of Tucuxi-CDSS,
including all software items SI-01 through SI-08 as defined in [SDS]:

- SI-01: |CLI| Controller
- SI-02: |CDSS| Flow Manager
- SI-03: Query Parser
- SI-04: Drug Model Loader
- SI-05: Configuration Loader
- SI-06: Data Checker
- SI-07: Adjustment Computer
- SI-08: Output Generator

The following are out of scope:

- Validation of drug model pharmacological content (covered by model validation)
- Performance benchmarking


Document Overview
=================

- :ref:`vvpOverview` — |VaV| strategy, levels, methods, pass/fail criteria.
- :ref:`vvpUnitTesting` — Unit test plan for each software item (|IEC62304| §5.5).
- :ref:`vvpIntegrationTesting` — Integration test plan for software item
  interactions (|IEC62304| §5.6).
- :ref:`vvpSystemTesting` — System test plan against the full set of requirements
  (|IEC62304| §5.7).
- :ref:`vvpToolsEnvironment` — Test tools, frameworks, and environment
  configuration.
- :ref:`vvpTraceability` — Traceability matrix from requirements to test cases.
