.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _sdsIntroduction:

Introduction
************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:

Purpose
=======

This chapter presents the Software Design Specification (|SDS|) for Tucuxi-CDSS.
It describes the software architecture
and the detailed design of the software items that compose the system, in accordance
with |IEC62304| (though Tucuxi-CDSS is not yet meant to be certified according to that norm).

This document is intended for:

- Software developers implementing or maintaining Tucuxi-CDSS
- Software architects reviewing the system design

Scope
=====

This |SDS| covers the design of Tucuxi-CDSS, including:

- The command-line interface (|CLI|) tool :program:`tucuxi_cdss_cli`
- The |CDSS| computation engine
- The drug model loading subsystem
- The query and response XML processing subsystem
- The HTML report generation subsystem and template system

The following are out of scope for this document:

- Drug model files (described in the Drug Model Language specification, see
  `here <https://doc.tucuxi.ch/Tucuxi_DrugFiles_Specification/Tucuxi_DrugModelFile_Specification.html>`__)
- Third-party libraries used by the system
- Deployment and infrastructure


Document Overview
=================

This chapter is organized as follows:

- :ref:`sdsArchitecture` — Describes the overall software architecture, the
  decomposition into software items, and their interactions (|IEC62304| §5.3).
- :ref:`sdsDetailedDesign` — Provides the detailed design of each software item,
  including algorithms and data structures (|IEC62304| §5.4).
- :ref:`sdsInterfaceDesign` — Specifies the external and internal interfaces of
  the system.
- :ref:`sdsSafetyDesign` — Describes how safety requirements and risk control
  measures are addressed in the design.
- :ref:`sdsTraceability` — Provides the traceability matrix linking software
  requirements to design elements.
