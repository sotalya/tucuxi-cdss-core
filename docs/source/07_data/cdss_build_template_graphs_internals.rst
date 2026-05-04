Graphs Internals — Data flow, entrypoints, and payload expectations
===================================================================

Purpose
-------

This document explains the **graph rendering subsystem**:

- which scripts are involved,
- which DOM nodes are used / created,
- which JSON blocks are required,
- how main graphs differ from comparative graphs,
- and the exact expectations on ``times`` / ``values`` formats.

This page is **optional**. If you remove graphs from your template, you can ignore it.

Where graphs live in the template
---------------------------------

The template provides one stable anchor:

- ``adjustmentContent`` (base HTML container)

At runtime, the renderer injects an HTML block into ``#adjustmentContent`` which includes:

- ``<div id="graphContainer"></div>`` for comparative graphs (multiple canvases)
- ``<canvas id="canBestAdj" ...></canvas>`` for the "main" adjustment graph

You do **not** need to add ``graphContainer`` or ``canBestAdj`` to your base HTML.
They are created by the injected HTML block.

Script modules and load order
-----------------------------

The graphs system is split into multiple modules.
Your reference template loads scripts in this order:

- ``drawGraph.js``
- ``graphcreator.js``
- ``graphdata.js``
- ``graphing.js``
- ``handle_input.js``

Two important symbols must exist before graph rendering runs:

1. ``CgDrawGraph(canvas, payload)``
   Used for comparative graphs (small per-target canvases).

2. ``GraphFullData`` + ``addTargets(...)`` + ``displayGraphs(...)``
   Used by the main graph rendering pipeline.

Runtime boot sequence
---------------------

On DOMContentLoaded:

1. JSON is read from ``#jsonData`` and parsed.
2. The renderer fills the static report fields (contacts, covariates, samples, etc.).
3. The renderer injects the recommendation/exposure HTML into ``#adjustmentContent``.
   This step creates the graph placeholders:

   - ``#graphContainer``
   - ``#canBestAdj``
4. Graphs are rendered:

   - main graphs: always attempted (but skipped if data is missing)
   - comparative graphs: only rendered when ``compgraph_data.compgraph_data`` is non-empty

Main graphs (time series + targets)
-----------------------------------

Entry point
^^^^^^^^^^^
Main graphs are triggered by calling:

- ``renderMainGraphs(jsonData)``

It builds:

- adjustment time series for one or more candidate adjustments
- target bands (min / best / max)
- percentile curves (a priori)
- prediction curves (a posteriori / a priori in current wiring)
- measured samples overlay

Required JSON blocks
^^^^^^^^^^^^^^^^^^^^
Main graphs require:

- ``graph_data``
- ``pda_apriori_data`` (percentiles)
- ``pda_aposteriori_data`` (concentrations) (optional but expected by the code path)
- ``samples`` (optional; used to overlay measurements)

If either adjustments or targets are missing, main graph rendering is skipped.

graph_data schema used by main graphs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``graph_data`` must provide:

- ``graph_data.adjustments[]``

  - each adjustment contains ``cycles[]``
  - each cycle contains:

    - ``start`` (string date-time)
    - ``times`` (comma-separated string of floats)
    - ``values`` (comma-separated string of floats)

- ``graph_data.targets[]``

  - each target contains:

    - ``type`` (string)
    - ``min`` (number or numeric string)
    - ``best`` (number or numeric string)
    - ``max`` (number or numeric string)

- ``graph_data.end_date`` (string date-time)
  Some producers wrap this in quotes (e.g. ``"'2024-04-22 08:30:00'"``).
  The renderer strips quotes before use.

pda_apriori_data schema used by main graphs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``pda_apriori_data.percentiles[]`` must provide:

- ``rank`` (string/number; e.g. 5,10,25,50,75,90,95)
- ``start`` (string date-time)
- ``times`` (comma-separated string of floats)
- ``values`` (comma-separated string of floats)

pda_aposteriori_data schema used by main graphs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``pda_aposteriori_data.concentrations[]`` must provide:

- ``start`` (string date-time)
- ``times`` (comma-separated string of floats)
- ``values`` (comma-separated string of floats)

Earliest date selection
^^^^^^^^^^^^^^^^^^^^^^^
The renderer computes the earliest date among:

- ``graph_data.adjustment_date`` (quotes stripped)
- each ``pda_aposteriori_data.concentrations[].start``
- each ``pda_apriori_data.percentiles[].start``

This earliest date is passed to the graph engine as the display origin.

Important format constraint
^^^^^^^^^^^^^^^^^^^^^^^^^^^
The current code expects:

- ``cycle.times`` is a string: ``"0.0,0.5,1.0"``
- ``cycle.values`` is a string: ``"582.8,643.9,..."``

If you change the producer to emit arrays instead of strings,
you must adapt the parsing code accordingly.

Comparative graphs (per-target “before/after” canvases)
-------------------------------------------------------

Entry point
^^^^^^^^^^^
Comparative graphs are rendered only if:

- ``compgraph_data.compgraph_data.length > 0``

and then the renderer calls:

- ``renderComparativeGraphs(compgraph_data.compgraph_data)``

DOM target
^^^^^^^^^^
Comparative graphs are appended into:

- ``#graphContainer``

For each item in the list, the renderer creates:

- a new ``<canvas id="graphCanvasN">`` (N = 0,1,2,...)
- wraps it in a div with class ``canvasAdjustments``

Payload passed to the graph engine
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Each comparative graph item is passed to:

- ``CgDrawGraph(canvas, payload)``

where payload includes the compgraph data (spread into ``payload.data``)
and rendering parameters (margins, scale, bounds).

compgraph_data schema used by comparative graphs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Each item in ``compgraph_data.compgraph_data[]`` may include:

- numeric values:

  - ``valueBefore``, ``valueAfter``
  - ``min``, ``best``, ``max``
- metadata:

  - ``unit``, ``targetName``
- UI labels:

  - ``textBefore``, ``textAfter``, ``textRecommendation``, etc.

The exact fields consumed by ``CgDrawGraph`` depend on the graph module.
At minimum, values and labels must match what your draw function expects.

Graph placeholders and "no adjustment" behavior
-----------------------------------------------

The HTML containing graph placeholders is injected by the recommendation renderer.

If ``adjustments.no_adjustment_phrase`` exists:

- the renderer replaces ``#adjustmentContent`` with a simple message,
- therefore ``#graphContainer`` and ``#canBestAdj`` are never created,
- and both graph renderers effectively do nothing (no DOM target).

If graphs are enabled, this means:

- ``no_adjustment_phrase`` is a "hard off switch" for the whole graph area.

Troubleshooting
---------------

Comparative graphs do not appear
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Check:

- ``compgraph_data.compgraph_data`` exists and is non-empty
- the injected HTML created ``#graphContainer`` (i.e., no ``no_adjustment_phrase``)
- ``CgDrawGraph`` exists (script load order)

Main graphs do not appear
^^^^^^^^^^^^^^^^^^^^^^^^^
Check:

- ``graph_data.adjustments`` has cycles with valid ``times`` / ``values`` strings
- ``graph_data.targets`` exists and numeric values parse correctly
- ``graph_data.end_date`` exists (quotes are stripped, but a missing value breaks rendering)
- ``GraphFullData`` / ``displayGraphs`` / ``addTargets`` exist (scripts loaded)

Times/values parsing errors
^^^^^^^^^^^^^^^^^^^^^^^^^^^
If you see NaNs or empty plots, verify:

- ``times`` and ``values`` are comma-separated numeric strings
- they have the same number of entries
- decimals are dot-based (``0.50`` not ``0,50``)

.. seealso::

   - :doc:`cdss_build_template_start_here`
   - :doc:`cdss_build_template_dom_contract`
   - :doc:`cdss_build_template_json_contract`
   - :doc:`cdss_build_template_graphs_internals`