.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _srsReliabilityRequirements:

Reliability and Portability Requirements
****************************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Reliability Requirements
========================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-REL-01", "The software shall not crash (exit with a signal or unhandled exception) on any syntactically valid query, regardless of the content."
   "REQ-REL-02", "The software shall not crash on a syntactically invalid (malformed XML) query file; it shall exit with exit code 4 and an error message."
   "REQ-REL-03", "The software shall not crash when the drug model directory is empty or contains no matching model."
   "REQ-REL-04", "The software shall not produce output files with partial content; if writing fails, no partial file shall remain."


Maintainability Requirements
=============================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-MAINT-01", "New drug models shall be added by placing a new drug model file in the drug model directory, without recompiling the software."
   "REQ-MAINT-02", "New HTML report templates shall be added by placing a new template directory in the template path, without recompiling the software."
   "REQ-MAINT-03", "Output language shall be extensible by adding translation files, without recompiling the software."


Portability Requirements
========================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-PORT-01", "The software shall build and run on 64-bit Linux."
   "REQ-PORT-02", "The software shall build and run on 64-bit Windows."
   "REQ-PORT-03", "The software shall build and run on 64-bit MacOS."
   "REQ-PORT-04", "The software shall not depend on any platform-specific library that is unavailable on all target platforms."
   "REQ-PORT-05", "File paths shall be handled in a platform-independent manner (support both '/' and '\\\\' path separators)."
