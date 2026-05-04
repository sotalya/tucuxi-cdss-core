# Tucuxi-CDSS-Core

[![License: AGPL v3](https://img.shields.io/badge/license-AGPL--3.0--or--later-blue.svg)](https://www.gnu.org/licenses/agpl-3.0.html)

A configurable **Clinical Decision Support System (CDSS)** for **Model-Informed Precision Dosing (MIPD)** and **Therapeutic Drug Monitoring (TDM)**. Given a patient's drug treatment history and pharmacokinetic/pharmacodynamic (PK/PD) drug models, Tucuxi-CDSS-Core predicts drug concentrations in blood and automatically proposes dosage adaptations. The output consists in an HTML report or of a more structure XML file.

This is part of the Tucuxi ecosystem. It wraps [tucuxi-core](https://github.com/sotalya/tucuxi-core) (the PK computation engine) and [tucuquery](libs/tucuxi-core) (the query layer) with a higher-level Xpert query/result pipeline suited for integration into hospital clinical decision support workflows.

> **Note**: This project uses Git submodules. Always clone with `--recursive`:
> ```sh
> git clone --recursive <repository-url>
> ```

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Documentation](#documentation)
- [Requirements](#requirements)
- [Building](#building)
  - [Linux](#linux)
  - [Windows](#windows)
- [Running the Tests](#running-the-tests)
- [Usage](#usage)
- [Citation](#citation)
- [Authors](#authors)
- [License](#license)

---

## Features

- Drug concentration **prediction** using population PK/PD models
- Automatic **dosage adaptation** proposals (a posteriori Bayesian estimation)
- Configurable per-drug decision rules via XML configuration files
- **Multi-drug, multi-request** query processing in a single call
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

| Section | Content |
|---|---|
| [Introduction](docs/source/01_introduction/) | Project background and motivation |
| [Overview](docs/source/02_overview/) | High-level system description |
| [Architecture](docs/source/03_architecture/) | Component design and data flow |
| [Installation](docs/source/04_installation/) | Detailed setup instructions |
| [Configuration](docs/source/05_configuration/) | Drug model and system configuration |
| [User Guide](docs/source/06_user_guide/) | How to run queries and interpret results |
| [Data](docs/source/07_data/) | Input/output data formats |
| [SRS](docs/source/08_srs/) | Software Requirements Specification |
| [SDS](docs/source/09_sds/) | Software Design Specification |
| [V&V Plan](docs/source/10_vvp/) | Verification & Validation Plan |
| [Testing](docs/source/11_testing/) | Test strategy and results |

To build the HTML documentation locally:
```sh
./scripts/linux/run doc
```
The output is written to `build/docs/`.

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

**Linux** — quick start:
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

Full build and CI documentation is in [`scripts/linux/common/common.sh`](scripts/linux/common/common.sh) and the per-command scripts under [`scripts/linux/commands/`](scripts/linux/commands/).

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
./tucuxi_cdss_cli --config config.xml --query patient_query.tqf --output report.html
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

- Yann Thoma — [yann.thoma@heig-vd.ch](mailto:yann.thoma@heig-vd.ch) — HEIG-VD, HES-SO
- Roberto Rigamonti — [roberto.rigamonti@heig-vd.ch](mailto:roberto.rigamonti@heig-vd.ch) — HEIG-VD, HES-SO
- Bruno Da Rocha Carvalho — [bruno.darochacarvalho@heig-vd.ch](mailto:bruno.darochacarvalho@heig-vd.ch) — HEIG-VD, HES-SO
- Melvyn Herzig
- Annie E. Cathignol
- Yuan J. Pétermann

See [AUTHORS.md](AUTHORS.md) and [CONTRIBUTORS.md](CONTRIBUTORS.md) for the full list.

This work is funded by the **Swiss National Science Foundation**.

---

## License

This project is licensed under the **GNU Affero General Public License v3.0 or later** (AGPL-3.0-or-later).

See [LICENSE](LICENSE) for the full text.

This project includes third-party components. See [NOTICE.txt](NOTICE.txt) for details.
