.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _srsVerification:

Requirements Verification
*************************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Verification Methods
====================

According to |IEC62304| §5.2.5, each requirement shall be verified by one or more of the following methods:

.. csv-table::
   :header: "Code", "Method", "Description"
   :widths: 10, 15, 55

   "T", "Test", "Automated unit test or integration test."
   "R", "Review", "Code review or document review."
   "I", "Inspection", "Inspection of the output artifact (file content, log)."
   "A", "Analysis", "Analytical demonstration (e.g., timing measurement, static analysis)."


Verification Matrix
===================

.. csv-table::
   :header: "Requirement ID", "Verification Method", "Test Reference", "Status"
   :widths: 18, 20, 30, 12

   "REQ-CLI-01", "T", "TBD", "Open"
   "REQ-CLI-02", "T", "TBD", "Open"
   "REQ-CLI-03", "T", "TBD", "Open"
   "REQ-CLI-04", "T", "TBD", "Open"
   "REQ-CLI-05", "T", "TBD", "Open"
   "REQ-CLI-06", "T", "TBD", "Open"
   "REQ-CLI-07", "T", "TBD", "Open"
   "REQ-CLI-08", "T", "TBD", "Open"
   "REQ-CLI-09", "T", "TBD", "Open"
   "REQ-CLI-10", "T", "TBD", "Open"
   "REQ-CLI-11", "T", "TBD", "Open"
   "REQ-CLI-12", "T", "TBD", "Open"
   "REQ-CLI-13", "T", "TBD", "Open"
   "REQ-CLI-14", "T", "TBD", "Open"
   "REQ-CLI-15", "T", "TBD", "Open"
   "REQ-QRY-01", "T", "TBD", "Open"
   "REQ-QRY-02", "T", "TBD", "Open"
   "REQ-QRY-03", "T", "TBD", "Open"
   "REQ-QRY-04", "T", "TBD", "Open"
   "REQ-QRY-05", "T", "TBD", "Open"
   "REQ-QRY-06", "T", "TBD", "Open"
   "REQ-QRY-07", "T", "TBD", "Open"
   "REQ-QRY-08", "T", "TBD", "Open"
   "REQ-QRY-09", "T", "TBD", "Open"
   "REQ-QRY-10", "T", "TBD", "Open"
   "REQ-QRY-11", "T", "TBD", "Open"
   "REQ-QRY-12", "T", "TBD", "Open"
   "REQ-DM-01", "T", "TBD", "Open"
   "REQ-DM-02", "T", "TBD", "Open"
   "REQ-DM-03", "T", "TBD", "Open"
   "REQ-DM-04", "T", "TBD", "Open"
   "REQ-CMP-01", "T + A", "TBD", "Open"
   "REQ-CMP-02", "T + A", "TBD", "Open"
   "REQ-CMP-03", "T + A", "TBD", "Open"
   "REQ-CMP-04", "T", "TBD", "Open"
   "REQ-CMP-05", "T + A", "TBD", "Open"
   "REQ-RSP-01", "T + I", "TBD", "Open"
   "REQ-RSP-02", "T + I", "TBD", "Open"
   "REQ-RSP-03", "T + I", "TBD", "Open"
   "REQ-RSP-04", "I", "TBD", "Open"
   "REQ-RSP-05", "T + I", "TBD", "Open"
   "REQ-RSP-06", "T + I", "TBD", "Open"
   "REQ-RSP-07", "T + I", "TBD", "Open"
   "REQ-RPT-01", "T + I", "TBD", "Open"
   "REQ-RPT-03", "T", "TBD", "Open"
   "REQ-RPT-04", "T", "TBD", "Open"
   "REQ-RPT-05", "T + I", "TBD", "Open"
   "REQ-CFG-01", "T", "TBD", "Open"
   "REQ-CFG-02", "T", "TBD", "Open"
   "REQ-CFG-03", "T", "TBD", "Open"
   "REQ-CFG-04", "T", "TBD", "Open"
   "REQ-IF-CLI-01", "R", "TBD", "Open"
   "REQ-IF-CLI-02", "T", "TBD", "Open"
   "REQ-IF-CLI-03", "T + I", "TBD", "Open"
   "REQ-IF-CLI-04", "T + I", "TBD", "Open"
   "REQ-IF-QRY-01", "R", "TBD", "Open"
   "REQ-IF-QRY-02", "T", "TBD", "Open"
   "REQ-IF-RSP-01", "R", "TBD", "Open"
   "REQ-IF-RSP-02", "T + I", "TBD", "Open"
   "REQ-IF-RSP-03", "T + I", "TBD", "Open"
   "REQ-IF-DM-01", "T", "TBD", "Open"
   "REQ-IF-DM-02", "T", "TBD", "Open"
   "REQ-IF-DM-03", "R", "TBD", "Open"
   "REQ-IF-RPT-01", "T + I", "TBD", "Open"
   "REQ-IF-RPT-02", "T + I", "TBD", "Open"
   "REQ-IF-RPT-03", "T + I", "TBD", "Open"
   "REQ-IF-RPT-04", "R", "TBD", "Open"
   "REQ-REL-01", "T", "TBD", "Open"
   "REQ-REL-02", "T", "TBD", "Open"
   "REQ-REL-03", "T", "TBD", "Open"
   "REQ-REL-04", "T + I", "TBD", "Open"
   "REQ-MAINT-01", "T", "TBD", "Open"
   "REQ-MAINT-02", "T", "TBD", "Open"
   "REQ-MAINT-03", "T", "TBD", "Open"
   "REQ-PORT-01", "T", "TBD", "Open"
   "REQ-PORT-02", "T", "TBD", "Open"
   "REQ-PORT-03", "T", "TBD", "Open"
   "REQ-PORT-04", "R", "TBD", "Open"
   "REQ-PORT-05", "T", "TBD", "Open"
   "SAF-01", "T", "TBD", "Open"
   "SAF-02", "T", "TBD", "Open"
   "SAF-03", "I", "TBD", "Open"
   "SAF-04", "T", "TBD", "Open"
