# Contributing to Tucuxi-CDSS-Core

Thank you for your interest in contributing! This document describes the conventions and workflow expected from contributors.

Full documentation is available in the [`docs/`](docs/) folder and online at https://tucuxi-cdss-core.readthedocs.io/en/latest/.

---

## Table of Contents

- [Setting Up Your Development Environment](#setting-up-your-development-environment)
- [Branching and Workflow](#branching-and-workflow)
- [Commit Messages](#commit-messages)
- [Code Style](#code-style)
- [Git Hooks](#git-hooks)
- [Tests](#tests)
- [Submitting Changes](#submitting-changes)

---

## Setting Up Your Development Environment

### Prerequisites

Ensure your system meets the [requirements listed in the README](README.md#requirements) before proceeding.

### First-time Setup

After cloning the repository (with submodules), run the setup command to install the Git hooks:

```sh
git clone --recursive <repository-url>
cd tucuxi-cdss-core
./scripts/linux/run setup
```

`setup` copies the hooks from `scripts/hooks/` into `.git/hooks/`. These hooks enforce code formatting and commit message conventions automatically on every commit — **this step is mandatory**.

On Windows, use the equivalent PowerShell script:

```powershell
.\scripts\win\run.ps1 setup
```

### Building

```sh
./scripts/linux/run build          # debug build (default)
./scripts/linux/run build --release  # release build
```

See `./scripts/linux/run help` for the full list of available commands.

---

## Branching and Workflow

- Target branch for contributions: `main` (or the branch specified in the issue/PR).
- Create a feature or fix branch off `main`:
  ```sh
  git checkout -b feat/my-feature
  ```
- Keep commits focused and atomic. One logical change per commit.

---

## Commit Messages

Commit messages must follow the [Conventional Commits](https://www.conventionalcommits.org/) specification. The **commit-msg** hook installed by `setup` enforces this automatically.

### Format

```
type(scope)?: subject
```

- **type** — one of: `build`, `chore`, `ci`, `docs`, `feat`, `fix`, `perf`, `refactor`, `revert`, `style`, `test`
- **scope** — optional, lowercase, e.g. `flow`, `exporter`, `query`
- **subject** — imperative mood, no trailing period

### Examples

```
feat(exporter): add JSON report export
fix(flow): handle null dose in pipeline
docs: update building instructions in README
refactor(query)!: rename XpertQueryData fields

BREAKING CHANGE: XpertQueryData::getDrug() renamed to getDrugModel()
```

### Rules

- The header must match `type(scope)?: subject` exactly.
- Breaking changes require a `!` after the type/scope **and** a `BREAKING CHANGE: ...` footer.
- Merge commits are exempted from validation automatically.

---

## Code Style

All C++ code must comply with the project's [`.clang-format`](.clang-format) configuration and the naming conventions enforced by [`.clang-tidy`](.clang-tidy).

### Formatting

Key conventions from `.clang-format`:

- Brace style: **Allman** (`BraceWrapping: Custom` with braces on their own line for classes, functions, enums, structs, unions, and control flow).
- Indentation: **4 spaces** (no tabs).
- Column limit: see `.clang-format` for the configured value.
- Template declarations always broken onto their own line (`AlwaysBreakTemplateDeclarations: true`).
- No bin-packing of arguments or parameters.

### Naming Conventions

The following naming rules are enforced by the `readability-identifier-naming` check in `.clang-tidy`:

| Symbol | Convention | Example |
|--------|-----------|---------|
| Classes | `CamelCase` | `XpertQueryImport` |
| Enums | `CamelCase` | `RequestStatus` |
| Enum constants | `CamelCase` | `RequestStatus::Ok` |
| Member variables | `camelBack` with prefix `m_` | `m_drugModel` |
| Local variables | `camelBack` | `patientAge` |
| Parameters | `camelBack` with prefix `_` | `_queryData` |
| Static constants | `UPPER_CASE` | `MAX_RETRIES` |
| Global constants | `UPPER_CASE` | `DEFAULT_TIMEOUT` |

### Checking Formatting

The **pre-commit** hook (installed by `setup`) runs `clang-format --dry-run -Werror` on every staged `.c`, `.cpp`, `.h`, and `.hpp` file. A commit is rejected if any file does not comply.

To check formatting manually without committing:

```sh
./scripts/linux/run format
```

> **Note**: `format` rewrites files in place. Review the diff with `git diff` before staging.

To check a single file without modifying it:

```sh
clang-format --dry-run -Werror src/tucucdss-core/computer.cpp
```

### Running clang-tidy

To run the full static analysis and naming convention checks:

```sh
./scripts/linux/run tidy
```

The report is written to `build/tidy-report.txt`. Address any `readability-identifier-naming` or other reported warnings before submitting a pull request.

---

## Git Hooks

The hooks in `scripts/hooks/` are installed into `.git/hooks/` by `./scripts/linux/run setup`.

| Hook | Purpose |
|------|---------|
| `pre-commit` | Rejects commits where staged C/C++ files fail `clang-format` validation |
| `commit-msg` | Rejects commit messages that do not follow the Conventional Commits format |

If a commit is rejected, fix the reported issue and recommit. Do **not** bypass hooks with `--no-verify` unless there is a documented exceptional reason.

---

## Tests

The project uses [Google Test](https://github.com/google/googletest) as its unit testing framework. Test sources live in [`test/gtest/src/`](test/gtest/src/).

### Requirement

Every new feature **must** be accompanied by at least one unit test that validates its behaviour. A pull request that adds functionality without a corresponding test will not be merged.

### Running the Tests

```sh
./scripts/linux/run build-unittest   # build the test suite
./scripts/linux/run run-unittest     # run all tests
```

Or with coverage instrumentation:

```sh
./scripts/linux/run coverage
```

### Writing Tests

- Add a new file `test/gtest/src/gtest_<module>.cpp` (following the existing naming convention) or extend an existing file if the new test clearly belongs to an existing module.
- Each test file should include the relevant header(s) under test and use standard `TEST` / `TEST_F` macros.
- Register the new file in [`test/gtest/CMakeLists.txt`](test/gtest/CMakeLists.txt) if it is a new source file.

---

## Submitting Changes

1. Ensure your branch builds cleanly: `./scripts/linux/run build`
2. Ensure all tests pass: `./scripts/linux/run run-unittest`
3. Open a pull request against `main` with a clear description of what the change does and why.
4. Reference any related issue in the PR description (e.g., `Closes #42`).
5. Add yourself to [CONTRIBUTORS.md](CONTRIBUTORS.md) if this is your first contribution.
