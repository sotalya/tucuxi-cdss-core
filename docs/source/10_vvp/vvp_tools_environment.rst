.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpToolsEnvironment:

Test Tools and Environment
**************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Test Framework
==============

.. csv-table::
   :header: "Tool", "Purpose"
   :widths: 25, 50

   "GoogleTest", "Unit and integration test execution"
   "CMake / CTest", "Build system and test runner orchestration"
   "Python (pytest)", "System-level test scripts invoking the CLI"
   "PlantUML", "Diagram generation in documentation"


Test Environments
==================

.. csv-table::
   :header: "Environment", "Used For", "OS"
   :widths: 20, 45, 25

   "Developer workstation", "Unit tests and integration tests during development", "Linux or Windows (64-bit)"
   "CI server", "Automated regression: unit tests + integration tests on every commit", "Linux (64-bit)"
   "Release candidate environment", "Full system test suite including performance and portability", "Linux + Windows + MacOS(64-bit)"



Configuration Management of Test Artefacts
===========================================

- All test data files shall be stored under version control alongside the
  source code.
- Reference expected outputs shall be reviewed and approved before being
  committed.
- Any update to a reference expected output requires a code review explaining
  the reason for the change.


Test Execution and Reporting
=============================

- Unit and integration tests are executed automatically by the |CI| pipeline on
  every commit to the main branch.
- System tests are executed manually on release candidate builds and results
  recorded in the |VVR|.
- A test is considered executed only when a pass/fail result with a timestamp,
  environment identifier, and tester identifier has been recorded.
- All test failures must be logged as defects and resolved or formally accepted
  as known issues before release.
