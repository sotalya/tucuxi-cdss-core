.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpUnitTesting:

Unit Testing Plan
*****************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Each software item (SI-01 through SI-08) is tested in isolation. Dependencies
on other software items are replaced by test doubles (stubs or mocks) where
needed.

Test case identifiers follow the pattern **UT-SIxx-NNN** (Unit Test, Software
Item xx, sequence number NNN).

The **Existing gtest** column lists currently implemented test cases from the
repository that already cover the planned test case, expressed as
``<SuiteName>::<TestName>``.  A dash (—) means the test case is not yet
implemented and must be added.


TS-SI01 — CLI Controller
=========================

Focus areas: argument parsing, exit code mapping, per-request isolation.

.. note::
   No dedicated unit-test file was found for SI-01 in the current codebase.
   All test cases listed below must be implemented.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI01-001", "All mandatory arguments provided and valid request: process completes normally.", "T", "Exit code 0; pipeline invoked.", "—"
   "UT-SI01-002", "Missing ``--drugpath`` argument: process exits with usage error.", "T", "Non-zero exit; usage message on stderr.", "—"
   "UT-SI01-003", "Missing ``--input`` argument: process exits with usage error.", "T", "Non-zero exit; usage message on stderr.", "—"
   "UT-SI01-004", "Missing ``--config`` argument and no ``config.xml`` file present: process exits with usage error.", "T", "Non-zero exit; usage message on stderr.", "—"
   "UT-SI01-005", "Missing ``--outputpath`` argument: process exits with usage error.", "T", "Non-zero exit; usage message on stderr.", "—"
   "UT-SI01-006", "Missing ``--templatename`` argument: process exits with usage error.", "T", "Non-zero exit; usage message on stderr.", "—"
   "UT-SI01-007", "``--help`` flag: print usage and exit 0.", "T", "Exit code 0; usage printed to stdout.", "—"
   "UT-SI01-008", "All requests succeed: exit code 0.", "T", "Exit code 0.", "—"
   "UT-SI01-009", "Some requests succeed, some fail: exit code 1.", "T", "Exit code 1.", "—"
   "UT-SI01-010", "All requests fail: exit code 2.", "T", "Exit code 2.", "—"
   "UT-SI01-011", "Query file cannot be loaded: exit code 4.", "T", "Exit code 4; error on stderr.", "—"
   "UT-SI01-012", "Config file missing: exit code 3.", "T", "Exit code 3; error on stderr.", "—"
   "UT-SI01-013", "First request fails; subsequent requests still processed.", "T", "Remaining requests processed; exit code 1.", "—"
   "UT-SI01-014", "``--jsondump`` flag: JSON file written to output directory.", "T + I", "JSON file present in output directory.", "—"
   "UT-SI01-015", "Error and warning messages go to stderr, not stdout.", "T + I", "stderr contains error; stdout empty.", "—"
   "UT-SI01-016", "Operations are logged to file.", "T + I", "Logfile contains the performed operations.", "—"


TS-SI02 — CDSS Flow Manager
=============================

Focus areas: flow step orchestration, error short-circuiting, translation loading.

.. note::
   No dedicated unit-test file was found for SI-02 in the current codebase.
   All test cases listed below must be implemented.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI02-001", "All flow steps executed in the correct order for a valid request.", "T", "Each step invoked; no error recorded.", "—"
   "UT-SI02-002", "Fatal error in a flow step short-circuits the remaining steps.", "T", "Steps after the failing one are not invoked.", "—"
   "UT-SI02-003", "Translation file loaded successfully for requested language.", "T", "LanguageManager populated; no error.", "TranslationsLoaderTest::MissSpelledTranslationsLoader; TranslationsLoaderTest::BadKeyTranslationsLoader"
   "UT-SI02-004", "Missing translation file: error recorded; request skipped.", "T", "Error message set; remaining steps not executed.", "TranslationsLoaderTest::EmptyTranslationsLoader"
   "UT-SI02-005", "GeneralXpertFlowStepProvider supplies all expected step instances.", "T", "Non-null pointer for every getter.", "—"


TS-SI03 — Query Parser
========================

Focus areas: XML parsing, field extraction, validation rules.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI03-001", "Valid minimal query: all mandatory fields parsed correctly.", "T", "XpertQueryData populated; no error.", "XpertQueryImportTest::MinimalXpertRequestXmlLoader"
   "UT-SI03-002", "Patient identifier extracted.", "T", "Correct patient ID in XpertQueryData.", "XpertQueryImportTest::CompleteAdminXmlLoader"
   "UT-SI03-003", "All standard covariates parsed.", "T", "Correct values in XpertQueryData.", "XpertQueryImportTest::CompleteAdminXmlLoader"
   "UT-SI03-004", "Dosage history parsed correctly.", "T", "Correct ordered list in XpertQueryData.", "—"
   "UT-SI03-005", "Observed concentration samples parsed.", "T", "Correct list in XpertQueryData.", "XpertQueryImportTest::CompleteXpertRequestXmlLoader"
   "UT-SI03-006", "Drug identifier extracted.", "T", "Correct drug ID in XpertQueryData.", "XpertQueryImportTest::CompleteXpertRequestXmlLoader"
   "UT-SI03-007", "Prediction request parsed with sample times.", "T", "Correct request in XpertQueryData.", "XpertQueryImportTest::CompleteXpertRequestXmlLoader"
   "UT-SI03-008", "Percentiles request parsed with percentile levels.", "T", "Correct request in XpertQueryData.", "XpertQueryImportTest::CompleteXpertRequestXmlLoader"
   "UT-SI03-009", "Adjustment request parsed with target type and range.", "T", "Correct request in XpertQueryData.", "XpertQueryImportTest::CompleteXpertRequestXmlLoader"
   "UT-SI03-010", "Malformed XML: error returned, no crash.", "T", "Error result; no exception propagated.", "XpertQueryImportTest::FileBadlyFormattedXmlLoader; XpertQueryImportTest::NonExistingFileXmlLoader"
   "UT-SI03-011", "Missing patient ID: error returned.", "T", "Error result with descriptive message.", "XpertQueryImportTest::MissingMandatoryValuesInCompletePatientPersonXmlLoader"
   "UT-SI03-012", "Missing drug ID: error returned.", "T", "Error result with descriptive message.", "XpertQueryImportTest::MissingMandatoryInXpertRequestXmlLoader"
   "UT-SI03-013", "No computation requests: error returned.", "T", "Error result with descriptive message.", "XpertQueryImportTest::xpertQueryImport_importError_withoutXpertRequest"
   "UT-SI03-014", "Multiple drug requests in single query file: all parsed.", "T", "One XpertRequestData per drug request within XpertQueryData.", "XpertQueryImportTest::CompleteAdminXmlLoader"


TS-SI04 — Drug Model Loader
==============================

Focus areas: model discovery, matching, structural validation.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI04-001", "Directory contains no matching model: error returned.", "T", "Error result; no crash.", "CovariateValidatorAndModelSelectorTest::NoDrugModelForDrugIdCVAMSelector"
   "UT-SI04-002", "Directory is empty: error returned.", "T", "Error result; no crash.", "CovariateValidatorAndModelSelectorTest::NoDrugModelForDrugIdCVAMSelector"
   "UT-SI04-003", "Drug model missing mandatory section: error returned.", "T", "Error result with descriptive message.", "CovariateValidatorAndModelSelectorTest::OneModelHardConstraintNotMetCVAMSelector"
   "UT-SI04-004", "Multiple model files in directory: correct one selected.", "T", "DrugModel with matching ID returned.", "CovariateValidatorAndModelSelectorTest::BetweenThreeModelsAndNoTieCVAMSelector"


TS-SI05 — Configuration Loader
================================

Focus areas: XML parsing, default values, error handling.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI05-001", "Valid config file: all parameters loaded.", "T", "Configurations and ComputationLimits populated.", "XpertConfigImportTest::ImportCorrectXML"
   "UT-SI05-002", "Config file missing: abort with error.", "T", "Error returned; process aborts.", "—"
   "UT-SI05-003", "Config file malformed XML: abort with error.", "T", "Error returned; process aborts.", "XpertConfigImportTest::ImportWrongXmlRoot"
   "UT-SI05-004", "Mandatory parameter absent: abort with error.", "T", "Error returned with descriptive message.", "XpertConfigImportTest::ImportMissingConfigCoreInfo; XpertConfigImportTest::ImportMissingCovariatesList"


TS-SI06 — Data Checker
========================

Focus areas: covariate validation, dose validation, sample percentile evaluation,
date consistency, target consistency.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI06-001", "Covariate value within typical range: no warning.", "T", "CovariateValidationResult with empty warning.", "—"
   "UT-SI06-002", "Covariate value outside typical but within valid range: warning issued.", "T", "CovariateValidationResult with warning message.", "—"
   "UT-SI06-003", "Covariate value outside valid range: error issued.", "T", "CovariateValidationResult with error message.", "—"
   "UT-SI06-004", "Missing covariate: warning issued and default value used.", "T", "Warning recorded; default value from drug model used.", "—"
   "UT-SI06-005", "Age covariate specified directly instead of birthdate: error returned.", "T", "Error message set on XpertRequestResult.", "—"
   "UT-SI06-006", "Dose within recommended range: no warning.", "T", "DoseValidationResult with NORMAL warning level.", "—"
   "UT-SI06-007", "Dose outside recommended range: warning.", "T", "DoseValidationResult with WARNING or CRITICAL level.", "—"
   "UT-SI06-008", "Sample within P25-P75 percentile range: no warning.", "T", "SampleValidationResult with NORMAL warning level.", "—"
   "UT-SI06-009", "Sample outside P10-P90 percentile range: warning.", "T", "SampleValidationResult with CRITICAL warning level.", "—"
   "UT-SI06-010", "Dose dates chronologically consistent: no warning.", "T", "No DoseDateValidationResult with warning.", "—"
   "UT-SI06-011", "Inconsistent date/time values: warning or error.", "T", "DoseDateValidationResult or SampleDateValidationResult with warning.", "—"
   "UT-SI06-012", "Targets mutually consistent: no warning.", "T", "No error on XpertRequestResult.", "—"


TS-SI07 — Adjustment Computer
================================

Focus areas: adjustment trait creation, |PK| computation correctness,
pre-adjustment data, justification assembly.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI07-001", "Adjustment trait created with correct prediction parameter type (a priori when no samples).", "T", "ComputingTraitAdjustment with Apriori type.", "AdjustmentTraitCreatorTest::NoDosageAndNoSampleATCreator"
   "UT-SI07-002", "Adjustment: no proposal in range when target is not achievable; failure reported.", "T", "Failure result; no silent out-of-range proposal.", "AdjustmentTraitCreatorTest::NoDosageAndNoSampleATCreator"
   "UT-SI07-003", "Missing required |PK| parameter: error returned without crash.", "T", "Error result; no exception propagated.", "RequestExecutorTest::FailedResquestExecutor"
   "UT-SI07-004", "Negative concentration detected: error returned without crash.", "T", "Error result with descriptive message.", "—"
   "UT-SI07-005", "Computation error in one request does not abort others.", "T", "Remaining requests produce results.", "RequestExecutorTest::FailedResquestExecutor"
   "UT-SI07-006", "Successful computation: XpertRequestResult contains prediction data.", "T + I", "Prediction time-series present.", "XpertQueryResultTest::XpertRequestResultGetter"
   "UT-SI07-007", "Successful computation: XpertRequestResult contains percentile bands.", "T + I", "Percentile bands present.", "XpertQueryResultTest::XpertRequestResultGetter"
   "UT-SI07-008", "Successful computation: XpertRequestResult contains adjustment proposals with scores.", "T + I", "Adjustment proposals with score values present.", "XpertQueryResultTest::XpertRequestResultGetter"
   "UT-SI07-009", "Pre-adjustment data (a priori percentiles) collected.", "T", "APrioriPercentiles set on XpertRequestResult.", "—"
   "UT-SI07-010", "Justification correctly identifies dose increase.", "T", "Justification with INCREASE dose sign.", "—"
   "UT-SI07-011", "Justification correctly identifies dose decrease.", "T", "Justification with DECREASE dose sign.", "—"
   "UT-SI07-012", "No computation result data silently dropped.", "T + I", "All ComputationResult fields present in XpertRequestResult.", "XpertQueryResultTest::XpertQueryDataOwnershipTaker"


TS-SI08 — Output Generator
============================

Focus areas: file output, format selection, template rendering, file naming.

.. csv-table::
   :header: "TC ID", "Description", "Method", "Expected Result", "Existing gtest"
   :widths: 13, 38, 7, 28, 14

   "UT-SI08-001", "XML output: response file written to output directory.", "T + I", "XML file present; conforms to response schema.", "—"
   "UT-SI08-002", "HTML output: HTML file written to output directory.", "T + I", "HTML file present and non-empty.", "—"
   "UT-SI08-003", "Output file name includes query ID and a timestamp.", "T + I", "File name contains query ID and timestamp pattern.", "FileNameComputerTest::FullFileNameComputer; FileNameComputerTest::WithExtensionFileNameComputer"
   "UT-SI08-004", "Write failure: no partial file left in output directory.", "T + I", "Output directory has no truncated file.", "—"
   "UT-SI08-005", "|JSON| debug dump written when ``--jsondump`` flag is set.", "T + I", "JSON file present in output directory.", "—"
   "UT-SI08-006", "Output language applied to HTML report.", "T + I", "Report text in requested language.", "OutputLangConverterTest::EnConversion; OutputLangConverterTest::FrConversion"
   "UT-SI08-007", "All numeric output fields include unit labels.", "I", "Unit attribute/element present on every numeric field.", "—"
   "UT-SI08-008", "Error response includes descriptive message.", "I", "Non-empty error message in response.", "XpertQueryResultTest::XpertRequestResultGetter"
   "UT-SI08-009", "Valid template directory: ``index.html`` loaded.", "T", "Template loaded without error.", "—"
   "UT-SI08-010", "|JSON| payload injected at correct |DOM| injection point.", "T + I", "JSON data accessible from injected script in rendered HTML.", "—"
   "UT-SI08-011", "CSS assets inlined in rendered HTML.", "I", "No external CSS links; styles embedded in HTML.", "—"
   "UT-SI08-012", "JS assets inlined in rendered HTML.", "I", "No external script links; scripts embedded in HTML.", "—"
   "UT-SI08-013", "Image assets Base64-inlined in rendered HTML.", "I", "No external image src; data URIs present.", "—"
   "UT-SI08-014", "Template directory not found: error returned.", "T", "Error result returned.", "—"
   "UT-SI08-015", "Template name not found in template directory: error returned.", "T", "Error result returned.", "—"
