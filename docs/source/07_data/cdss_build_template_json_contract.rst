JSON Contract — Runtime payload used by the template
=====================================================

Purpose
-------

This document defines the **runtime JSON payload contract** used by the HTML template renderer
(``handle_input.js``).

It answers:

- which top-level JSON blocks exist,
- which fields are typically read to render each UI feature,
- what is optional vs required,
- and common data-format caveats (HTML strings, dates, parsing).

Scope
-----

This contract covers fields used by the **template runtime** (DOM rendering).
The overall application may provide additional JSON blocks; those may be ignored by the template.

Conventions
-----------
- ``?`` means optional.
- Arrays are written as ``[...]``.
- Paths use dotted notation (example: ``contacts.patient.name``).
- "Feature" sections below map directly to template areas (contacts, covariates, samples, etc.).

Runtime injection reminder
---------------------------
The binary injects a JSON string into:

::

  <script type="application/json" id="jsonData">
    {{ json_dump }}
  </script>

The renderer reads ``#jsonData.textContent`` and parses it as JSON.

Top-level blocks (common)
-------------------------
Your example payload includes these blocks:

- ``header``
- ``intro``
- ``contacts``
- ``report_translation``
- ``covariates``
- ``computation_covariates``
- ``samples``
- ``error_message`` ? (may exist depending on run)
- ``treatment``
- ``adjustments``
- ``targets``
- ``compgraph_data``
- ``graph_data`` (graphs only)
- ``pda_apriori_data`` (graphs only)
- ``pks`` (optional table, opt-in)

Additional blocks may exist (for example ``predictions``, ``clinical_datas``,
``pda_aposteriori_data``). If the current renderer does not reference them, they are ignored.

Feature: Report header and title
--------------------------------

Used for the report title and the "computed on" date.

Fields
^^^^^^
- ``header.computation_time`` (string; format is implementation-defined)
- ``header.computed_on_translation`` (string) ?
- ``header.tuberxpert_report_translation`` (string) ?
- ``report_translation.page_title`` (string)
- ``intro.drug_id`` (string) — used to fill ``.drug_name``

Notes
^^^^^
- In example dumps, ``header.computation_time`` may be a localized date string (e.g., ``"12.3.2025"``),
  not necessarily ISO.

Feature: Contacts (hospital / practitioner / patient)
-----------------------------------------------------

The contacts block provides the "left column" identity data and label translations.

Label translations
^^^^^^^^^^^^^^^^^^
Used to fill repeated label classes:

- ``contacts.name_translation`` -> ``.name_translate``
- ``contacts.phone_translation`` -> ``.phone_translate``
- ``contacts.address_translation`` -> ``.address_translation``
- ``contacts.email_translation`` -> ``.email_translation``

Hospital / institute (typically mandator institute)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Common fields:

- ``contacts.mandator.institute.name``
- ``contacts.mandator.institute.phone`` ?
- ``contacts.mandator.institute.address`` ? (may include ``<br>``)
- ``contacts.mandator.institute.email`` ?

Practitioner (mandator)
^^^^^^^^^^^^^^^^^^^^^^^
Common fields:

- ``contacts.mandator.name``
- ``contacts.mandator.email`` ?
- ``contacts.mandator.phone`` ?
- ``contacts.mandator.address`` ? (may include ``<br>``)
- ``contacts.mandator_translation`` ? (section label)
- ``report_translation.requesting_practi_translate`` ? (template label)

Patient
^^^^^^^
Common fields:

- ``contacts.patient.id``
- ``contacts.patient.name``
- ``contacts.patient.birthdate`` (string; renderer may truncate to YYYY-MM-DD)
- ``contacts.patient.address`` (often contains ``<br>``)
- ``contacts.patient.email`` ?
- ``contacts.patient.phone`` ?
- ``contacts.patient_translation`` ? (section label)

Notes (important)
^^^^^^^^^^^^^^^^^
- Address strings in your example contain embedded HTML (``<br>``). If the renderer uses ``innerHTML``,
  line breaks render correctly; if it uses ``textContent``, they will not.
- ``birthdate`` may include time (``"1981-01-01 00:00:00"``). Many templates keep only the date part.

Feature: Covariates (display list + warnings)
---------------------------------------------

Two blocks cooperate:

- ``covariates.covariates`` is the display list (with dates, values, warnings)
- ``computation_covariates.computation_covariates`` indicates which covariates were used for computation

Covariate list
^^^^^^^^^^^^^^
- ``covariates.translation`` ?
- ``covariates.date_translation`` ?
- ``covariates.value_translation`` ?
- ``covariates.none_translation`` ?

Items:
- ``covariates.covariates[]`` objects with:

  - ``name`` (string)
  - ``value`` (string)
  - ``date`` (string)
  - ``desc`` ? (string)
  - ``warning`` ? (string) — produces warning display and/or warning styling

Computation covariates
^^^^^^^^^^^^^^^^^^^^^^
- ``computation_covariates.translation`` ?
- ``computation_covariates.covariate_translation`` ?
- ``computation_covariates.value_translation`` ?

Items:
- ``computation_covariates.computation_covariates[]`` objects with:

  - ``name`` (string)
  - ``value`` ? (string)

Notes
^^^^^
- A common behavior is to separate covariates into "used" vs "unused" based on name matching.
- Some implementations exclude special covariates (e.g., ``birthdate``) from "unused" display.

Feature: CDSS / global warnings (optional)
------------------------------------------

These warnings are independent from covariate/sample warnings.

Fields
^^^^^^
- ``error_message`` ? (may be a list or a string depending on producer)

Notes
^^^^^
- The renderer typically treats ``error_message`` as list-like; if it is a string, it may be wrapped.

Feature: Samples
----------------

Samples drive the sample table and optional warning rows.

Fields
^^^^^^
Translations:

- ``samples.translation`` ?
- ``samples.date_translation`` ?
- ``samples.measure_translation`` ?
- ``samples.percentile_translation`` ?
- ``samples.none_translation`` ?

Items:

- ``samples.samples[]`` objects with:

  - ``date`` (string)
  - ``tad`` ? (string)
  - ``measure`` (string)
  - ``percentile`` ? (string/number)
  - ``warning`` ? (string) — produces an entry in the sample warnings list
  - ``warning_sentence`` ? (string) — may create an *inline row* under the sample row
  - ``warning_level`` ? (string) — typical values: ``normal``, ``warning``, ``critical``

Notes
^^^^^
- Inline warning rows usually map severity to CSS classes (e.g., ``level-warning`` / ``level-critical``).

Feature: Current treatment (current dosage)
-------------------------------------------

Fields
^^^^^^
Translations (optional):

- ``treatment.translation`` ?
- ``treatment.dosage_translation`` ?
- ``treatment.from_translation`` ?
- ``treatment.to_translation`` ?
- ``treatment.type_translation`` ?
- ``treatment.none_translation`` ?

Dosage schedule:

- ``treatment.dosage_time_ranges[]`` objects with:

  - ``date_from`` (string)
  - ``date_to`` ? (string)
  - ``type`` ? (string)
  - ``single_doses[]`` with:

    - ``dosage`` (string)

Notes
^^^^^
- Many templates render only the first range and first single dose unless extended.

Feature: Recommendation / justification block
---------------------------------------------

This is the narrative "before/new dosage + justification" content.

Fields
^^^^^^
- ``report_translation.justification_info[]`` (array; often only ``[0]`` is displayed)

Item fields (common):

- ``type`` ? (string, e.g. ``"double"``)
- ``justification_sign`` ? (string: ``increase`` | ``decrease`` | ``equal`` | ``new``)
- ``before_title`` ?
- ``before_dosage_sentence`` ? (often contains HTML like ``<strong>``)
- ``new_title`` ?
- ``dosage_sentence`` ? (often contains HTML)
- ``justification_exposure_sentence`` ?
- ``justification_dosage_sentence`` ?
- ``justification_interval_sentence`` ?

Notes
^^^^^
- Sentences may contain HTML tags (``<strong>``, ``<b>``). Templates usually inject them as HTML.

Feature: Adjustments list (alternative schedules)
-------------------------------------------------

The payload may include candidate adjustments with scores.

Fields
^^^^^^
- ``adjustments.translation`` ?
- ``adjustments.adjustment_suggested_translation`` ?
- ``adjustments.displayed_adjustment_translation`` ?
- ``adjustments.displayed_adjustments_translation`` ?
- ``adjustments.suggestion_phrase_translation`` ?
- ``adjustments.intro_phrase_translation`` ?
- ``adjustments.score_translation`` ?
- ``adjustments.dosage_translation`` ?
- ``adjustments.from_translation`` ?
- ``adjustments.to_translation`` ?
- ``adjustments.per_interval_translation`` ?

Candidates:

- ``adjustments.adjustments[]`` objects with:

  - ``score`` (string/number)
  - ``dosage_time_ranges[]`` with:

    - ``date_from`` (string)
    - ``date_to`` ? (string)
    - ``single_doses[]`` with:

      - ``dosage`` (string)

Optional override:

- ``adjustments.no_adjustment_phrase`` ? (string)
  If present, some templates replace the whole recommendation/graphs area with this message.

Notes
^^^^^
- Whether the full list of candidate adjustments is rendered depends on the current renderer behavior.
  Some renderers only show the "best suggestion" narrative and use the list internally.

Feature: Exposure table (compgraph_data) and targets
----------------------------------------------------

These blocks power the "before/after exposure vs target" display, and the comparative graph decision.

Exposure / compgraph_data
^^^^^^^^^^^^^^^^^^^^^^^^^
- ``compgraph_data.compgraph_data[]`` with (common fields):

  - ``valueBefore`` (number/string)
  - ``valueAfter`` (number/string) ?
  - ``min`` / ``best`` / ``max`` (numbers/strings)
  - ``unit`` (string)
  - ``targetName`` (string)
  - UI labels like:

    - ``textBefore``, ``textAfter``, ``textRecommendation``, ``textTherapeuticRange`` ...

Targets
^^^^^^^
- ``targets.phrase_translation`` ?
- ``targets.targets[]`` objects with:

  - ``value`` (string, e.g. ``"945.82 (ug/l)"``)
  - ``bounds`` (string; semi-structured human text)
  - ``score`` ? (string/number)
  - ``type`` ? (string)

Notes (important)
^^^^^^^^^^^^^^^^^
- ``targets.targets[0].bounds`` is not a structured object; it is a string that may contain
  embedded markup (``<b>``) and embedded values (Min/Best/Max). Many implementations parse it
  with a regex. This is inherently fragile: document the exact expected format where possible.

Feature: Graphs (optional)
--------------------------

Graphs are rendered only if graph scripts are included and the necessary JSON exists.

Main graph data
^^^^^^^^^^^^^^^
- ``graph_data.end_date`` (string; may include extra quotes in some dumps)
- ``graph_data.adjustment_date`` ? (string; may include extra quotes)
- ``graph_data.targets[]`` objects with:

  - ``min``, ``best``, ``max`` (strings/numbers)
  - ``type`` (string)
- ``graph_data.adjustments[]`` (array of adjustments) with nested:

  - ``cycles[]`` containing:

    - ``start`` (string)
    - ``times`` (string or list; representation depends on producer)
    - ``values`` (string or list; representation depends on producer)

Percentiles (a priori)
^^^^^^^^^^^^^^^^^^^^^^
- ``pda_apriori_data.percentiles[]`` objects with:

  - ``rank`` (string/number; e.g. 5, 10, 25, 50, 75, 90, 95)
  - ``start`` (string)
  - ``times`` (string CSV or list)
  - ``values`` (string CSV or list)

Samples overlay
^^^^^^^^^^^^^^^
Graphs may also reuse:

- ``samples.samples[]`` to plot measured points.

Notes
^^^^^
- If ``graph_data`` or ``pda_apriori_data`` is missing, graph rendering is typically skipped.
- Some producers serialize ``times``/``values`` as comma-separated strings; others as arrays.
  The graph renderer usually expects one specific format — document what your code produces.

Feature: PK parameters table (optional, opt-in)
-----------------------------------------------

Only rendered if the template provides the DOM hook (``pharmaParamsBody``).

Fields
^^^^^^
Translations:

- ``pks.translation`` ?
- ``pks.typical_patient_translation`` ?
- ``pks.a_priori_translation`` ?
- ``pks.a_posteriori_translation`` ?

Items:

- ``pks.pks[]`` objects with:

  - ``name`` (string, e.g. ``"CL"``, ``"V"``)
  - ``typical_patient`` (string/number)
  - ``a_priori`` (string/number)
  - ``a_posteriori`` ? (string/number)

Notes
^^^^^
- If ``pharmaParamsBody`` does not exist in the HTML, this block is ignored.

Practical data-format caveats
========================================

HTML strings inside JSON
------------------------
Some fields intentionally contain HTML tags (``<br>``, ``<strong>``, ``<b>``).
If you change the renderer to use ``textContent`` everywhere, those tags will not render.

Common examples:

- ``contacts.*.address`` (contains ``<br>``)
- ``report_translation.justification_info[].*_sentence`` (may contain ``<strong>``)
- ``targets.targets[].bounds`` (may contain ``<b>``)

Date formats
------------
Date strings may be:

- localized (``"15.4.2024 8h30"``),
- ISO (``"2024-04-15 08:30:00"``),
- or wrapped in quotes as a serialized string (``"'2024-04-22 08:30:00'"``).

If parsing is needed, document the exact expected input format(s).


.. seealso::

   - :doc:`cdss_build_template_start_here`
   - :doc:`cdss_build_template_dom_contract`
   - :doc:`cdss_build_template_json_contract`
   - :doc:`cdss_build_template_graphs_internals`