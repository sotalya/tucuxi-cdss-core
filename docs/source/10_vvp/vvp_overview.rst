.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _vvpOverview:

Verification and Validation Strategy
*************************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


V&V Levels
===========

Verification and validation is conducted at three levels, in order:

.. uml::
   :caption: V&V Levels and Flow

   @startuml
   skinparam defaultFontName Arial
   skinparam backgroundColor white
   skinparam shadowing false
   skinparam ArrowColor #555555
   skinparam activity {
       BackgroundColor #DDEEFF
       BorderColor #336699
       FontColor Black
   }

   |Developer|
   start
   :Implement software unit;
   :Unit Testing\n(IEC 62304 §5.5);

   |Integration Tester|
   :Integration Testing\n(IEC 62304 §5.6);

   |System Tester|
   :System Testing\n(IEC 62304 §5.7);
   :Validation against SRS;
   stop
   @enduml

.. csv-table::
   :header: "Level", "IEC 62304 Clause", "Objective", "Entry Criterion"
   :widths: 18, 15, 37, 30

   "Unit Testing", "§5.5", "Verify each software item (|SI|) behaves correctly in isolation.", "Software unit implementation complete."
   "Integration Testing", "§5.6", "Verify that software items interact correctly across their interfaces.", "All relevant unit tests passed."
   "System Testing", "§5.7", "Verify the complete system satisfies all software requirements in the :ref:`srs`.", "Integration tests passed."


Verification Methods
=====================

.. csv-table::
   :header: "Code", "Method", "Description"
   :widths: 10, 15, 55

   "T", "Test", "Automated or manual execution of a defined test case with expected result."
   "R", "Review", "Structured review of source code, design, or documentation."
   "I", "Inspection", "Examination of an output artifact (file content, log output, etc.)."
   "A", "Analysis", "Measurement or analytical computation (e.g., timing, memory)."


Pass/Fail Criteria
===================

A test case **passes** when:

- The actual output exactly matches the expected output as defined in the test case, OR
- The actual output satisfies the acceptance range defined in the test case.

A test case **fails** when:

- The actual output does not match the expected output, OR
- The software crashes, hangs, or produces an unhandled exception, OR
- A safety-relevant requirement is not met.

A **test suite passes** when all its test cases pass. A failed test case that
is not resolved by a code fix must be documented as a known issue in the
Verification and Validation Report (|VVR|), with an accepted risk rationale.


Regression Policy
==================

All unit tests and integration tests shall be executed automatically on every
code change (continuous integration). System tests shall be executed at least
at each release candidate build.

Any previously passing test that begins to fail constitutes a regression and
must be resolved before the build may be released.


.. Roles and Responsibilities
.. ===========================

.. .. csv-table::
..    :header: "Role", "Responsibility"
..    :widths: 25, 65

..    "Developer", "Write and maintain unit tests for their |SI|; fix defects found during |VaV|."
..    "Integration Tester", "Design and execute integration test cases; report defects."
..    "System Tester", "Design and execute system test cases; verify requirement coverage."
..    "QA Engineer", "Review |VaV| plans and reports; confirm |IEC62304| §5.5–5.7 compliance."
..    "Release Manager", "Approve release based on |VVR|; manage known-issue register."
