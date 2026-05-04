# Tucuxi-CDSS-Core

[![License: AGPL v3](https://img.shields.io/badge/license-AGPL--3.0--or--later-blue.svg)](https://www.gnu.org/licenses/agpl-3.0.html)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.20024722.svg)](https://doi.org/10.5281/zenodo.20024722)[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/12192/badge)](https://www.bestpractices.dev/projects/12192)[![fair-software.eu](https://img.shields.io/badge/fair--software.eu-%E2%97%8F%20%20%E2%97%8F%20%20%E2%97%8B%20%20%E2%97%8F%20%20%E2%97%8B-orange)](https://fair-software.eu)

A configurable **Clinical Decision Support System (CDSS)** for **Model-Informed Precision Dosing (MIPD)** and **Therapeutic Drug Monitoring (TDM)**. Given a patient's drug treatment history and pharmacokinetic (PK) drug models, Tucuxi-CDSS-Core predicts drug concentrations in blood and automatically proposes dosage adaptations. The output consists in an HTML report or of a more structure XML file.

This is part of the Tucuxi ecosystem. It wraps [tucuxi-core](https://github.com/sotalya/tucuxi-core) (the PK computation engine) and [tucuquery](libs/tucuxi-core) (the query layer) with a higher-level Xpert query/result pipeline suited for integration into hospital clinical decision support workflows.

> **Note**: This project uses Git submodules. Always clone with `--recursive`:
> ```sh
> git clone --recursive <repository-url>
> ```

The last documentation version is available here: https://tucuxi-cdss-core.readthedocs.io/en/latest/

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Documentation](#documentation)
- [Requirements](#requirements)
- [Building](#building)
- [Running the Tests](#running-the-tests)
- [Usage](#usage)
- [Citation](#citation)
- [Authors](#authors)
- [License](#license)

---

## Features

- Drug concentration **prediction** using population PK models
- Automatic **dosage adaptation** proposals (a posteriori Bayesian estimation)
- Configurable per-drug decision rules via XML configuration files
- HTML report generation from customisable Jinja2-style templates
- Cross-platform: Linux, Windows, macOS

---

## Architecture

The project is structured as follows:

```
src/
  tucucdss-core/
    computer.*          # Top-level computation orchestrator
    query/              # XpertQuery data, import, and core extraction
    result/             # XpertQueryResult and XpertRequestResult
    flow/               # Processing pipeline (steps, flow controller)
    exporter/           # HTML/JSON report exporters
    language/           # Internationalisation helpers
    utils/              # Shared utilities
  tucucdss-cli.cpp      # Command-line interface entry point
libs/
  tucuxi-core/          # Git submodule: PK/PD engine + tucuquery
test/
  gtest/                # Google Test unit and integration tests
  system/               # System / integration tests
  fuzzing/              # Fuzz tests
  drugfiles/            # Drug model XML files used by tests
```

Queries are expressed as XML documents following the Tucuxi query schema. The `XpertQueryImport` class parses them into `XpertQueryData` objects; `Computer` drives the full pipeline and produces an `XpertQueryResult` containing per-request `XpertRequestResult` objects, which are then rendered to HTML or JSON by the exporters.

---

## Documentation

Full documentation is located in the [`docs/`](docs/) folder and is built with [Sphinx](https://www.sphinx-doc.org/). It covers:

To build the HTML documentation locally:
```sh
./scripts/linux/run doc
```
The output is written to `build/docs/`.

You can however access the last documenation build here: https://tucuxi-cdss-core.readthedocs.io/en/latest/

---

## Requirements

| Dependency | Version | Notes |
|---|---|---|
| C++ compiler | C++17 | GCC ≥ 9, Clang ≥ 10, MSVC 2019+ |
| CMake | ≥ 3.16 | |
| Ninja *(Windows)* | any | Recommended generator on Windows |
| wkhtmltopdf / wkhtmltox | ≥ 0.12 | Optional, for PDF export |
| Google Test | bundled | Only needed for tests |

All other dependencies (nlohmann/json, inja, tucuxi-core) are included as submodules or vendored under `libs/`.

---

## Building

The project ships a `run` helper script that wraps all build, test, and tooling commands.

**Linux/MacOS** — quick start:
```sh
./scripts/linux/run setup    # copy git hooks
./scripts/linux/run build    # configure + build (debug by default)
```

**Windows** — equivalent PowerShell script:
```powershell
.\scripts\win\run.ps1 setup
.\scripts\win\run.ps1 build
```

Pass `--release` for a release build or `--coverage` for a coverage-instrumented build.

For the full list of available commands (`build`, `build-unittest`, `run-unittest`, `coverage`, `format`, `tidy`, `doc`, `scan-build`, …) run:
```sh
./scripts/linux/run help
```

---

## Running the Tests

```sh
./scripts/linux/run build-unittest   # build the test suite
./scripts/linux/run run-unittest     # run all tests
```

Or with coverage instrumentation:
```sh
./scripts/linux/run coverage
```

See `./scripts/linux/run help` for additional options.

---

## Usage

The CLI accepts a Tucuxi XML query file and writes an HTML report:

```sh
./tucuxi_cdss_cli --config config.xml --query patient_query.tqf --output <output_folder>
```

For integration into other applications, link against the `tucucdss-core` CMake target and use the `Tucuxi::Xpert` API directly:

```cpp
#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/computer.h"

std::unique_ptr<Tucuxi::Xpert::XpertQueryData> queryData;
Tucuxi::Xpert::XpertQueryImport importer;
auto status = importer.importFromFile(queryData, "patient_query.tqf");
if (status == Tucuxi::Xpert::XpertQueryImport::Status::Ok) {
    Tucuxi::Xpert::Computer computer;
    auto result = computer.compute(std::move(queryData));
    // render result to HTML ...
}
```

---

## Citation

If you use this software in your research, please cite the forthcoming paper:

> Rigamonti R., Pétermann Y.J., Herzig M., Cathignol A.E., Da Rocha Carvalho B., Mpagama S.G., Said B., Sariko M.L., Csajka C., Guidi M., Thoma Y. *A Configurable Clinical Decision Support System for Model-Informed Precision Dosing: System Architecture and Operational Principles.* (2026, under review)

See [CITATION.cff](CITATION.cff) for a machine-readable citation.

---

## Authors

- Yann Thoma — HEIG-VD, HES-SO (maintainer)
- Roberto Rigamonti — HEIG-VD, HES-SO
- Bruno Da Rocha Carvalho — HEIG-VD, HES-SO
- Melvyn Herzig
- Annie E. Cathignol

See [AUTHORS.md](AUTHORS.md) and [CONTRIBUTORS.md](CONTRIBUTORS.md) for the full list.

The initial version ot this work has funded by the **Swiss National Science Foundation**.

---

## License

This project is licensed under the **GNU Affero General Public License v3.0 or later** (AGPL-3.0-or-later).

See [LICENSE](LICENSE) for the full text.

This project includes third-party components. See [NOTICE.txt](NOTICE.txt) for details.
