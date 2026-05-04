.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _srsInterfaceRequirements:

Interface Requirements
**********************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


CLI Interface
=============

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-IF-CLI-01", "The software shall expose a command-line interface."
   "REQ-IF-CLI-02", "The software shall print a usage message and exit when invoked with the ``--help`` flag."
   "REQ-IF-CLI-03", "The software shall write error and warning messages to the standard error stream (stderr)."
   "REQ-IF-CLI-04", "The software shall keep a log of the operations performed in a text file in the application directory."


XML Query Interface
===================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-IF-QRY-01", "The XML query format shall conform to the schema defined in the :ref:`data-formats` section."
   "REQ-IF-QRY-02", "The software shall support queries containing multiple independent drug requests within a single query file."

..    "REQ-IF-QRY-03", "The software shall support continuous, categorical, and date-based covariate types."
..    "REQ-IF-QRY-04", "The software shall support the following dosage route types: oral, intravenous bolus, intravenous infusion."


XML Response Interface
======================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-IF-RSP-01", "The XML response format shall conform to the schema defined in the :ref:`data-formats` section."
   "REQ-IF-RSP-02", "The response file, if requested, shall be written to the output directory specified on the command line."
   "REQ-IF-RSP-03", "The output file name shall include the query identifier and a timestamp to avoid collisions."


Drug Model File Interface
=========================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-IF-DM-01", "Drug model files shall be XML files located in the directory specified by the ``--drugpath`` argument."
   "REQ-IF-DM-02", "The software shall scan all files in the drug model directory to find a matching model; subdirectory traversal is not required."
   "REQ-IF-DM-03", "The drug model file format shall conform to the Drug Model specification."


HTML Report Interface
=====================

.. csv-table::
   :header: "ID", "Requirement"
   :widths: 18, 75

   "REQ-IF-RPT-01", "HTML templates shall reside in a subdirectory named after the template, inside the template directory."
   "REQ-IF-RPT-02", "The template's main file shall be named ``index.html``."
   "REQ-IF-RPT-03", "The software shall inject the |JSON| data payload into the template at the injection point defined in the |DOM| contract."
   "REQ-IF-RPT-04", "CSS, JavaScript, and image assets referenced in the template shall be inlined (Base64 for images) at build time."
