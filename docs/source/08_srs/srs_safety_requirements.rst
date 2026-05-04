.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _srsSafetyRequirements:

Safety Requirements
*******************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Software Safety Class
=====================

If Tucuxi-CDSS were to be certified according to |IEC62304| §4.3, it would be classified as
**Software Safety Class B**, as a human operator is supposed to stay in the loop and
prevent any serious harm to the patient.
The requirements in the following subsection ensure that the major risks are averted.
Nonetheless, the following hazards are not going to be mitigated as they fall outside
the scope of the software:

.. csv-table::
   :header: "Hazard", "Rationale"
   :widths: 40, 50

   "Drug model pharmacological error", "Out of scope; addressed by model validation studies."
   "User ignores or overrides recommendation", "Out of scope; addressed by intended use labelling and training."
   "Drug model file tampered with externally", "Out of scope; addressed by access control and configuration management."


Safety Requirements
===================


.. csv-table::
   :header: "ID", "Requirement"
   :widths: 16, 75

   "SAF-01", "The software shall validate all patient covariate values against defined plausibility ranges before use in computation; values outside the defined range shall cause the request to raise an explicit error message."
   "SAF-02", "The software shall validate the structural integrity of the drug model file before use; a missing or malformed mandatory section shall cause the request to fail with an explicit error message."
   "SAF-03", "The software shall process each request independently; a failure in one request shall not affect the results of other requests in the same query file."
   "SAF-04", "The software shall not silently discard or truncate any computation result; any loss of result data shall be reported as an error."
