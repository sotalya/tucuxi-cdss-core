.. SPDX-License-Identifier: AGPL-3.0-or-later

Software Requirements Specification (SRS)
#########################################

Introduction
************

.. contents:: Contents
   :depth: 2
   :backlinks: top
   :local:


Purpose
=======

This chapter presents the Software Requirements Specification (|SRS|) for Tucuxi-CDSS. It defines the requirements that
the software shall satisfy, in "light" accordance with |IEC62304| §5.2. For now
the software is not certified as a medical device, but we are following the
standard to ensure a robust development process and facilitate future
potential certification if desired.

This document is intended for:

- Software developers implementing the Tucuxi-CDSS core
- Software architects producing the Software Design Specification [|SDS|]


Scope
=====

This |SRS| covers the requirements for the Tucuxi-CDSS core software, including:

- The command-line interface (|CLI|) tool :program:`tucuxi_cdss_cli`
- Processing of XML queries (patient data, dosage history, computation requests)
- Pharmacokinetic (|PK|) computation (dosage adjustment, including
  the underlying concentration predictions and percentiles)
- Drug model loading and validation
- Output generation (XML response, HTML report)
- Template-based HTML report rendering

The following are out of scope:

- Drug model file content and pharmacological validity (covered by model validation)
- Third-party libraries
- Deployment, infrastructure, and network interfaces

Document Overview
=================

This chapter is organized as follows:

- :ref:`srsOverallDescription` — Software context, high-level functions, user
  characteristics, constraints, and assumptions.
- :ref:`srsFunctionalRequirements` — Detailed functional requirements grouped by
  subsystem.
- :ref:`srsInterfaceRequirements` — External and internal interface requirements.
- :ref:`srsReliabilityRequirements` — Reliability and portability requirements.
- :ref:`srsSafetyRequirements` — Safety requirements derived from risk analysis
  (|IEC62304| §5.2.2).
- :ref:`srsVerification` — Requirements verification matrix.
