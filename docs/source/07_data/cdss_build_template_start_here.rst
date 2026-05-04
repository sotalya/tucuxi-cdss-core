Template System
============================

This document explains **how to build and customize an HTML template**
for the report rendering system.

It focuses on:

- how data flows from the binary to the browser,
- which parts are injected at build-time vs runtime,
- how to build anything from a minimal template to a full-featured report
  (samples, recommendations, graphs).

This guide is **action-oriented**.
Detailed contracts (DOM and JSON) are documented in separate pages.

Mental model
------------

The template system is split into **two independent phases**:

1. **Compile-time (CMake)**
   
   - HTML contains special inline markers.
   - CMake replaces these markers with CSS, JS, or raw data.
   - The final output is a **single self-contained HTML file**.

2. **Runtime (browser)**
   
   - The binary injects a JSON payload into the HTML.
   - JavaScript reads this JSON and fills the DOM.
   - Missing DOM elements are ignored (no crash).

You can freely redesign the HTML layout as long as you respect
the DOM contract for the features you want to display.

Compile-time asset inlining
---------------------------

The HTML template may contain special comment markers.
These markers are replaced during compilation by CMake.

Supported markers (strict syntax):

CSS
^^^
::

  <!-- INLINE_CSS:css/style.css -->

The referenced file is read from ``${ROOT_DIR}/css/style.css`` and replaced by:

::

  <style> ... </style>

JavaScript
^^^^^^^^^^
::

  <!-- INLINE_JS:handle_input.js -->

The file is read from ``${ROOT_DIR}/js/handle_input.js`` and replaced by:

::

  <script> ... </script>

Images / raw data
^^^^^^^^^^^^^^^^^
::

  <!-- INLINE_IMG:assets/logo.txt -->

The file is read verbatim and inserted at the marker location
(typically base64 or a full data URI).

If a referenced file does not exist, the build fails.

Runtime JSON injection
----------------------

At runtime, the binary injects a JSON payload into the page:

::

  <script type="application/json" id="jsonData">
    {{ json_dump }}
  </script>

When the DOM is ready, ``handle_input.js``:

1. Reads ``#jsonData.textContent``
2. Parses it as JSON
3. Renders data into the DOM

If a JSON field is missing:
- a default value is shown, or
- the corresponding section is hidden.

Building a template by features
-------------------------------

The runtime renderer is **feature-based**.

Each feature requires:

- a small set of DOM anchors (IDs or classes),
- a corresponding JSON block.

If the DOM anchors are missing, the feature is simply skipped.

The main features are:

- Contacts (hospital / practitioner / patient)
- Covariates and covariate warnings
- Samples and sample warnings
- Current treatment
- Dosage recommendation and exposure
- Main graph - time-series adjustment graph (optional)
- Comparative graphs - per-target before/after canvases (optional)
- PK parameter table (optional)

You can combine these features freely.

Feature matrix
--------------

==============================  ==============================  =========================================
Feature                          Required DOM anchors            JSON sections used
==============================  ==============================  =========================================
Contacts                         hospitalName, patientName       contacts, header
Covariates                       covariatesBody                  covariates, computation_covariates
Samples                          sample_container                samples
Warnings                         warnings* containers             covariates, samples, error_message
Current treatment                current_dosage_container        treatment
Recommendation & exposure        adjustmentContent               report_translation, targets, compgraph_data
Main graph (optional)            adjustmentContent               graph_data, pda_apriori_data, samples
Comparative graphs (optional)    adjustmentContent               compgraph_data
PK parameters (optional)         pharmaParamsBody                pks
==============================  ==============================  =========================================

Minimal working template
------------------------

This is the smallest useful HTML template.

It displays:

- page title
- hospital
- patient

HTML
^^^^
::

  <!doctype html>
  <html>
  <head>
    <meta charset="utf-8">
    <title>Report</title>

    <!-- INLINE_CSS:css/style.css -->
    <!-- INLINE_JS:handle_input.js -->
  </head>
  <body>
    <h1 id="page_title"></h1>

    <section>
      <h2 id="hospital_translate"></h2>
      <div id="hospitalName"></div>
      <div id="hospitalContact"></div>
      <div>
        <span id="report_date_translate"></span>
        <span id="reportDate"></span>
      </div>
    </section>

    <section>
      <h2 id="patient_translation"></h2>
      <div>
        <span class="name_translate"></span>
        <span id="patientName"></span>
      </div>
      <div>Date of birth: <span id="patientDOB"></span></div>
    </section>

    <script type="application/json" id="jsonData">
      {{ json_dump }}
    </script>
  </body>
  </html>

Corresponding JSON fields
^^^^^^^^^^^^^^^^^^^^^^^^^^
- ``report_translation.page_title``
- ``contacts.mandator.*``
- ``contacts.patient.*``
- ``header.computation_time``

.. seealso::

   - :doc:`cdss_build_template_start_here`
   - :doc:`cdss_build_template_dom_contract`
   - :doc:`cdss_build_template_json_contract`
   - :doc:`cdss_build_template_graphs_internals`
