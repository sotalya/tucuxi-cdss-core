.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _sdsInterfaceDesign:

Interface Design
****************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


CLI Interface
=============

The system is invoked as a command-line process. The full argument specification is:

.. code-block:: bash

   Usage:
       ./tucuxi_cdss_cli [OPTION...]

       -d, --drugpath arg      Path of the directory containing drug model files
       -i, --input arg         Path of the input XML query file
       -c, --config arg        Path of the XML configuration file (optional; default: config.xml)
       -o, --outputpath arg    Path of the directory for output files
       -l, --languagepath arg  Path of the directory containing translation files
                               (optional; default: language/ subdirectory of root)
       -g, --language arg      Desired output language code (optional; default:
                               English)
       -t, --templatename arg  Name of the HTML template to use
       -p, --templatepath arg  Path of the directory containing HTML templates
                               (optional; default: template/ subdirectory of root)
       -j, --jsondump          Enable JSON dump for debugging (written to output
                               folder; disabled by default)
           --help              Print this help message

Mandatory arguments are: ``--drugpath``, ``--input``,
``--outputpath``, ``--templatename``.


XML Query Interface
===================

The input to the system is an XML file conforming to the Tucuxi query schema.
The complete specification of the query format is provided in the
:ref:`data-formats` section. SI-03 (Query Parser) reads this file into a
``XpertQueryData`` structure and, for each request, the
``XpertQueryToCoreExtractor`` extracts a ``DrugTreatment`` (Tucuxi-Core)
containing the patient's dosage history, covariates, samples, and targets
(see :ref:`main-domain-classes`).

Top-Level Structure
-------------------

.. code-block:: xml

   <query>
       <date>...</date>
       <admin>...</admin>
       <drugTreatment>
           <patient>
                <covariates> ... </covariates>
           </patient>
           <drugs>
               <drug>
                   <drugId>...</drugId>
                   <treatment> ... </treatment>
                   <samples> ... </samples>
                   ...
               </drug>
           </drugs>
       </drugTreatment>
       <requests>
           <xpertRequest>
            <drugId> ... </drugId>
            <configId> ... </configId>
            <output>
                <format> ... </format>
                <language> ... </language>
            </output>
            <adjustmentDate> ... </adjustmentDate>
            <options> ...  </options>
           </xpertRequest>
       </requests>
   </query>

Key Elements
------------

.. csv-table::
   :header: "Element", "Description"
   :widths: 25, 55

   "``<admin>``", "Administrative data on the patient, mandator, institution."
   "``<patient>``", "Patient covariates."
   "``<treatment>``", "Past dosing administrations."
   "``<samples>``", "Observed drug concentration measurements."
   "``<xpertRequest>``", "Computation request options and the adjustment date."


XML Response Interface
======================

The primary output is an XML file conforming to the Tucuxi response schema.

.. note::

   The simplified structure shown below illustrates the core adjustment data.
   The full response format, rooted in ``<tuberxpertResult>``, includes
   additional elements (computation time, drug info, admin data, covariates,
   treatment, samples, parameters, statistics) described in the
   :ref:`data-formats` section.

Top-Level Structure
-------------------

.. code-block:: xml

   <dataAdjustment>
       <adjustments>
           <adjustment>
               <cycleData> ... </cycleData>
           </adjustment>
       </adjustments>
   </dataAdjustment>

Key Elements
------------

.. csv-table::
   :header: "Element", "Description"
   :widths: 25, 55

   "``<adjustments>``", "List of dosage adjustment proposals with scores."
   "``<cycleData>``", "Per-dosing-cycle summary metrics (|AUC|, |Cmin|, |Cmax|, ...)."


Drug Model File Interface
=========================

Drug models are XML files located in the drug model directory. The system reads
these files at runtime through SI-04. Each file describes a single drug model.

Top-Level Structure
-------------------

.. code-block:: xml

   <model>
       <head>
           <drugName> ... </drugName>
       </head>
       <drugModel>
           <drugModelId>...</drugModelId>
           <activeMoieties> ... </activeMoieties>
           <covariates> ... </covariates>
           <formulationAndRoutes> ... </formulationAndRoutes>
       </drugModel>
   </model>

The drug model format is fully specified in the Tucuxi Drug Model File Specification.


HTML Report Interface
=====================

When HTML output is requested, the report is produced by SI-08.

Template Directory Layout
--------------------------

.. code-block:: none

   <templatepath>/
   +-- <templatename>/
       +-- index.html          (main template file)
       +-- assets/             (images, Base64-inlined at build time)
       +-- css/                (stylesheets, inlined at build time)
       +-- js/                 (scripts, inlined at build time)

JSON Data Payload
-----------------

The report template receives a |JSON| object injected at a well-known injection
point. The |JSON| structure contains all computation results needed for rendering.
