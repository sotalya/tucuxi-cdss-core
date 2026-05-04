.. SPDX-License-Identifier: AGPL-3.0-or-later

Testing Strategy & Suites
=========================

.. contents::
   :local:
   :depth: 2

Overview
--------

This document describes the different test suites used in Tucuxi-CDSS,
their goals, scope, prerequisites, and how to run them locally.

Test Suites Summary
-------------------

- **Unit Tests (GoogleTest / gtests)**: fast, isolated checks of core components.
- **System Tests (End-to-End)**: run the |CDSS| |CLI| on representative |TQF| inputs to validate report generation.
- **Fuzzing**: inject invalid/extreme values into |XML| inputs to validate robustness and crash resistance.
- **Covariate Validation Tests**: generate systematic covariate boundary violations and validate report behavior.

Repository Layout
-----------------

.. code-block:: text

   tucuxi-cdss-core/
     test/
       gtest/                      # Unit tests (GoogleTest)
       system/                     # End-to-end system tests
       fuzzing/                    # Fuzzing driver and modifiers
       covariate_boundaries_driver/# Covariate boundary test generator

Common Prerequisites
--------------------

Most non-unit tests execute the |CDSS| |CLI|, and therefore require:

- a built CDSS CLI binary: :program:`tucuxi_cdss_cli`
- runtime assets:

  - drug model files directory (``tucuxi-drugs/drugfiles/``)
  - language files directory (``tucuxi-cdss-core/language/``)
  - HTML templates directory (``tucuxi-cdss-core/html_template/``)

Suite Index
-----------

- :ref:`unit-tests-gtest`
- :ref:`system-tests`
- :ref:`fuzzing-tests`
- :ref:`covariate-validation-tests`

.. _unit-tests-gtest:


1. Unit Tests (GoogleTest)
--------------------------

The goal of the unit test suite is to validate the behavior of the Tucuxi-CDSS library at a component level:

- verify individual classes/functions in isolation (as much as possible)
- validate expected behavior for normal cases and edge cases
- detect regressions quickly during development

Location
~~~~~~~~

The unit tests are located under:

- ``test/gtest/``

This directory is organized as a small standalone test project, typically split into:

- ``include/``: test headers, fixtures, and shared helpers
- ``src/``: the test implementations (``gtest_*.cpp`` files)
- a dedicated ``main`` (GoogleTest entry point)
- a dedicated ``CMakeLists.txt`` to build the test binary

Prerequisites
~~~~~~~~~~~~~

To build and run the unit tests, you need:

- **CMake** (to configure and generate the build files)
- a compatible C++ compiler/toolchain (depending on the platform)

Build & Run
~~~~~~~~~~~

From the repository root, use the :file:`scripts/run` script to build and run the GoogleTest suite:

.. code-block:: bash

   # Build unit tests (debug mode)
   ./scripts/linux/run build-unittest

   # Build unit tests (release mode)
   ./scripts/linux/run build-unittest --release

   # Run unit tests (debug mode)
   ./scripts/linux/run run-unittest

   # Run unit tests (release mode)
   ./scripts/linux/run run-unittest --release

The ``run-unittest`` command automatically builds the test binary if it does not exist.

Scope
~~~~~

This suite focuses specifically on the internal logic and validation pipeline of Tucuxi-CDSS,
including (non-exhaustive):

- request parsing/import
- covariate validation and model selection
- dose/date/sample/target validations
- request execution logic
- translation and conversion utilities
- output/result object construction

The unit test suite is implemented across multiple source files named ``gtest_*.cpp``.

How It Runs (Typical Output)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When executing the test binary (example: ``./build/tucu-cdss-tests``), GoogleTest reports:

- the total number of tests and test suites
- execution time per test suite
- a list of failures (if any), including file and line number.

Test Suite Breakdown (High Level)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is how the unit tests are logically grouped:

- **AdjustmentTraitCreatorTest**: validates the construction of adjustment traits from treatment data and options.
- **XpertConfigImportTest**: validates |XML| configuration import and error handling for malformed or incomplete configs.
- **CovariateCheckerTest**: validates covariate presence and constraint checks (success/warning/error/missing).
- **CovariateValidatorAndModelSelectorTest**: validates covariate validation and drug model selection rules.
- **DateValidatorTest**: validates time and scheduling consistency of dosages and related date rules.
- **DoseValidatorTest**: validates dosing rules (units, under/over dosing, supported routes/formulations, warnings).
- **JustificationCreatorTest**: validates the generation of justification sentences from computation results.
- **Translations/Language tests** (e.g., ``TranslatorTest``, ``TranslationsLoaderTest``): validates translation mapping and loading.
- **RequestExecutorTest / RequestExecuterTest**: validates request execution paths (success/failure, parameter extraction).
- **SampleValidatorTest**: validates sample checks against expected ranges/percentiles and related warnings.
- **TargetValidatorTest**: validates target constraints and compatibility with drug models.
- **XpertQueryImportTest**: validates Xpert request |XML| parsing and mandatory/optional sections.
- **XpertQueryResultTest**: validates result object creation and ownership/accessors.
- **Converters/Utilities tests**: validates small conversion helpers used by the core pipeline.

.. _system-tests:

2. System Tests
----------------

System tests are located under ``test/system/`` and validate the system
end-to-end by executing the |CDSS| |CLI| with real request inputs, checking that
the produced outputs are correct.

The main objectives are:

- ensure the full request-to-report pipeline works (|CLI| -> core -> report generation)
- cover a wide range of realistic scenarios and edge cases
- verify that all report-generation branches are exercised (different inputs lead to expected report variants)

Inputs and Expected Outputs
~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Inputs are provided as |TQF| request files.
- Each ``.tqf`` file represents a specific scenario (e.g., first dose, missing dates, loading dose, rest period, sample far away, etc.).
- The expected result is typically an HTML report generated from a template.

Execution Driver Script
~~~~~~~~~~~~~~~~~~~~~~~

The system tests are driven by a Python script (``imatinib_tests.py``) which:

- enumerates all |TQF| files in the given directory
- invokes the |CDSS| |CLI| for each file with the appropriate parameters (drug files, languages, configuration, template, output folder)
- captures stdout/stderr and logs errors
- renames the produced HTML file so it matches the input file name (one report per test case)
- writes a global execution log (``output/000_LOG.txt``)

Some configuration defaults are already defined in the script, but can be
overwritten on the command line:

- ``DEFAULT_CDSS_CLI_PATH``: path to the CDSS CLI binary
- ``DEFAULT_DRUG_FILES_DIR``: directory containing the drug model files
- ``DEFAULT_LANGUAGE_FILES_DIR``: directory containing language/translation resources
- ``DEFAULT_TEMPLATE_DIR`` and ``DEFAULT_TEMPLATE_NAME``: HTML template directory and template name
- ``DEFAULT_OUT_DIR``: output directory (created fresh on each run)

Prerequisites
~~~~~~~~~~~~~

To run the system tests, the following requirements must be met:

- **Python 3**
- **pip**
- a built |CDSS| |CLI| binary (:program:`tucuxi_cdss_cli`)
- access to required runtime assets:

  - drug model files directory (``tucuxi-drugs/drugfiles/``)
  - language files directory (``tucuxi-cdss-core/language/``)
  - HTML templates directory (``tucuxi-cdss-core/html_template/``)
  - a valid |CDSS| configuration file

Python Dependencies
~~~~~~~~~~~~~~~~~~~

The Python dependencies required to run the system tests are listed in:

- ``test/system/imatinib/requirements.txt``

It is recommended to use a dedicated virtual environment.

Setup Example
~~~~~~~~~~~~~

Use the :file:`scripts/run` script to set up the Python environment:

.. code-block:: bash

   # Install all Python dependencies (creates .venv if needed)
   ./scripts/linux/run py-deps

   # Get the activation command
   ./scripts/linux/run py-env

   # Activate the virtual environment (optional)
   source ".venv/bin/activate"


How to Run
~~~~~~~~~~

Use the :file:`scripts/linux/run` script to run the Imatinib system tests:

.. code-block:: bash

   # Run with debug build
   ./scripts/linux/run imatinib

   # Run with release build
   ./scripts/linux/run imatinib --release

   # Run with coverage build
   ./scripts/linux/run imatinib --coverage

The script automatically:

1. Builds the CDSS CLI if not already built
2. Runs the test driver with all required paths configured

Use the same entrypoint pattern for the clibase CLI-input system tests:

.. code-block:: bash

   # Run with debug build
   ./scripts/linux/run clibase

   # Run with release build
   ./scripts/linux/run clibase --release

   # Run with coverage build
   ./scripts/linux/run clibase --coverage

The clibase suite runs a matrix of command-line scenarios against
``test/system/clibase/imatinib_correct_sample.tqf`` to exercise input
validation and error-management branches in :program:`tucuxi_cdss_cli`.

When running coverage repeatedly across several suites, use
``./scripts/linux/run coverage --append ...`` to keep the existing
:file:`build/coverage/` directory and accumulate additional execution data
instead of starting from a clean coverage build each time.

Outputs
~~~~~~~

The script generates an ``output/`` directory (removed and recreated on each run), containing:

- one ``.html`` report per ``.tqf`` input
- a log file: ``000_LOG.txt``

For clibase, the output directory instead contains one subdirectory per
scenario and a summary log describing pass/fail expectations for each CLI
invocation.

Return Codes / Failure Handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver script runs the |CLI| via ``subprocess`` and checks the process return code:

- if the return code is ``0`` (success) or ``1`` (partial success --- some requests failed), the produced report is kept and renamed
- otherwise, the script records the failure in ``output/000_LOG.txt``

.. _fuzzing-tests:

3. Fuzzing
----------

The fuzzing suite, located under ``test/fuzzing/``, stresses the |CDSS| |CLI|
by automatically generating invalid, unexpected, and extreme inputs, in order to
validate:

- robustness against malformed or surprising |XML| values
- error handling paths (graceful failures, clear error messages)
- absence of crashes (segfaults, aborts, unhandled exceptions)
- stability when processing large numbers of altered inputs

Unlike system tests (which use curated realistic scenarios), fuzzing attempts to
break assumptions by injecting unrealistic or corrupted values.


Prerequisites
~~~~~~~~~~~~~

To run the fuzzing tests, the following requirements must be met:

- **Python 3**
- **pip**
- a built |CDSS| |CLI| binary (:program:`tucuxi_cdss_cli`)
- access to required runtime assets:

  - drug model files directory (``tucuxi-drugs/drugfiles/``)
  - language files directory (``tucuxi-cdss-core/language/``)
  - HTML templates directory (``tucuxi-cdss-core/html_template/``)

Python Dependencies
~~~~~~~~~~~~~~~~~~~

The Python dependencies required for fuzzing are listed in:

- ``test/fuzzing/requirements.txt``

It is recommended to use a dedicated virtual environment.

Setup Example
~~~~~~~~~~~~~

Use the :file:`scripts/run` script to set up the Python environment:

.. code-block:: bash

   # Install all Python dependencies (creates .venv if needed)
   ./scripts/linux/run py-deps

   # Get the activation command
   ./scripts/linux/run py-env

   # Activate the virtual environment (optional)
   source ".venv/bin/activate"

How to Run
~~~~~~~~~~

Use the :file:`scripts/linux/run` script to run fuzzing tests:

.. code-block:: bash

   # Run with debug build
   ./scripts/linux/run fuzz

   # Run with release build
   ./scripts/linux/run fuzz --release

   # Run with coverage build
   ./scripts/linux/run fuzz --coverage

The script automatically:

1. Builds the CDSS CLI if not already built
2. Runs the fuzzing driver with all required paths configured

The script will:

- generate modified (fuzzed) |XML| / |TQF| files
- invoke the |CDSS| |CLI| on each generated input
- store logs and outputs in the ``output/`` directory


How It Works
~~~~~~~~~~~~

The fuzzing process is driven by a Python script (``main.py``) that:

1) loads a baseline input query (``imatinib.tqf``) and a baseline configuration (``config.xml``)
2) iterates through a set of *cell modifiers* (functions defined in ``cell_modifiers.py``)
3) traverses the |XML| tree and, for each text node:
   - attempts to apply a modifier
   - writes a modified |XML| file into a temporary directory (``tmptqf/``)
   - runs the |CDSS| |CLI| on that modified file
   - logs the outcome and stores the produced HTML (if any)

Cell Modifiers
~~~~~~~~~~~~~~

Cell modifiers are small functions that alter |XML| values. They target numeric
fields, units, strings, dates, and other types, for example:

- forcing numeric values to ``0`` or negative values
- multiplying numeric values by a large factor (e.g., ``* 1000``)
- swapping units (e.g., ``mg`` <-> ``mg/l``)
- injecting non-numeric values (``NaN``)
- injecting extreme values (e.g., ``1e308``)
- malformed numeric strings (e.g., ``12..34``)
- invalid / ambiguous dates (e.g., ``31/02/2023`` or ``9999-99-99``)
- invalid timezones (e.g., ``+25:00``)
- long text payloads, special characters, unicode characters
- invalid or empty units


Outcome Classification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The script records outcomes in dedicated log files:

- ``output/<NNN>_LOG.txt``: general run log
- ``output/<NNN>_ERROR.txt``: failed executions without HTML
- ``output/<NNN>_CRASH.txt``: crash signatures and stderr output

(Where ``<NNN>`` is an auto-incremented prefix to avoid overwriting previous logs.)

Artifacts
~~~~~~~~~

- ``tmptqf/``: contains generated fuzzed |XML| / |TQF| files (one per modification)
- ``output/``: contains generated HTML outputs (when produced) and logs

.. _covariate-validation-tests:

4. Covariate Validation Tests
-----------------------------

The covariate validation tests, located under
``test/covariate_boundaries_driver/``, validate the **covariate handling logic**
of the |CDSS| by generating many vignette variations that systematically violate
(or hit) covariate boundaries.

The goal is to ensure that:

- covariate constraints from the configuration file are correctly interpreted
  (valid range and typical range)
- violations are detected and correctly reflected in the generated report
  (warnings/errors)
- missing and empty covariate values are handled gracefully
- the report output remains consistent and robust across many covariate cases.

Prerequisites
~~~~~~~~~~~~~

To run the covariate validation tests, the following requirements must be met:

- **Python 3**
- **pip**
- a built |CDSS| |CLI| binary (:program:`tucuxi_cdss_cli`)
- access to required runtime assets:

  - drug model files directory (``tucuxi-drugs/drugfiles/``)
  - language files directory (``tucuxi-cdss-core/language/``)
  - HTML templates directory (``tucuxi-cdss-core/html_template/``)

Python Dependencies
~~~~~~~~~~~~~~~~~~~

The Python dependencies required to run this driver are listed in:

- ``test/covariate_boundaries_driver/requirements.txt``

It is recommended to use a dedicated virtual environment.

Setup Example
~~~~~~~~~~~~~

Use the :file:`scripts/run` script to set up the Python environment:

.. code-block:: bash

   # Install all Python dependencies (creates .venv if needed)
   ./scripts/linux/run py-deps

   # Get the activation command
   ./scripts/linux/run py-env

   # Activate the virtual environment (optional)
   source ".venv/bin/activate"

How to Run
~~~~~~~~~~

Use the :file:`scripts/linux/run` script to run the covariate boundaries driver:

.. code-block:: bash

   # Run with debug build
   ./scripts/linux/run cb-driver

   # Run with release build
   ./scripts/linux/run cb-driver --release

   # Run with coverage build
   ./scripts/linux/run cb-driver --coverage

The script automatically:

1. Builds the CDSS CLI if not already built
2. Runs the driver with all required paths configured

Artifacts
~~~~~~~~~

The script generates two directories (recreated on each run):

- ``tqf/``: generated vignette files (one ``.tqf`` per generated covariate combination)
- ``out/``: HTML reports produced by the |CDSS| |CLI|, plus:

  - ``out/000_LOG.txt``: execution log listing failures (if any)
