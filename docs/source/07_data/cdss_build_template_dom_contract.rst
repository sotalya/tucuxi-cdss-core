DOM Contract — IDs, classes, and runtime-injected nodes
========================================================

Purpose
-------

This document is the **DOM integration contract** between the HTML template and
the runtime renderer (``handle_input.js``).

The runtime code:

- looks up specific **IDs** and **classes**,
- injects HTML into some containers,
- and creates some nodes dynamically (graphs).

Key design property
-------------------

The renderer is defensive:

- if an element is missing, rendering for that part is skipped,
- helper functions typically check for element existence before modifying it.

This means you can freely redesign the HTML, but **features only appear if their
anchors exist**.

How to use this document
------------------------

Two typical workflows:

A) Compatibility mode (recommended for template redesigns)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Keep the existing IDs/classes in your new layout.
You can change markup structure and CSS freely.

B) Custom mode (recommended for long-term clean templates)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Rename IDs/classes and update selectors inside ``handle_input.js`` accordingly.

Features and required anchors
-----------------------------

The runtime behaves like a set of independent features.
Each feature has a minimal set of anchors.

Feature matrix (DOM only)
^^^^^^^^^^^^^^^^^^^^^^^^^

==============================  ==========================================
Feature                          Minimal required anchors
==============================  ==========================================
Report title / header            ``page_title`` and report date fields
Contacts                         hospital + practitioner + patient anchors
Covariates table                 ``covariatesBody`` (+ optional date label)
Warnings                          warning containers (3 independent blocks)
Samples table                    ``sample_container``
Current treatment                 ``current_dosage_container``
Recommendation / adjustment       ``adjustmentContent``
Targets table (optional)          ``targetsBody``
Main graph (optional)             container inside injected adjustment HTML
Comparative graphs (optional)     container inside injected adjustment HTML
PK parameters table (optional)    ``pharmaParamsBody`` (must be provided)
==============================  ==========================================


1) Report header / title
------------------------

These IDs display the page title and the report date.

Required IDs
^^^^^^^^^^^^
- ``page_title``
- ``report_date_translate``
- ``reportDate``

Optional IDs (translation labels)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- ``hospital_translate`` (often displayed near the hospital section)

Notes
^^^^^
- The report date is derived from the JSON header (commonly ``header.computation_time``).
- If ``report_date_translate`` is missing, the date label is not shown.


2) Contacts
-----------

The contact system is usually presented as three blocks:

- hospital (institute)
- requesting practitioner (mandator)
- patient

The renderer fills these IDs if they exist.

2.1 Hospital (institute)
^^^^^^^^^^^^^^^^^^^^^^^^

Required IDs
~~~~~~~~~~~~
- ``hospitalName``
- ``hospitalContact``

Optional IDs
~~~~~~~~~~~~
- ``hospital_translate``

Notes
~~~~~
- ``hospitalContact`` typically contains phone/address/email depending on template design.
- If you split hospital contact info into multiple lines, you can either:

  - keep a single container and inject a formatted HTML string, or
  - modify ``handle_input.js`` to target more granular IDs.

2.2 Practitioner (mandator)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Required IDs
~~~~~~~~~~~~
- ``requesting_practi_translate``
- ``practitionerName``
- ``practitionerLocation``
- ``practitionerEmail``
- ``practitionerPhone``

Notes
~~~~~
- Some JSON dumps contain nested institute info under the mandator. The reference template
  often displays institute address as the practitioner location.

2.3 Patient
^^^^^^^^^^^

Required IDs
~~~~~~~~~~~~
- ``patient_translation``
- ``patientId``
- ``patientName``
- ``patientDOB``
- ``patientAddress``
- ``patientEmail``
- ``patientPhone``

Notes
~~~~~
- Patient address in the JSON may include HTML line breaks (``<br>``).
  If your template uses ``textContent`` instead of HTML injection, those breaks
  will render literally. Keep this in mind when changing the renderer.


3) Translation label hooks (classes)
-------------------------------------

Some labels are repeated in multiple places. The renderer fills them by class.

Required classes (if you want those labels to appear)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- ``drug_name``              — formatted drug name (from ``intro.drug_id``)
- ``name_translate``         — label (from ``contacts.name_translation``)
- ``phone_translate``        — label (from ``contacts.phone_translation``)
- ``address_translation``    — label (from ``contacts.address_translation``)
- ``email_translation``      — label (from ``contacts.email_translation``)

Optional classes
^^^^^^^^^^^^^^^^
- ``dosage_translate``       — used by some templates for dosage label text (if present)


4) Covariates table
-------------------

The covariate feature renders:

- a date (usually based on the first covariate's date),
- a table body of covariate rows,
- and optionally covariate warnings.

Required IDs
^^^^^^^^^^^^
- ``covariateDate`` (optional but recommended; shows the displayed date)
- ``covariatesBody`` (required to show covariate rows)

Notes
^^^^^
- ``covariatesBody`` is typically a ``<tbody>`` element.
- The renderer injects rows (``<tr>...</tr>``) into this element.
- If you do not include ``covariatesBody``, covariates are silently skipped.


5) Warnings
-----------

Warnings are rendered in three independent blocks:

A) CDSS/global warnings (from ``error_message``)
B) Covariate warnings (from ``covariates.covariates[].warning``)
C) Sample warnings (from ``samples.samples[].warning`` and/or warning_sentence)

You can include any subset.

5.1 CDSS warnings
^^^^^^^^^^^^^^^^^
Required IDs
~~~~~~~~~~~~
- ``warningsCDSSContainer``
- ``warningsCDSS``

Expected structure (example)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
::

  <div id="warningsCDSSContainer">
    <ul id="warningsCDSS"></ul>
  </div>

Notes
~~~~~
- ``warningsCDSSContainer`` is shown/hidden as a block.
- ``warningsCDSS`` is filled with list items.

5.2 Covariate warnings
^^^^^^^^^^^^^^^^^^^^^^
Required IDs
~~~~~~~~~~~~
- ``warningsCovariateContainer``
- ``warningsCovariateList``

Expected structure (example)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
::

  <div id="warningsCovariateContainer">
    <ul id="warningsCovariateList"></ul>
  </div>

Notes
~~~~~
- Rows in the covariates table may also be styled as warning rows
  (e.g., adding a class like ``warning-row``).

5.3 Sample warnings
^^^^^^^^^^^^^^^^^^^
Required IDs
~~~~~~~~~~~~
- ``warningsSampleContainer``
- ``warningsSampleList``

Expected structure (example)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
::

  <div id="warningsSampleContainer">
    <ul id="warningsSampleList"></ul>
  </div>


6) Samples
----------

Samples are rendered into a single container (often as a table).

Required IDs
^^^^^^^^^^^^
- ``sample_container``

Notes
^^^^^
- The renderer injects the entire samples markup into this container.
- Inline warning rows may be appended under a sample row when
  ``warning_sentence`` is present, using CSS severity helpers
  (e.g., ``level-warning`` / ``level-critical``).


7) Current treatment / dosage
-----------------------------

The current dosage section is injected as a block of HTML.

Required IDs
^^^^^^^^^^^^
- ``current_dosage_container``

Notes
^^^^^
- The renderer typically formats the first dosage time range and first single dose.
- If you want to show full schedules (multiple ranges/doses), you must extend the renderer.


8) Recommendation / adjustments
-------------------------------

The recommendation section is injected as HTML into a single container.
It may also contain graph placeholders.

Required IDs
^^^^^^^^^^^^
- ``adjustmentContent``

Notes
^^^^^
- This section is also where the "no adjustment" fallback may appear
  (e.g., when ``adjustments.no_adjustment_phrase`` exists).
- If ``adjustmentContent`` is missing, recommendation and graphs are skipped.


9) Targets (optional)
---------------------

The targets section displays the therapeutic target values used for
adjustment scoring.  This data is also available in the JSON contract
(see ``targets.targets[]`` in the :doc:`JSON contract <cdss_build_template_json_contract>`).

Required IDs
^^^^^^^^^^^^
- ``targetsBody``

Expected markup
^^^^^^^^^^^^^^^
You must provide a table body like:

::

  <table>
    <thead>
      <tr>
        <th>Type</th>
        <th>Value</th>
        <th>Score</th>
        <th>Bounds</th>
      </tr>
    </thead>
    <tbody id="targetsBody"></tbody>
  </table>

Notes
^^^^^
- If ``targetsBody`` is missing, the targets table is silently skipped.
- Each row corresponds to one entry in ``targets.targets[]``.
- The ``bounds`` field may contain embedded HTML (e.g., ``<b>`` tags).


10) Runtime-injected nodes (created by JS)
------------------------------------------

Some IDs do not exist in the base HTML and are created as part of injected HTML.

These IDs are commonly created inside the recommendation/exposure markup:

- ``dailyExposure``      — cell/element showing exposure value(s)
- ``graphContainer``     — container where comparative canvases are appended
- ``canBestAdj``         — main canvas for “best adjustment” graph

Comparative graph canvases are created dynamically:
- ``graphCanvas0``
- ``graphCanvas1``
- ``graphCanvas2``
- ...

Important
^^^^^^^^^
- Your template does **not** need to include these IDs directly.
- You only need to provide the higher-level anchor that receives injected HTML
  (typically ``adjustmentContent``).


11) Optional PK parameters table (opt-in)
------------------------------------------

The renderer includes a PK parameter table builder, but it is opt-in:
it only renders if the template provides the expected hook.

Required ID
^^^^^^^^^^^
- ``pharmaParamsBody``

Expected markup
^^^^^^^^^^^^^^^
You must provide a table body like:

::

  <table>
    <thead>
      <tr>
        <th>Parameter</th>
        <th>Typical patient</th>
        <th>A priori</th>
        <th>A posteriori</th>
      </tr>
    </thead>
    <tbody id="pharmaParamsBody"></tbody>
  </table>

Notes
^^^^^
- If ``pharmaParamsBody`` is missing, PK parameters are silently skipped.
- Labels and translations are driven by the ``pks`` JSON block.


Recommended minimal DOM per feature
==============================================

This appendix provides minimal HTML snippets per feature.
They are safe defaults for compatibility mode.

Contacts (minimal)
------------------
::

  <h2 id="hospital_translate"></h2>
  <div id="hospitalName"></div>
  <div id="hospitalContact"></div>

  <h2 id="requesting_practi_translate"></h2>
  <div id="practitionerName"></div>
  <div id="practitionerLocation"></div>
  <div id="practitionerEmail"></div>
  <div id="practitionerPhone"></div>

  <h2 id="patient_translation"></h2>
  <div id="patientId"></div>
  <div id="patientName"></div>
  <div id="patientDOB"></div>
  <div id="patientAddress"></div>
  <div id="patientEmail"></div>
  <div id="patientPhone"></div>

Covariates (minimal)
--------------------
::

  <div id="covariateDate"></div>
  <table>
    <tbody id="covariatesBody"></tbody>
  </table>

Samples (minimal)
-----------------
::

  <div id="sample_container"></div>

Warnings (minimal)
------------------
::

  <div id="warningsCDSSContainer">
    <ul id="warningsCDSS"></ul>
  </div>

  <div id="warningsCovariateContainer">
    <ul id="warningsCovariateList"></ul>
  </div>

  <div id="warningsSampleContainer">
    <ul id="warningsSampleList"></ul>
  </div>

Recommendation (minimal)
------------------------
::

  <div id="adjustmentContent"></div>

Current dosage (minimal)
------------------------
::

  <div id="current_dosage_container"></div>

PK parameters (minimal)
-----------------------
::

  <tbody id="pharmaParamsBody"></tbody>

Targets (minimal)
-----------------
::

  <table>
    <tbody id="targetsBody"></tbody>
  </table>

.. seealso::

   - :doc:`cdss_build_template_start_here`
   - :doc:`cdss_build_template_dom_contract`
   - :doc:`cdss_build_template_json_contract`
   - :doc:`cdss_build_template_graphs_internals`
