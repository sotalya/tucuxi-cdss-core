.. SPDX-License-Identifier: AGPL-3.0-or-later

Common Errors and Troubleshooting
=================================

.. csv-table::
   :header: "Symptom", "Likely Cause", "Resolution"
   :widths: 25, 35, 40

   "Exit code 2 (no request succeeded)", "Drug model not found", "Ensure the drug model directory (``-d``) contains a ``.tdd`` file whose drug identifier matches the query's ``<drugId>``."
   "Exit code 3", "Missing or invalid CLI argument", "Verify all mandatory flags (``-d``, ``-i``, ``-o``, ``-t``) are present and point to valid paths."
   "Exit code 4", "Malformed XML query", "Validate the query against the schema described in :ref:`queryFormat`. Check for encoding issues or unclosed tags."
   "Covariate hard-boundary error", "Implausible covariate value", "Review the covariate value in the query. The error message identifies the offending covariate and the acceptable range."
   "No candidate within target", "Target range too narrow or dose dates too far in time", "Check that the target range and the drug model's allowed doses and intervals permit at least one feasible regimen."
   "Unexpected report language", "Translation file missing", "Ensure a translation file for the requested language exists in the language directory (``-l``)."
