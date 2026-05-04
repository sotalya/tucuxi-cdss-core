.. SPDX-License-Identifier: AGPL-3.0-or-later

Build and Test Script (run)
===========================

The :file:`scripts/run` script is the unified interface for building, testing, and maintaining the Tucuxi-CDSS project. It provides a consistent way to perform common development tasks across different platforms.

Overview
--------

The script is located at the repository root:

.. code-block:: bash

   ./scripts/linux/run <command> [options]

Global Options
--------------

The following global options can be passed to any command to control the build configuration:

+------------------+----------------------------------------------------------+
| Option           | Description                                              |
+==================+==========================================================+
| ``--debug``      | Build in debug mode (default)                            |
+------------------+----------------------------------------------------------+
| ``--release``    | Build in release mode                                    |
+------------------+----------------------------------------------------------+
| ``--coverage``   | Build with coverage instrumentation (implies debug)      |
+------------------+----------------------------------------------------------+

Commands
--------

Help
~~~~

Display available commands and usage information:

.. code-block:: bash

   ./scripts/linux/run help
   ./scripts/linux/run --help


Hooks
~~~~~~
Install git hooks for the repository:
.. code-block:: bash

   ./scripts/linux/run setup

Build
~~~~~

Configure and build the main project:

.. code-block:: bash

   ./scripts/linux/run build                    # Debug build (default)
   ./scripts/linux/run build --release          # Release build
   ./scripts/linux/run build --coverage         # Build with coverage flags

Output location:

- Debug:    :file:`build/debug/main/tucuxi_cdss_cli`
- Release:  :file:`build/release/main/tucuxi_cdss_cli`
- Coverage: :file:`build/coverage/main/tucuxi_cdss_cli`

Unit Tests (GoogleTest)
~~~~~~~~~~~~~~~~~~~~~~~

Build the unit test suite:

.. code-block:: bash

   ./scripts/linux/run build-unittest              # Debug build (default)
   ./scripts/linux/run build-unittest --release    # Release build

Run the unit tests:

.. code-block:: bash

   ./scripts/linux/run run-unittest               # Debug build (default)
   ./scripts/linux/run run-unittest --release      # Release build

If the test binary does not exist, it will be built automatically before running.

Code Coverage
~~~~~~~~~~~~~

Generate code coverage reports:

.. code-block:: bash

   ./scripts/linux/run coverage --help          # Show coverage options

Basic usage (runs gtests only):

.. code-block:: bash

   ./scripts/linux/run coverage

Run coverage with specific test suites:

.. code-block:: bash

   ./scripts/linux/run coverage --unittest      # Run gtests only (default)
   ./scripts/linux/run coverage --system        # Run all system tests
   ./scripts/linux/run coverage --all           # Run gtests + all system tests

Run only specific system tests:

.. code-block:: bash

   ./scripts/linux/run coverage --only fuzz
   ./scripts/linux/run coverage --only imatinib
   ./scripts/linux/run coverage --only clibase
   ./scripts/linux/run coverage --only cb-driver

Combine tests:

.. code-block:: bash

   ./scripts/linux/run coverage --unittest --fuzz --imatinib --clibase
   ./scripts/linux/run coverage --no-unittest --fuzz --cb-driver

Append to an existing coverage build:

Use ``--append`` to preserve the existing :file:`build/coverage/` directory
instead of deleting it before the run. This is useful when you want to execute
several coverage commands in sequence and accumulate their execution data into a
single coverage report.

.. code-block:: bash

   ./scripts/linux/run coverage --append --only clibase
   ./scripts/linux/run coverage --append --system

Setup Python environment for coverage:

.. code-block:: bash

   COVERAGE_SETUP_PY=1 ./scripts/linux/run coverage --all

The coverage report is generated at: :file:`build/coverage/html/index.html`

Python Environment
~~~~~~~~~~~~~~~~~~

Get the activation command for the Python virtual environment:

.. code-block:: bash

   ./scripts/linux/run py-env

This prints the command to activate the :file:`.venv` directory:

.. code-block:: bash

   source ".venv/bin/activate"

Install Python dependencies:

.. code-block:: bash

   ./scripts/linux/run py-deps

This creates the virtual environment (if needed) and installs all required dependencies for:

- Fuzzing tests
- Imatinib system tests
- Clibase CLI-input system tests
- Covariate boundaries driver

System Tests
~~~~~~~~~~~~

Run fuzzing tests:

.. code-block:: bash

   ./scripts/linux/run fuzz                    # Debug build
   ./scripts/linux/run fuzz --release          # Release build
   ./scripts/linux/run fuzz --coverage         # Using coverage build

Output: :file:`build/<config>/fuzzing/<timestamp>/output/`

Run Imatinib system tests:

.. code-block:: bash

   ./scripts/linux/run imatinib                # Debug build
   ./scripts/linux/run imatinib --release      # Release build
   ./scripts/linux/run imatinib --coverage     # Using coverage build

Output: :file:`build/<config>/system/imatinib/output/`

Run clibase CLI-input system tests:

.. code-block:: bash

   ./scripts/linux/run clibase                 # Debug build
   ./scripts/linux/run clibase --release       # Release build
   ./scripts/linux/run clibase --coverage      # Using coverage build

Output: :file:`build/<config>/system/clibase/output/`

Run Covariate Boundaries Driver:

.. code-block:: bash

   ./scripts/linux/run cb-driver               # Debug build
   ./scripts/linux/run cb-driver --release     # Release build
   ./scripts/linux/run cb-driver --coverage    # Using coverage build

Output: :file:`build/<config>/cb_driver/out/`

Code Quality Tools
~~~~~~~~~~~~~~~~~~

Format source code with clang-format:

.. code-block:: bash

   ./scripts/linux/run format

Runs :program:`clang-format -i` on all C++ files in :file:`src/`.

Run clang-tidy static analysis:

.. code-block:: bash

   ./scripts/linux/run tidy                    # Debug compile_commands.json
   ./scripts/linux/run tidy --release          # Release compile_commands.json

Output: :file:`build/tidy-report.txt`

Clean
~~~~~

Remove the build directory:

.. code-block:: bash

   ./scripts/linux/run clean

This removes the entire :file:`build/` directory.

Common Workflows
----------------

Initial Setup
~~~~~~~~~~~~~

.. code-block:: bash

   # Build the project
   ./scripts/linux/run build --release

   # Install Python dependencies
   ./scripts/linux/run py-deps

   # Activate virtual environment (optional, for manual Python work)
   eval "$(./scripts/linux/run py-env)"

Development Loop
~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Build and run unit tests
   ./scripts/linux/run build-unittest
   ./scripts/linux/run run-unittest

   # Format code before commit
   ./scripts/linux/run format

   # Run static analysis
   ./scripts/linux/run tidy

Testing
~~~~~~~

.. code-block:: bash

   # Run all unit tests
   ./scripts/linux/run run-unittest

   # Run all system tests
   ./scripts/linux/run fuzz
   ./scripts/linux/run imatinib
   ./scripts/linux/run cb-driver

   # Generate full coverage report
   COVERAGE_SETUP_PY=1 ./scripts/linux/run coverage --all

   # Open coverage report
   xdg-open build/coverage/html/index.html  # Linux
   # open build/coverage/html/index.html    # macOS
   # start build/coverage/html/index.html   # Windows

Release Build
~~~~~~~~~~~~~

.. code-block:: bash

   # Clean previous builds
   ./scripts/linux/run clean

   # Build in release mode
   ./scripts/linux/run build --release

   # Binary location
   ls -la build/release/main/tucuxi_cdss_cli

Environment Variables
---------------------

The following environment variables can be used to customize behavior:

+------------------------+----------------------------------------------------------+
| Variable               | Description                                              |
+========================+==========================================================+
| ``REPO_ROOT``          | Repository root directory (auto-detected)                |
+------------------------+----------------------------------------------------------+
| ``BUILD_ROOT``         | Build directory root (default: :file:`build`)            |
+------------------------+----------------------------------------------------------+
| ``CONFIG``             | Build configuration: ``debug`` or ``release``            |
+------------------------+----------------------------------------------------------+
| ``COVERAGE_MODE``      | Enable coverage mode: ``0`` or ``1``                     |
+------------------------+----------------------------------------------------------+
| ``PYTHON_BIN``         | Python interpreter (default: ``python3``)                |
+------------------------+----------------------------------------------------------+
| ``COVERAGE_SETUP_PY``  | Setup Python venv for coverage: ``0`` or ``1``           |
+------------------------+----------------------------------------------------------+

Error Handling
--------------

The script uses ``set -euo pipefail`` for strict error handling:

- Exits on any command failure
- Exits on undefined variables
- Exits on pipeline failures

Error messages are printed to stderr with the format:

.. code-block:: bash

   ==> ERROR: <message>

Dependencies
------------

The script checks for required commands at runtime:

+-------------------+----------------------------------------------------------+
| Command           | Used by                                                  |
+===================+==========================================================+
| ``cmake``         | build, build-unittest, tidy, coverage                    |
+-------------------+----------------------------------------------------------+
| ``clang-format``  | format                                                   |
+-------------------+----------------------------------------------------------+
| ``clang-tidy``    | tidy                                                     |
+-------------------+----------------------------------------------------------+
| ``run-clang-tidy``| tidy                                                     |
+-------------------+----------------------------------------------------------+
| ``python3``       | py-env, py-deps, fuzz, imatinib, cb-driver, coverage     |
+-------------------+----------------------------------------------------------+
| ``lcov``          | coverage                                                 |
+-------------------+----------------------------------------------------------+
| ``genhtml``       | coverage                                                 |
+-------------------+----------------------------------------------------------+

If a required command is missing, the script exits with an error message
suggesting how to install it.
