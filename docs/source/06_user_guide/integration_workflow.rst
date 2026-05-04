.. SPDX-License-Identifier: AGPL-3.0-or-later

Typical Integration Workflow
============================

1. **Construct the XML query.**
   The integrating application assembles an |XML| query file containing the
   patient's demographic covariates, dosing history, observed drug
   concentrations, and the desired adjustment options. The query format is
   described in :ref:`queryFormat`.

2. **Invoke the CLI.**
   Call :program:`tucuxi_cdss_cli` with the required arguments (drug model
   directory, query file, output directory, and template name).
   See :doc:`running_tucucdsscli` for the full argument list.

3. **Check the exit code.**
   The process exit code indicates overall success or failure (see
   :ref:`return_values`).

4. **Consume the output.**

   - **XML response**: a machine-readable file that the integrating application
     can parse to extract the recommended dosage, warnings, and supporting data.
     See :doc:`../07_data/cdss_response_description`.
   - **HTML report**: a human-readable report intended for display to the end
     user. The report is rendered from an institutional HTML template and can be
     served directly by a web application.
