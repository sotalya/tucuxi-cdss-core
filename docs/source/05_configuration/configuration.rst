.. SPDX-License-Identifier: AGPL-3.0-or-later

Configuration
#############

Tucuxi-CDSS relies on a hierarchy of |XML| configuration files to
separate institution-specific policies from the drug-model definitions.
This section describes each configuration layer and the parameters
available in each.

Configuration Files and Parameters
************************************

Four categories of configuration asset control system behaviour:

.. csv-table::
   :header: "Asset", "Format", "Purpose"
   :widths: 30, 10, 60

   "|CDSS| configuration file",  "|XML|",  "Institution-wide covariate boundaries, computational limits, and default behaviour."
   "Drug model files",         "|XML|",  "|popPK| model, covariate definitions, dose ranges, and therapeutic targets for a specific drug."
   "Language files",           "|XML|",  "Translations of all warning messages, user-facing strings, and clinical recommendation sentences."
   "HTML templates",           "HTML", "Institutional report layout.  One template directory per institutional profile."

All paths are supplied at runtime via command-line flags
(see :doc:`../06_user_guide/running_tucucdsscli`); no paths are
hard-coded in the binary (though the default configuration is read from ``config.xml`` if no specific path is given).

Drug and Model Configuration
*****************************

Drug model files are the primary mechanism for drug-specific
customisation.  They are shared with the standalone Tucuxi application
:cite:`Ravix2025` and are thoroughly described
`here <https://www.tucuxi.ch/drugmodels/>`__.

Covariate definitions
======================

Each covariate used by the |popPK| model is declared with:

- **Default value**: the population median, applied when the covariate
  is absent from the patient record (a warning is always emitted in
  this case).
- **Soft threshold** (``typicalRange``): values outside
  this range trigger a warning that appears in the report, but
  processing continues.
- **Hard threshold** (``validRange``): values outside this
  range trigger an error and halt processing.  Hard thresholds are set
  at physiologically implausible limits (e.g., an age of 110 years
  most likely indicates a data-entry error rather than a real patient).

Dose configuration
==================

The drug model specifies:

- the set of allowed discrete dose values (in the clinical units of the
  drug);
- the set of allowed dosing intervals;
- a dose range for plausibility checking (doses outside this range
  trigger an error).

..
   Sample plausibility
   ===================

   The |popPK| model is used to compute population percentiles at each
   sample time point.  The |CDSS| applies the following decision table:

   .. csv-table::
      :header: "Observed concentration", "Action"
      :widths: 40, 60

      "Within 10th-90th percentile",      "Accepted; no warning."
      "Outside 10th-90th percentile",     "Warning issued; processing continues."

Therapeutic targets
===================

Targets are defined in the drug model file as a range and a best-point value.
Common target types include:

- **Trough** (|Cmin|): minimum concentration at the end of a
  dosing interval; the primary target for imatinib
  (1100-2000 ng/mL :cite:`vanderKleij2023`).
- **AUC**: total drug exposure over a dosing interval.
- **Cmax**: peak concentration.

Note that more target types are available.
The user may override the model defaults for a specific patient by
supplying alternative targets in the |XML| query file.  The system
validates that multiple supplied targets are not mutually contradictory
before forwarding them to the computational engine.

Institution-Specific Settings
******************************

Per-drug-model configuration
=============================

The ``<configs>`` section of ``config.xml`` allows defining institution-specific
overrides for each drug model.  Each ``<config>`` entry identifies a drug model
by its ``<drugId>`` and ``<drugModelId>``, and may include:

- A ``<configId>``, with which it will later be referred to.
- A ``<description>`` with one or more ``<desc>`` elements (one per language,
  identified by a ``lang`` attribute).
- A ``<covariates>`` block that specifies institution-specific
  **valid ranges** and **typical ranges** for the covariates declared in
  the drug model.  Ranges are expressed using ``<constraint>`` elements,
  each containing a ``<value>`` and an ``<operator>``
  (``gt``, ``lt``, ``geq``, ``leq``).

Institutional policies
=======================

The ``<policies>`` section of ``config.xml`` groups configurations into
named policies.  Each ``<policy>`` has a ``<policyId>`` and a
``<configList>`` that references one or more ``<configId>`` values from
the ``<configs>`` section.  This allows different institutions or wards
to enable different subsets of drug-model configurations without
duplicating the configuration entries themselves.

Language configuration
=======================

All user-visible strings --- warning messages, recommendation sentences,
report labels --- are read from |XML| language files at runtime.  Adding a
new language requires only translating the content of the corresponding
|XML| file; no recompilation is needed.  The desired language is selected
at runtime with the ``-g`` |CLI| flag, defaulting to the language
specified in the query file.

HTML reporting templates
========================

Report layout is controlled by HTML template directories.  Each
directory contains a template file into which the |CDSS| inserts computed
values and warning blocks.  Institutions can maintain several templates
(e.g., one per ward or institutional context) and select among them at
runtime with the ``-t`` and ``-p`` CLI flags.

Computational limits
====================

The |CDSS| configuration file (``config.xml``) allows imposing upper
bounds on the computational resources consumed during processing.
These limits prevent excessive CPU use in resource-constrained
deployment environments.

.. csv-table::
   :header: "Parameter (XML element)", "Built-in default", "Provided ``config.xml`` value", "Description"
   :widths: 30, 12, 12, 46

   "``nbPredictionPoints``", "10 000", "20 000", "Maximum number of concentration-time points computed for a single prediction curve."
   "``nbPercentilesPoints``", "2 000", "10 000", "Maximum number of concentration-time points computed per percentile curve."
   "``nbDosagePossibilities``", "10 000", "20 000", "Maximum number of candidate dosing regimens evaluated during dosage adjustment."

These values can be overridden in the
``<generalConfig><computationLimits>`` section of the configuration file.
Reducing the limits shortens computation time at the expense of output
resolution and dosage-search granularity.

Configuration File Formats
***************************

This section provides a description of the configuration file formats
and representative examples.

CDSS configuration file (``config.xml``)
=========================================

The main configuration file uses an |XML| format.  A minimal example
is shown below:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <cdssConfig>
       <generalConfig>
           <computationLimits>
               <nbPredictionPoints>20000</nbPredictionPoints>
               <nbPercentilesPoints>10000</nbPercentilesPoints>
               <nbDosagePossibilities>20000</nbDosagePossibilities>
           </computationLimits>
       </generalConfig>
       <configs>
           <config>
               <configId>vancomycin.colin2019</configId>
               <drugId>vancomycin</drugId>
               <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
               <description>
                   <desc lang='en'>Adults (pooled 14-study model)</desc>
               </description>
               <covariates>
                   <!-- per-drug-model covariate range overrides -->
               </covariates>
           </config>
           <!-- additional config entries ... -->
       </configs>
       <policies>
           <policy>
               <policyId>MyHospital</policyId>
               <configList>
                   <configId>vancomycin.colin2019</configId>
                   <!-- additional config references ... -->
               </configList>
           </policy>
           <!-- additional policies ... -->
       </policies>
   </cdssConfig>

Drug model file (covariate and target excerpt)
===============================================

Drug model files are shared with the standalone Tucuxi application and are detailed
`here <https://www.tucuxi.ch/drugmodels/>`__.
Below is an excerpt illustrating how covariates and therapeutic targets
are defined inside a drug model file:

.. code-block:: xml

   <!-- Covariate definition excerpt -->
   <covariate>
       <covariateId>bodyweight</covariateId>
       <covariateType>standard</covariateType>
       <dataType>double</dataType>
       <defaultValue>
           <value>70</value>
           <unit>kg</unit>
       </defaultValue>
       <validRange>
           <min>10</min>
           <max>300</max>
       </validRange>
       <typicalRange>
           <min>40</min>
           <max>150</max>
       </typicalRange>
   </covariate>

   <!-- Target definition excerpt -->
   <target>
       <targetType>residual</targetType>
       <unit>ug/l</unit>
       <min>1100</min>
       <best>1500</best>
       <max>2000</max>
   </target>

Configuration file covariate overrides
=======================================

Inside each ``<config>`` entry in ``config.xml``, covariate ranges use a
constraint-based format.  Below is an excerpt from the configuration
file showing how age and bodyweight ranges are defined for a specific
drug model configuration:

.. code-block:: xml

   <covariates>
       <covariate>
           <covariateId>age</covariateId>
           <covariateType>ageInYears</covariateType>
           <dataType>double</dataType>
           <validRange>
               <constraint>
                   <value>16</value>
                   <operator>gt</operator>
               </constraint>
               <constraint>
                   <value>130</value>
                   <operator>lt</operator>
               </constraint>
           </validRange>
           <typicalRange>
               <constraint>
                   <value>18</value>
                   <operator>geq</operator>
               </constraint>
               <constraint>
                   <value>110</value>
                   <operator>leq</operator>
               </constraint>
           </typicalRange>
       </covariate>
       <covariate>
           <covariateId>bodyweight</covariateId>
           <covariateType>standard</covariateType>
           <dataType>double</dataType>
           <validRange>
               <constraint>
                   <value>45</value>
                   <operator>gt</operator>
               </constraint>
               <constraint>
                   <value>135</value>
                   <operator>lt</operator>
               </constraint>
           </validRange>
           <typicalRange>
               <constraint>
                   <value>50</value>
                   <operator>geq</operator>
               </constraint>
               <constraint>
                   <value>130</value>
                   <operator>leq</operator>
               </constraint>
           </typicalRange>
       </covariate>
   </covariates>

Each ``<constraint>`` pairs a boundary ``<value>`` with a comparison
``<operator>``: ``gt`` (strictly greater), ``lt`` (strictly less),
``geq`` (greater or equal), ``leq`` (less or equal).

Language file
==============

Language files contain translations of all user-visible strings.
They follow a simple key/value |XML| structure validated by the
accompanying ``translations_file.xsd`` schema:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8" standalone="no"?>
   <translations
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:noNamespaceSchemaLocation="translations_file.xsd">
       <translation key="maximum_dosage_warning">Maximum recommended dosage reached</translation>
       <translation key="minimum_dosage_warning">Minimum recommended dosage reached</translation>
       <!-- ... additional translation entries ... -->
   </translations>
