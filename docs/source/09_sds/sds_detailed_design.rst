.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _sdsDetailedDesign:

Software Detailed Design
************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


This section provides the detailed design of each software item identified in
:ref:`sdsArchitecture`. For each item the following are described:

- Responsibilities and behaviour
- Key classes (with their source: **Tucuxi-CDSS** for classes defined in ``src/tucucdss-core/``
  or ``src/tucucdss-cli.cpp``, and **Tucuxi-Core** for classes defined in the
  ``libs/tucuxi-core/`` library)
- Key data structures
- Algorithms or processing logic
- Error handling


SI-01 — CLI Controller
=======================

Responsibilities
----------------

The |CLI| controller is the entry point of the :program:`tucuxi_cdss_cli` process.
It:

1. Parses and validates command-line arguments (via the ``cxxopts`` library).
2. Validates the existence of all specified paths; creates the output directory
   if it does not exist.
3. Instantiates the ``Computer`` class and invokes ``computeFromFile()``.
4. Handles top-level error conditions and maps them to exit codes.
5. Supports processing of multi-request query files (one response per request).

Key Classes
------------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``Computer``", "Tucuxi-CDSS", "Drives the processing pipeline: imports the query, loads configuration, and iterates over each request."
   "``ComputingConfig``", "Tucuxi-CDSS", "Struct grouping all configuration parameters (drug path, config file, output path, template name/path, language path/code, JSON dump flag)."
   "``ComputingStatus``", "Tucuxi-CDSS", "Enum (``ALL_REQUESTS_SUCCEEDED``, ``SOME_REQUESTS_SUCCEEDED``, ``NO_REQUESTS_SUCCEEDED``, ``IMPORT_ERROR``) mapped to process exit codes."

Processing Logic
----------------

.. uml::
   :caption: SI-01 CLI Controller — Processing Logic

   @startuml
   skinparam defaultFontName Arial
   skinparam backgroundColor white
   skinparam shadowing false
   skinparam activity {
       BackgroundColor #DDEEFF
       BorderColor #336699
       FontColor Black
   }
   skinparam ActivityDiamondBackgroundColor #FFF8DC
   skinparam ActivityDiamondBorderColor #B8860B
   skinparam ArrowColor #555555

   start
   :parse arguments;
   if (arguments valid?) then (no)
       :log error;
         :return exit code 3;
       stop
   endif
   :import query file **[SI-03]**;
   if (query imported?) then (no)
       :log error;
         :return exit code 4;
       stop
   endif
   :load configuration **[SI-05]**;
   if (configuration loaded?) then (no)
       :log error;
         :return exit code 4;
       stop
   endif
   while (more requests in file?) is (yes)
       :execute flow for request **[SI-02]**;
       if (request succeeded?) then (no)
           :mark request as failed;
       else (yes)
           :mark request as succeeded;
       endif
   endwhile (no)
   :return exit code(succeeded, failed);
   stop
   @enduml

Exit Codes
----------

.. csv-table::
   :header: "Code", "Meaning"
   :widths: 10, 60

   "3", "Bad command line arguments."
   "4", "Query or configuration import error."
   "0", "All requests succeeded."
   "1", "Some requests succeeded; at least one failed."
   "2", "No requests succeeded."


SI-02 — CDSS Flow Manager
===========================

Responsibilities
----------------

Orchestrates the calls to the different functional blocks for each request.
It is implemented by the ``Computer::executeFlow()`` method, which invokes a
sequence of *flow steps* provided by a drug-specific (or generic)
``AbstractXpertFlowStepProvider``. This design allows drug-specific processing
to be introduced by supplying a custom provider without modifying the core
pipeline.

Key Classes
-----------

The following classes support this mechanism:

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``AbstractXpertFlowStep``", "Tucuxi-CDSS", "Abstract interface for every flow step; defines ``perform(XpertRequestResult&)``."
   "``AbstractXpertFlowStepProvider``", "Tucuxi-CDSS", "Abstract factory that exposes virtual getters for each flow step."
   "``GeneralXpertFlowStepProvider``", "Tucuxi-CDSS", "Concrete provider for the general (non-drug-specific) case; creates instances of all standard flow steps."
   "``LanguageManager``", "Tucuxi-CDSS", "Singleton that loads and provides translations from XML language files."

Processing Logic
----------------

For each request, the flow manager executes the following steps in order:

1. Verify the extraction state (``shouldContinueProcessing``).
2. Load the translation file through ``LanguageManager``.
3. Validate covariates and select drug model **[SI-04, SI-06]**.
4. Validate doses **[SI-06]**.
5. Validate samples **[SI-06]**.
6. Validate dates **[SI-06]**.
7. Validate targets **[SI-06]**.
8. Compute time-after-dose durations **[SI-07]**.
9. Create adjustment trait **[SI-07]**.
10. Submit computation request **[SI-07]**.
11. Collect pre-adjustment data **[SI-07]**.
12. Create clinical justification **[SI-07]**.
13. Generate report **[SI-08]**.

After each step the manager checks ``shouldContinueProcessing()`` on the
``XpertRequestResult``.  The exact error-handling behaviour varies by step:

- Steps 1–4 (extraction-state verification, translation loading,
  covariate/model validation, dose validation) **abort** the flow on error
  via an active ``return``.
- Step 8 (TAD calculation) **aborts** on exception via a ``try``/``catch``
  block containing an active ``return``.
- Steps 5–7 (sample validation, date validation, target validation) and
  steps 9–13 (adjustment trait creation, computation request, pre-adjustment
  data collection, justification creation, report generation) **record** errors
  but **allow the flow to continue**, so that a report is generated with any
  available data and error information included.

Error Handling
--------------

Errors raised by individual flow steps are captured in ``XpertRequestResult``
without aborting the processing of subsequent requests in the same query file.


SI-03 — Query Parser
=======================

Responsibilities
----------------

Reads the input XML query file and produces a ``XpertQueryData`` data structure
consumed by the |CDSS| engine. The parser handles administrative patient and
mandator data as well as CDSS-specific request options.

Key Classes
-----------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``XpertQueryImport``", "Tucuxi-CDSS", "Imports the XML query into a ``XpertQueryData``; extends ``QueryImport`` (Tucuxi-Core)."
   "``XpertQueryData``", "Tucuxi-CDSS", "Top-level query data; extends ``QueryData`` (Tucuxi-Core). Contains admin data and a list of ``XpertRequestData``."
   "``XpertRequestData``", "Tucuxi-CDSS", "Per-request data: drug ID, config ID, output format/language, adjustment time, and computation options (loading dose, rest period, target extraction, formulation/route selection)."
   "``AdminData``", "Tucuxi-CDSS", "Wraps mandator, patient, and clinical data."
   "``FullPersonData``", "Tucuxi-CDSS", "Combines ``PersonData`` and ``InstituteData``."
   "``PersonData``", "Tucuxi-CDSS", "Name, address (``AddressData``), phone (``PhoneData``), email (``EmailData``)."
   "``ClinicalDatas``", "Tucuxi-CDSS", "Arbitrary key-value clinical data pairs."
   "``XpertQueryToCoreExtractor``", "Tucuxi-CDSS", "Extracts a ``DrugTreatment`` (Tucuxi-Core) from each request; extends ``QueryToCoreExtractor`` (Tucuxi-Core)."
   "``QueryImport``", "Tucuxi-Core", "Base XML import functionality."
   "``QueryData``", "Tucuxi-Core", "Base query data (treatment, covariates, samples, targets)."
   "``QueryToCoreExtractor``", "Tucuxi-Core", "Base extractor producing ``DrugTreatment`` from query data."
   "``DrugTreatment``", "Tucuxi-Core", "Combines dosage history, patient covariates, samples, and targets for one drug."

Key Data Structures
-------------------

``XpertQueryData`` contains:

- **Request metadata**: request ID, client ID, date/time, language
- **Administrative data**: mandator and patient person/institute records
  (identifier, name, address, phone, email)
- **Patient data**: patient identifier, birthdate, clinical data
  key-value pairs, covariates
- **Dosage history**: list of past administrations (dose, route, timing,
  infusion duration)
- **Drug identifier**: reference to the drug model to load
- **Observed samples**: measured drug concentrations and collection times
- **Requested computations**: list of adjustment dates and options
  (loading dose option, rest period option, target extraction option,
  formulation and route selection option)

Validation Rules
----------------

- The XML document must be well-formed and conform to the Tucuxi query schema.
- Mandatory fields (patient ID, drug ID, at least one computation request) must
  be present.
- Date/time values must be parseable and internally consistent.

Error Handling
--------------

Malformed or schema-invalid XML causes a parsing error; the request is skipped and
an error entry is added to the response.


SI-04 — Drug Model Loader
============================

Responsibilities
----------------

Locates the drug model file matching the drug identifier specified in the
configuration, loads it, and validates it. In the current implementation, the
drug model is resolved through the Tucuxi-Core ``DrugModelRepository`` and the
identifier stored in the ``Configuration`` associated with each request.

Key Classes
-----------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``CovariateValidatorAndModelSelector``", "Tucuxi-CDSS", "Flow step that selects the best drug model among candidates by computing a dissimilarity score based on covariate compatibility. (Currently unused in the pipeline; model selection is handled by the ``CovariateChecker`` using the configuration's drug model ID.)"
   "``DrugModelRepository``", "Tucuxi-Core", "Enumerates and loads drug model files from the drug model directory; provides ``getDrugModelById()``."
   "``DrugModel``", "Tucuxi-Core", "In-memory representation of a drug model (parameters, formulae, covariates, targets, formulations/routes)."

Processing Logic
----------------

1. On startup, ``Computer::computeFromString()`` creates a
   ``DrugModelRepository`` (Tucuxi-Core) and registers the drug model directory.
2. During flow execution, the ``CovariateChecker`` (SI-06) retrieves the
   ``DrugModel`` from the repository using the drug model identifier from the
   request's ``Configuration`` and sets it on the ``XpertRequestResult``.
3. Validation of mandatory drug model sections (parameters, formulae, covariates,
   targets, dosage) is performed by Tucuxi-Core during model loading.

Error Handling
--------------

If no drug model matches the query's drug identifier, the request is skipped and
an error is recorded in the ``XpertRequestResult``.


SI-05 — Configuration Loader
===============================

Responsibilities
----------------

Reads the Tucuxi-CDSS XML configuration file and provides typed access to
per-drug covariate boundaries and computation limits.

Key Classes
-----------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``ConfigImport``", "Tucuxi-CDSS", "Imports the XML configuration file into ``Configurations`` and ``ComputationLimits``; extends ``XMLImporter`` (Tucuxi-Core)."
   "``Configurations``", "Tucuxi-CDSS", "Collection of per-drug ``Configuration`` objects."
   "``Configuration``", "Tucuxi-CDSS", "Container for a single drug's covariate validation rules, including the drug ID, drug model ID, and the list of ``ConfigCovariate`` constraints."
   "``ConfigCovariate``", "Tucuxi-CDSS", "Defines a single covariate's validation rules with two boundary layers (typical range and valid range) using ``Constraint`` objects."
   "``Constraint``", "Tucuxi-CDSS", "Represents a single numerical comparison (e.g., ``> 10``); evaluated by ``evaluate(double)``."
   "``ComputationLimits``", "Tucuxi-CDSS", "Configurable upper bounds on prediction points (default 10 000), percentile points (default 2 000), and dosage possibilities (default 10 000)."
   "``EvaluationResult``", "Tucuxi-CDSS", "Enum: ``InTypicalRange``, ``InValidRange``, ``NotInRange``, ``IsEmpty``, ``CovariateNotExist``."
   "``XMLImporter``", "Tucuxi-Core", "Base XML import functionality used by ``ConfigImport``."

Key Data Structures
-------------------

- ``ConfigCovariate``: two boundary layers:

  - *Typical range* (``typicalRange``): values outside this range trigger
    a warning; processing continues.
  - *Valid range* (``validRange``): values outside this range trigger an
    error; the request is rejected.

  The ``evaluateValue()`` method returns one of the ``EvaluationResult`` values.

- ``ComputationLimits``: upper bounds applied through Tucuxi-Core's
  ``SingleOverloadEvaluator`` to prevent excessive resource consumption.

Error Handling
--------------

If the configuration file is missing or malformed, the system logs an error and
returns ``IMPORT_ERROR``, causing the CLI to exit with code -1.


SI-06 — Data Checker
======================

Responsibilities
----------------

Checks that the different data elements (covariates, doses, samples, dates,
targets) gathered by the query parser (SI-03), drug model loader (SI-04), and
configuration loader (SI-05) are correct and coherent before computation begins.

Key Classes
-----------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``CovariateChecker``", "Tucuxi-CDSS", "Validates patient covariates against the configuration constraints and the drug model definitions; also resolves the ``DrugModel`` from the repository and sets it on the ``XpertRequestResult``."
   "``DoseValidator``", "Tucuxi-CDSS", "Validates patient doses against the drug model's recommended dose ranges."
   "``SampleValidator``", "Tucuxi-CDSS", "Evaluates observed samples against a priori percentiles predicted for a typical patient under the current regimen."
   "``DateValidator``", "Tucuxi-CDSS", "Validates plausibility of dose and sample dates (chronological order, dose-to-sample distances, absorption phase detection)."
   "``TargetValidator``", "Tucuxi-CDSS", "Validates that therapeutic targets are mutually consistent."
   "``AbstractValidationResult<T>``", "Tucuxi-CDSS", "Template base class for all validation results; carries a source pointer, a warning message, and a ``WarningLevel``."
   "``CovariateValidationResult``", "Tucuxi-CDSS", "Stores the validation result for a single covariate (extends ``AbstractValidationResult<CovariateDefinition>``)."
   "``DoseValidationResult``", "Tucuxi-CDSS", "Stores the validation result for a dose (extends ``AbstractValidationResult<DosageBounded>``)."
   "``SampleValidationResult``", "Tucuxi-CDSS", "Stores the validation result for a sample, including its percentile bucket (extends ``AbstractValidationResult<Sample>``)."
   "``DoseDateValidationResult``", "Tucuxi-CDSS", "Stores the validation result for a dose date (extends ``AbstractValidationResult<DosageBounded>``)."
   "``SampleDateValidationResult``", "Tucuxi-CDSS", "Stores the validation result for a sample date (extends ``AbstractValidationResult<Sample>``)."
   "``WarningLevel``", "Tucuxi-CDSS", "Enum: ``NORMAL``, ``WARNING``, ``CRITICAL``."
   "``CovariateDefinition``", "Tucuxi-Core", "Defines a covariate in the drug model (type, unit, validation operation)."
   "``PatientCovariate``", "Tucuxi-Core", "A timed patient covariate value from the query."
   "``DosageBounded``", "Tucuxi-Core", "Abstract bounded dosage type from the dosage history."
   "``Sample``", "Tucuxi-Core", "Observed drug concentration measurement."

All checker classes extend ``AbstractXpertFlowStep`` (Tucuxi-CDSS).

Processing Logic
----------------

**Covariate checking** (``CovariateChecker``):

1. Retrieve the ``DrugModel`` from the ``DrugModelRepository`` using the drug
   model identifier from the request's ``Configuration``.
2. Set the drug model on ``XpertRequestResult``.
3. Filter patient covariates against the drug model's non-computed covariate
   definitions.
4. Reject age covariates supplied directly (age must be computed from
   birthdate).
5. For each patient covariate that matches a drug model definition, validate
   the value against the configuration's typical and valid ranges; record a
   ``CovariateValidationResult``.
6. For missing covariates, record a warning and use the model's default value.

**Dose validation** (``DoseValidator``):

1. For each dose in the patient's dosage history, check that the dose amount
   falls within the drug model's recommended range.
2. Record a ``DoseValidationResult`` for each dose.

**Sample validation** (``SampleValidator``):

1. Group samples by collection time.
2. For each group, compute a priori percentiles using Tucuxi-Core and
   determine where the observed concentration falls (< P10, P10-P25,
   P25-P75, P75-P90, > P90).
3. Record a ``SampleValidationResult`` with the percentile bucket.

**Date validation** (``DateValidator``):

1. Extract all dose dates from the dosage history.
2. Check chronological consistency and plausible dose-to-measure distances.
3. Record ``DoseDateValidationResult`` and ``SampleDateValidationResult``
   entries.

**Target validation** (``TargetValidator``):

1. Check that multiple targets for the same patient are mutually consistent.

Error Handling
--------------

Validation issues are classified by ``WarningLevel``. A ``CRITICAL`` warning
does not necessarily abort processing (the flow manager decides), but it is
prominently reported in the output.


SI-07 — Adjustment Computer
=============================

Responsibilities
----------------

Performs the adjustment computation. This includes creating the computation
trait, submitting the computation to Tucuxi-Core, collecting additional
pre-adjustment data, and assembling the justification for the
recommended dosage.

Key Classes
-----------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``AdjustmentTraitCreator``", "Tucuxi-CDSS", "Creates the ``ComputingTraitAdjustment`` (Tucuxi-Core) for the computation: determines the prediction parameter type, adjustment time, loading dose option, rest period option, and steady-state target option."
   "``RequestExecutor``", "Tucuxi-CDSS", "Submits the adjustment computation to Tucuxi-Core and gathers additional data (steady-state statistics, a priori and population parameters)."
   "``PADCollector``", "Tucuxi-CDSS", "Computes pre-adjustment data: a priori percentiles and a posteriori concentrations."
   "``JustificationCreator``", "Tucuxi-CDSS", "Assembles a clinical ``Justification`` for the dosage recommendation: compares the proposed dosage to the previous one and characterises the exposure change."
   "``TimeAfterDoseExporter``", "Tucuxi-CDSS", "Exports time-after-dose durations for each sample."
   "``Justification``", "Tucuxi-CDSS", "Data class holding the justification type, dose sign, exposure sign, interval comparison, and textual dose descriptions."
   "``ComputingTraitAdjustment``", "Tucuxi-Core", "Embeds all options needed for an adjustment computation (loading option, rest period option, steady-state target option, formulation and route selection)."
   "``ComputingAdjustments``", "Tucuxi-Core", "Performs the actual dosage adjustment computation."
   "``ComputingRequest``", "Tucuxi-Core", "Pairs a ``DrugModel``, a ``DrugTreatment``, and a ``ComputingTraits`` into a single request object submitted to the computation engine (see :ref:`main-domain-classes`)."
   "``ComputingComponent``", "Tucuxi-Core", "Main computation engine implementing ``IComputingService``; dispatches ``ComputingRequest`` objects to trait-specific solvers (see :ref:`main-domain-classes`)."
   "``ComputingResponse``", "Tucuxi-Core", "Output container returned by ``ComputingComponent::compute()``; holds predicted concentrations, cycle data, and parameters (see :ref:`main-domain-classes`)."
   "``AdjustmentData``", "Tucuxi-Core", "Result of an adjustment computation: list of candidate dosage adjustments with predicted concentrations."
   "``PercentilesData``", "Tucuxi-Core", "Percentile prediction data."
   "``SinglePredictionData``", "Tucuxi-Core", "Single-subject concentration prediction data."
   "``CycleStats``", "Tucuxi-Core", "Cycle statistics (mean, peak, trough, AUC, etc.)."

All Tucuxi-CDSS flow step classes above (``AdjustmentTraitCreator``,
``RequestExecutor``, ``PADCollector``, ``JustificationCreator``,
``TimeAfterDoseExporter``) extend ``AbstractXpertFlowStep`` (Tucuxi-CDSS).
``Justification`` is a plain data class.

Algorithm
---------

1. **Adjustment trait creation** (``AdjustmentTraitCreator``):
   determine the prediction parameter type (a priori vs. a posteriori based on
   whether samples are available), compute the adjustment time from the last
   intake, and set loading dose, rest period, and steady-state target options
   from the request.

2. **Computation submission** (``RequestExecutor``):

   a. Validate that the adjustment trait and drug model are present.
   b. Submit the adjustment trait to Tucuxi-Core's ``ComputingAdjustments``
      engine, which enumerates candidate dosing regimens from the drug model's
      available doses and dosing intervals.
   c. For each candidate regimen, Tucuxi-Core runs a prediction over the target
      evaluation interval and scores it against the specified target
      (e.g., |AUC|, |Cmin|, |Cmax|).
   d. Return candidates sorted by score; filter to those within the target range.
   e. Gather additional data for the selected candidate:

      - **Steady-state statistics**: extend the prediction by the drug
        model's half-life multiplied by the model-defined multiplier
        beyond the adjustment time to capture steady-state exposure
        metrics.
      - **A priori parameters**: if the computation was performed with
        Bayesian (a posteriori) estimation, a separate population-based
        (a priori) prediction is computed for comparison.
      - **Population/typical patient parameters**: a prediction using
        population-level parameters is computed to provide a baseline
        reference.

3. **Pre-adjustment data collection** (``PADCollector``):
   compute a priori percentiles and a posteriori concentrations for the
   period preceding the adjustment.

4. **Justification assembly** (``JustificationCreator``):
   select the best adjustment, compare it to the previous dosage, and produce
   a ``Justification`` describing the dose and exposure change.

Error Handling
--------------

Computation errors (e.g., numerical instability, missing covariates) are recorded
per request in the ``XpertRequestResult``. The remaining requests in the file
continue to be processed.


SI-08 — Output Generator
==========================

Responsibilities
----------------

Serialises the ``XpertRequestResult`` to the requested output format(s) and
writes files to the output directory.

Key Classes
-----------

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``ReportPrinter``", "Tucuxi-CDSS", "Flow step that selects the appropriate exporter based on the requested output format and dispatches the export. Extends ``AbstractXpertFlowStep``."
   "``AbstractXpertRequestResultExport``", "Tucuxi-CDSS", "Abstract base interface for all exporters; defines ``exportToFile(XpertRequestResult&)``."
   "``XpertRequestResultXmlExport``", "Tucuxi-CDSS", "Serialises ``XpertRequestResult`` to XML (extends ``ComputingQueryResponseXmlExport`` from Tucuxi-Core). Exports drug intro, admin data, covariate results, treatment, sample results, adjustment data, parameters, statistics, and computation covariates."
   "``XpertRequestResultHtmlExport``", "Tucuxi-CDSS", "Renders a standalone HTML report by injecting a |JSON| data payload into an HTML template via the Inja template engine. All CSS, JavaScript, and image assets are inlined to produce a self-contained file."
   "``AbstractHtmlExport``", "Tucuxi-CDSS", "Abstract base interface for HTML exporters."
   "``FileString``", "Tucuxi-CDSS", "Provides static minified CSS and JavaScript strings (normalize CSS, report styling, graphing scripts) for inlining into HTML."
   "``ComputingQueryResponseXmlExport``", "Tucuxi-Core", "Base XML response export functionality extended by the CDSS XML exporter."

Supported Output Formats
------------------------

.. csv-table::
   :header: "Format", "Implementation", "Description"
   :widths: 15, 15, 70

   "XML", "``XpertRequestResultXmlExport``", "Raw response in the Tucuxi XML response schema."
   "HTML", "``XpertRequestResultHtmlExport``", "Standalone HTML report rendered via the Inja template engine."

Processing Logic
----------------

1. ``ReportPrinter`` reads the desired output format from the
   ``XpertRequestResult`` and instantiates the corresponding exporter.
2. **XML output**: ``XpertRequestResultXmlExport`` serialises the
   ``XpertRequestResult`` to an XML string and writes the file to the output
   directory.
3. **HTML output**: ``XpertRequestResultHtmlExport`` builds a |JSON| object
   containing all report data (CSS strings, JavaScript code, translations, drug
   information, administrative data, clinical data, covariate validation results,
   treatment and dosage information, adjustment results, |PK| parameters, and
   statistics), reads the HTML template, injects the |JSON| payload at the
   designated injection point, and writes the rendered HTML file.
4. If the ``--jsondump`` flag is set, the |JSON| payload is also written to the
   output directory for debugging.

File Naming
-----------

Output files are named using the request identifier and a timestamp to avoid
collisions when multiple requests are processed.

Error Handling
--------------

If the template directory or template file does not exist, an error is reported
before any computation is performed. Write failures are reported through the
``XpertRequestResult`` error message.


Cross-Cutting Data Structures
==============================

The following data structures are populated and consumed across multiple software
items:

.. csv-table::
   :header: "Class", "Source", "Role"
   :widths: 15, 15, 70

   "``XpertQueryResult``", "Tucuxi-CDSS", "Top-level container for all request results; holds the computation time, admin data, output paths, and a vector of ``XpertRequestResult`` objects."
   "``XpertRequestResult``", "Tucuxi-CDSS", "Accumulates all results for a single request as the flow progresses: validation results (SI-06), adjustment data (SI-07), and is consumed by the output generator (SI-08). Key members include: drug model, covariate/dose/sample/date validation results, ``AdjustmentData``, ``PercentilesData``, ``SinglePredictionData``, ``CycleStats``, parameters, ``Configuration``, ``Justification``, and time-after-dose durations."


.. _main-domain-classes:

Main Domain Classes
====================

This section describes the core domain classes that flow through the pipeline.
Understanding these classes is essential because every software item either
produces, transforms, or consumes them.

DrugModel (Tucuxi-Core)
------------------------

``DrugModel`` is defined in ``libs/tucuxi-core/src/tucucore/drugmodel/drugmodel.h``.
It is the in-memory representation of a **pharmacokinetic drug model file**.
A ``DrugModel`` encodes all the generic pharmaceutical knowledge about a drug:
how it is absorbed, distributed, metabolised, and eliminated, as well as the
covariate effects, available formulations, therapeutic targets, and parameter
domains.

**Key members:**

.. csv-table::
   :header: "Member", "Type", "Description"
   :widths: 15, 15, 70

   "``m_drugId``", "``std::string``", "Drug identifier (e.g., ``imatinib``)."
   "``m_drugModelId``", "``std::string``", "Model version identifier."
   "``m_analyteSets``", "``std::vector<AnalyteSet>``", "Analytes and their associated |PK| parameters."
   "``m_activeMoieties``", "``ActiveMoieties``", "Active moiety definitions derived from analyte sets."
   "``m_covariates``", "``CovariateDefinitions``", "Covariate definitions (type, unit, default value, validation) that describe how patient attributes affect the |PK| parameters."
   "``m_formulationAndRoutes``", "``FormulationAndRoutes*``", "Supported formulations (e.g., tablet, intravenous) and administration routes, each with its recommended dose range."
   "``m_domain``", "``DrugModelDomain*``", "Valid parameter domains (constraints on covariate values)."
   "``m_timeConsiderations``", "``TimeConsiderations*``", "Time-to-steady-state and other temporal properties."
   "``m_metadata``", "``DrugModelMetadata*``", "Author, version, approval status."

**Key methods:**

- ``getDrugId()``, ``getDrugModelId()`` — identifiers.
- ``getAnalyteSets()`` — access the list of analytes and their parameter definitions.
- ``getFormulationAndRoutes()`` — iterate over available formulations and dose ranges.
- ``getCovariates()`` — access covariate definitions.
- ``getActiveMoieties()`` — access active moiety definitions.
- ``getParameterDefinitions(...)`` — retrieve parameters for a given formulation and route.
- ``isSingleAnalyte()``, ``isSingleMoiety()`` — convenience checks for simple single-compartment models.
- ``hasDoseCovariate()``, ``getDoseCovariate()`` — special dose-as-covariate handling.

**Lifecycle:**
loaded once by the ``DrugModelRepository`` (SI-04) from a ``.tdd`` XML file and
shared (by reference) across all requests that target the same drug.


DrugTreatment (Tucuxi-Core)
-----------------------------

``DrugTreatment`` is defined in ``libs/tucuxi-core/src/tucucore/drugtreatment/drugtreatment.h``.
It represents **one patient's treatment data for a single drug**: the actual
doses administered, the patient's covariate values, the measured drug
concentrations, and the therapeutic targets.

**Key members:**

.. csv-table::
   :header: "Member", "Type", "Description"
   :widths: 15, 15, 70

   "``m_dosageHistory``", "``DosageHistory``", "Complete chronological record of doses administered to the patient (dose amounts, routes, timing, infusion durations)."
   "``m_covariates``", "``PatientVariates``", "Patient-specific covariate values (e.g., weight, age, sex) that individualise the |PK| model."
   "``m_samples``", "``Samples``", "Measured drug concentrations collected from the patient at known times."
   "``m_targets``", "``Targets``", "Therapeutic target definitions (e.g., desired trough concentration range) that guide the dosage adjustment."

**Key methods:**

- ``getDosageHistory()`` — access the dosage history.
- ``getCovariates()`` — access patient covariate values.
- ``getSamples()`` — access measured concentration samples.
- ``getTargets()`` — access therapeutic targets.
- ``addCovariate()``, ``addDosageTimeRange()``, ``addSample()``, ``addTarget()`` — builders used during extraction.

**Lifecycle:**
created per request by ``XpertQueryToCoreExtractor`` (SI-03) during query
parsing and stored in the ``XpertRequestResult``. It is consumed (read-only)
by the data checker (SI-06) and the adjustment computer (SI-07).

**Relationship to DrugModel:**
``DrugModel`` describes *what is possible* for a drug (generic knowledge);
``DrugTreatment`` describes *what happened* to a specific patient (clinical
data). Both are required by the computation engine: the model provides the
mathematical structure; the treatment provides the patient-specific inputs.


ComputingRequest (Tucuxi-Core)
-------------------------------

``ComputingRequest`` is defined in ``libs/tucuxi-core/src/tucucore/computingservice/computingrequest.h``.
It is the input container submitted to the Tucuxi-Core computation engine.
It bundles together the three pieces of information needed for a single
computation: the drug model, the patient treatment, and the specification of
what to compute.

**Key members:**

.. csv-table::
   :header: "Member", "Type", "Description"
   :widths: 15, 15, 70

   "``m_id``", "``RequestResponseId``", "Unique identifier linking the request to its response."
   "``m_drugModel``", "``DrugModel&``", "Reference to the drug model (non-owning)."
   "``m_drugTreatment``", "``DrugTreatment&``", "Reference to the patient treatment (non-owning)."
   "``m_computingTraits``", "``ComputingTraits*``", "Specifies the type of computation: ``ComputingTraitAdjustment``, ``ComputingTraitConcentration``, ``ComputingTraitPercentiles``, etc."

**Lifecycle:**
created by the ``RequestExecutor`` flow step (SI-07) just before submission to
the ``ComputingComponent``. It is a transient object that exists only for the
duration of one computation call.


ComputingComponent (Tucuxi-Core)
---------------------------------

``ComputingComponent`` is defined in ``libs/tucuxi-core/src/tucucore/computingcomponent.h``.
It is the **main Tucuxi-Core computation engine** that performs all
pharmacokinetic calculations. It implements the ``IComputingService`` interface.

**Key method:**

- ``ComputingStatus compute(ComputingRequest&, ComputingResponse&)`` — dispatches
  to a trait-specific overload (concentration, percentiles, adjustment,
  at-measures, single-points) and populates the response.  **Note:**
  Tucuxi-CDSS exclusively uses the **adjustment** trait; the other trait
  types are capabilities of Tucuxi-Core that are not exposed by the |CDSS|.

**Internal behaviour:**
depending on the ``ComputingTraits`` type embedded in the request, the engine
enumerates candidate dosing regimens from the drug model's available doses and
intervals, runs a prediction for each candidate over the target evaluation
interval, scores each prediction against the specified target, and returns the
candidates sorted by score.

**Lifecycle:**
a single ``ComputingComponent`` instance is created once and reused for all
computations within the same process invocation.


ComputingResponse (Tucuxi-Core)
--------------------------------

``ComputingResponse`` is defined in ``libs/tucuxi-core/src/tucucore/computingservice/computingresponse.h``.
It is the output container returned by ``ComputingComponent::compute()``.
It holds the computation results as a collection of ``SingleComputingResponse``
objects, each containing predicted concentration time-series, cycle data, and
parameters.

**Key nested types:**

- ``CycleData`` — concentration data for one dosing cycle (start time, end time,
  time offsets, concentration values).
- ``AdjustmentData`` — list of candidate dosage adjustments with predicted
  concentrations and scores.
- ``PercentilesData`` — percentile prediction data.
- ``SinglePredictionData`` — single-subject concentration prediction data.

**Lifecycle:**
created by the ``RequestExecutor`` (SI-07), populated by
``ComputingComponent::compute()``, and its contents are transferred into the
``XpertRequestResult`` for downstream consumption by SI-08.
