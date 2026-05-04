.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _srsOverallDescription:

Overall Description
*******************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Software Perspective
====================

Tucuxi-CDSS is a standalone software component used within a
Therapeutic Drug Monitoring (|TDM|) workflow. It
acts as the computation backend for |TDM| applications: it receives structured
patient and dosing information, performs |PK| calculations, and returns dosage
adjustment proposals.  Concentration predictions and percentile data are
computed internally as part of the adjustment process and are included in
the response for context, but cannot be requested independently.

The system is invoked as a command-line process and communicates exclusively
through files (XML input, XML/HTML output). It does not expose a network
interface or a graphical user interface.

.. uml::
   :caption: Tucuxi-CDSS — Software context and interactions

   @startuml
   skinparam defaultFontName Arial
   skinparam backgroundColor white
   skinparam shadowing false
   skinparam ArrowColor #555555

   skinparam actor {
       BackgroundColor #FFF8DC
       BorderColor #B8860B
       FontColor Black
   }

   skinparam component {
       BackgroundColor #DDEEFF
       BorderColor #336699
       FontColor Black
   }

   skinparam file {
       BackgroundColor #F5F5F5
       BorderColor #999999
   }

   skinparam folder {
       BackgroundColor #F0FFF0
       BorderColor #228B22
   }

   actor "Client Application" as client
   actor "End User" as enduser

   component "tucuxi_cdss_cli" as cdss

   folder "Drug Model\nDirectory" as drugdir
   file "Configuration\nFile (.xml)" as config
   file "XML Query (.xml)" as query
   file "XML Response (.xml)" as xmlresp
   file "HTML Report (.html)" as html

   client --> query       : writes
   query  --> cdss        : input
   drugdir --> cdss       : loaded by
   config  --> cdss       : loaded by

   cdss --> xmlresp       : produces
   cdss --> html          : produces\n(optional)

   xmlresp --> client     : reads
   html    --> enduser  : views
   @enduml


System Functions
================

At a high level, Tucuxi-CDSS shall:

1. Accept a structured XML query describing a patient, their dosage history, their covariates, drug samples,
   and one or more computation requests.
2. Load the appropriate drug model for the requested drug.
3. Perform checks on the inputs.
4. Perform a dosage adjustment for each request
5. Depending on the requests, return a structured XML response containing computation results and a status
   for each request, or an HTML report.


User Characteristics
====================

Tucuxi-CDSS has not been designed to be used directly by end users.
Indeed, its users are:

- **Integrating applications**: software systems that construct the XML query
  and consume the XML response programmatically.
- **Clinical pharmacologists / software developers**: who configure drug models
  and templates, and who may invoke the |CLI| directly for testing or batch processing.

No particular computing expertise is assumed for the integrating application;
the XML interface is documented in the :ref:`data-formats` section.


Operating Environment
=====================

- **Supported platforms**: Linux, Windows, MacOS.
- **Runtime dependencies**: drug model files (XML), CDSS configuration file (XML),
  HTML templates (for HTML output).
- **No network access** is required for operating.

Constraints
===========

- The software shall be implemented in C++.
- The software shall not modify the input query file.
- The software shall not require administrator/root privileges for normal operation.
- Drug model files are provided externally and are not part of this software;
  the software shall validate their structure but is not responsible for their
  pharmacological content.


Assumptions and Dependencies
==============================

- Drug model files are present and accessible in the configured directory.
- The XML query conforms to the Tucuxi query schema described in the :ref:`data-formats` section.
- The host system provides a filesystem with read/write access to the configured
  input and output directories.
