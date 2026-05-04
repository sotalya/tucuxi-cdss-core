.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _sdsArchitecture:

Software Architectural Design
******************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Architectural Overview
======================

Tucuxi-CDSS is designed according to a **pipeline architecture**: an incoming XML query is
parsed, processed by the computation engine, and transformed into one or more output
formats (XML, HTML). The system is structured as a set of loosely coupled
software items (|SI|) that communicate through well-defined data structures.

.. uml::
   :caption: Tucuxi-CDSS - Software Architecture Overview

   @startuml
   skinparam defaultFontName Arial
   skinparam backgroundColor white
   skinparam shadowing false
   skinparam ArrowColor #555555

   skinparam component {
       BackgroundColor #DDEEFF
       BorderColor #336699
       FontColor Black
   }
   skinparam file {
       BackgroundColor #F5F5F5
       BorderColor #999999
   }
   skinparam folder {
       BackgroundColor #F0FFF0
       BorderColor #228B22
   }
   skinparam database {
       BackgroundColor #FFF8DC
       BorderColor #B8860B
   }

   package "tucuxi_cdss_cli" {
       component "SI-01\nCLI Controller" as SI01
       component "SI-02\nCDSS Flow Manager" as SI02
       component "SI-03\nQuery Parser" as SI03
       component "SI-04\nDrug Model Loader" as SI04
       component "SI-05\nConfiguration Loader" as SI05
       component "SI-06\nData Checker" as SI06
       component "SI-07\nAdjustments Computer" as SI07
       component "SI-08\nOutput Generator" as SI08
   }

   SI01 -right-> SI02

   SI03 .down.> SI04
   SI04 .down.> SI05
   SI05 .down.> SI06
   SI06 .down.> SI07
   SI07 .down.> SI08

   SI02 --> SI03
   SI02 --> SI04
   SI02 --> SI05
   SI02 --> SI06
   SI02 --> SI07
   SI02 --> SI08

   @enduml


Solid arrows represent orchestration dependencies (SI-02 invokes each SI);
dashed arrows represent the pipeline execution order through which data flows
sequentially from one step to the next.

Software Items
==============

The system is decomposed into the following software items:

.. csv-table::
   :header: "ID", "Software Item", "Responsibility"
   :widths: 10, 15, 65

   "SI-01", "|CLI| Controller", "Entry point; parses command-line arguments and orchestrates the pipeline."
   "SI-02", "|CDSS| Flow Manager", "Orchestrates the calls to the different functional blocks."
   "SI-03", "Query Parser", "Reads and validates the input XML query; populates internal data structures."
   "SI-04", "Drug Model Loader", "Locates, reads, and validates drug model files."
   "SI-05", "Configuration Loader", "Loads the configuration options."
   "SI-06", "Data Checker", "Checks that the different elements (doses, samples, dates, targets) are correct and coherent."
   "SI-07", "Adjustment Computer", "Performs the adjustment computation."
   "SI-08", "Output Generator", "Renders the HTML report by injecting |JSON| data into an HTML template or creates the XML output (according to the user request)."


Software Item Interfaces
========================

SI-01 — CLI Controller
----------------------

- **Input**: command-line arguments (see :ref:`sdsInterfaceDesign`)
- **Output**: return code in (-2, 2)
- **Depends on**: SI-02

SI-02 — CDSS Flow Manager
-------------------------

- **Input**: XML query file path, configuration file path
- **Output**: populated ``XpertRequestResult`` for each request (passed by reference)
- **Depends on**: SI-03, SI-04, SI-05, SI-06, SI-07, SI-08

SI-03 — Query Parser
--------------------

- **Input**: XML query file path
- **Output**: internal ``XpertQueryData`` data structure; for each request a
  ``DrugTreatment`` (Tucuxi-Core) is extracted via ``XpertQueryToCoreExtractor``
- **Depends on**: (none)

SI-04 — Drug Model Loader
-------------------------

- **Input**: drug model directory path, drug identifier from query
- **Output**: internal ``DrugModel`` data structure
- **Depends on**: (none)

SI-05 — Configuration Loader
----------------------------

- **Input**: XML configuration file path
- **Output**: ``Configurations`` data structure
- **Depends on**: (none)

SI-06 — Data Checker
-------------------------

- **Input**: ``XpertRequestResult`` that gathers the information from SI-03 to SI-05
- **Output**: validation results (``CovariateValidationResult``, ``DoseValidationResult``, ``SampleValidationResult``, ``DoseDateValidationResult``, ``SampleDateValidationResult``) stored in the ``XpertRequestResult``
- **Depends on**: SI-03, SI-04, SI-05

SI-07 — Adjustment Computer
---------------------------

- **Input**: ``XpertRequestResult`` that gathers the information from SI-03 to SI-06
- **Output**: updated internal ``XpertRequestResult`` structure (containing
  ``AdjustmentData``, parameters, cycle statistics, and justification).
  Internally a ``ComputingRequest`` (pairing the ``DrugModel``, ``DrugTreatment``,
  and a ``ComputingTraitAdjustment``) is submitted to the Tucuxi-Core
  ``ComputingComponent``, which returns a ``ComputingResponse``.
- **Depends on**: SI-03, SI-04, SI-05, SI-06

SI-08 — Output Generator
------------------------------

- **Input**: internal ``XpertRequestResult`` structure containing the adjustment information
- **Output**: rendered XML / HTML string
- **Depends on**: SI-07


Data Flow
=========

The nominal data flow for a single request is:

1. **SI-01** receives |CLI| arguments and starts the pipeline.
2. **SI-02** starts invoking the different components of the pipeline, in turn.
3. **SI-03** parses the input XML query into a ``XpertQueryData`` and, for each
   request, extracts a ``DrugTreatment`` (Tucuxi-Core) containing the patient's
   dosage history, covariates, samples, and targets.
4. **SI-04** loads the matching drug model into a ``DrugModel``.
5. **SI-05** loads the configuration data into a ``Configurations`` data structure.
6. **SI-06** checks the information gathered thus far, to ensure that no clear discrepancy/error is present
7. **SI-07** builds a ``ComputingRequest`` (pairing the ``DrugModel`` and the
   ``DrugTreatment`` with a ``ComputingTraitAdjustment``), submits it to the
   Tucuxi-Core ``ComputingComponent``, and populates the ``XpertRequestResult``
   with the adjustment data, parameters, and justification.
8. **SI-08** writes the output files in the format requested by the user.
9. **SI-01** returns the appropriate exit code.


Hardware / Software Interfaces
===============================

Tucuxi-CDSS runs as a native executable on the host operating system. It
does not interface with dedicated hardware. All I/O is performed through:

- The host filesystem (input files, output files, drug model files)
- Standard process streams (stderr for error reporting)
- The OS process return code.


Safety Requirements Allocation
================================

The following software items handle safety-relevant functionality and are subject
to additional verification as described in :ref:`sdsSafetyDesign`:

.. csv-table::
   :header: "Software Item", "Safety Relevance"
   :widths: 30, 70

   "SI-03 Query Parser", "Incorrect parsing of patient data could lead to erroneous computation."
   "SI-04 Drug Model Loader", "Loading a wrong or malformed drug model could produce incorrect |PK| results."
   "SI-06 Data Checker", "Undetected data errors (covariate out-of-range, inconsistent dates) propagate to computation."
   "SI-07 Adjustment Computer", "The computation core directly determines dosage recommendations."
   "SI-08 Output Generator", "Misrepresentation of results in the response (wrong units, truncation)."


Architecture Verification
==========================

.. csv-table::
   :header: "Verification Item", "Method"
   :widths: 50, 30

   "All software requirements are allocated to at least one software item", "Traceability review (see :ref:`sdsTraceability`)"
   "Software item interfaces are complete and consistent", "Interface review"
   "Safety-relevant items are identified and segregated", "Safety review (see :ref:`sdsSafetyDesign`)"
   "No circular dependencies between software items", "Architecture review"
