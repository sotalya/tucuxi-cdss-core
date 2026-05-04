.. SPDX-License-Identifier: AGPL-3.0-or-later

Limitations
===========

- **Adjustment only**: the system computes dosage adjustments; it does not expose
  standalone prediction, simulation, or parameter-estimation capabilities.
- **Single drug per request**: each ``<xpertRequest>`` addresses exactly one
  drug. Multi-drug patients require one request per drug.
- **No network interface**: the system communicates exclusively through files.
  Real-time integration requires the calling application to manage file I/O.
- **Drug-model dependent**: the quality of recommendations depends entirely on
  the quality and appropriateness of the |popPK| model. The system validates the
  model structure but cannot assess its pharmacological correctness.

