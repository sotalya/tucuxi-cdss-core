.. SPDX-License-Identifier: AGPL-3.0-or-later

Installation and Deployment
###########################

System Requirements
*******************

Tucuxi-CDSS is written in C++ and is cross-platform.  The minimum
build requirements are:

- A C++17-compliant compiler (GCC >= 9, Clang >= 10, or MSVC >= 19.20).
- **CMake** >= 3.16.
- Standard POSIX shell or PowerShell for invoking build scripts.

For running the Python-based test suites (system tests, fuzzing,
covariate boundary driver):

- **Python** >= 3.9.
- **pip** (to install test-specific dependencies from
  ``requirements.txt``).

Dependencies
************

Runtime dependencies
====================

The following assets must be present at runtime.  Their paths are
supplied as command-line arguments to the :program:`tucuxi_cdss_cli` binary
(see :doc:`../06_user_guide/running_tucucdsscli`):

.. csv-table::
   :header: "Asset", "Typical location", "CLI flag"
   :widths: 25, 35, 20

   "Drug model files",      "``tucuxi-drugs/drugfiles/``",               "``-d``"
   "|CDSS| configuration",  "``config.xml``",              "``-c``"
   "Language files",        "``tucuxi-cdss-core/language/``",            "``-l``"
   "HTML template",         "``tucuxi-cdss-core/html_template/<name>/``","``-p / -t``"

The drug model files are maintained in a separate repository
(``tucuxi-drugs``) and must be checked out alongside the |CDSS| source.
The configuration and language files are bundled with the |CDSS|
repository :cite:`TucuxiCDSSGitHub`.

Build-time dependencies
=======================

- **Tucuxi-Core** :cite:`TucuxiCore2025`: the computational engine;
  fetched automatically via CMake's ``FetchContent`` mechanism.
- **GoogleTest**: the unit-test framework.

Compilation
***********

The project provides a unified build script :file:`scripts/run` that handles all build operations.

Using the Build Script
======================

Basic Build
~~~~~~~~~~~

To configure and build the main project:

.. code-block:: bash

   ./scripts/linux/run build

This configures the project with CMake and builds the :program:`tucuxi_cdss_cli` binary in debug mode.

Build Modes
~~~~~~~~~~~

The script supports three build configurations via global options:

.. code-block:: bash

   ./scripts/linux/run build --debug     # Debug build (default)
   ./scripts/linux/run build --release   # Release build
   ./scripts/linux/run build --coverage  # Debug build with coverage flags

Build Output
~~~~~~~~~~~~

By default, the binary is produced at:

- Debug:    :file:`build/debug/main/tucuxi_cdss_cli`
- Release:  :file:`build/release/main/tucuxi_cdss_cli`
- Coverage: :file:`build/coverage/main/tucuxi_cdss_cli`

Complete list of available commands is shown with:

.. code-block:: bash

   ./scripts/linux/run help

Minimal deployment checklist
=============================

1. Copy or symlink :program:`build/tucuxi_cdss_cli` to a directory on the
   system ``PATH`` (e.g., ``/usr/local/bin/``).
2. Clone or copy the ``tucuxi-drugs`` drug model directory to a stable
   path.
3. Verify that the CDSS configuration file (``config.xml``) has been
   populated with institution-specific covariate thresholds and
   computational limits (see :doc:`../05_configuration/configuration`).
4. Verify that language files and an HTML template directory exist and
   are readable by the process running the binary.
5. Run a smoke test using one of the provided ``.tqf`` test inputs
   (see :doc:`../11_testing/cdss_tests`):

.. code-block:: bash

   # Build first if not already done
   ./scripts/linux/run build --release

   # Run a smoke test
   ./build/release/main/tucuxi_cdss_cli \
       -d tucuxi-drugs/drugfiles \
       -i test/system/imatinib/imatinib_correct_sample.tqf \
       -c config.xml \
       -o /tmp/cdss_smoke \
       -t default

   A return code of ``0`` and the presence of an HTML file in
   ``/tmp/cdss_smoke/`` confirm a working installation.
