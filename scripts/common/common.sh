#!/usr/bin/env bash
set -euo pipefail

: "${REPO_ROOT:?REPO_ROOT must be set by entrypoint}"

# Defaults (do not overwrite if already set)
: "${CONFIG:=debug}"
: "${COVERAGE_MODE:=0}"
: "${BUILD_ROOT:=$REPO_ROOT/build}"
: "${PYTHON_BIN:=python3}"

SCRIPT_DIR="$REPO_ROOT/scripts"

die() {
  local msg="$1"; local code="${2:-1}"
  echo "==> ERROR: $msg" >&2
  exit "$code"
}

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "missing command '$1' (install it and retry)"
}

usage() {
  cat <<'EOF'
Usage:
  ./scripts/run <command> [--debug|--release|--coverage]

By default, commands build in debug mode. Use --release for release mode, or --coverage to build with coverage flags (implies debug).

Commands:
  build           Configure + build main project (root CMake)
  build-gtest     Configure + build gtests (test/gtest CMake)
  run-gtest       Run gtests
  coverage        Run coverage (default: gtests only; see: ./scripts/run coverage --help)
  format          Run clang-format on src
  tidy            Run clang-tidy (root compile_commands.json)
  clean           Remove build dir

  py-env          Print activation command for .venv
  py-deps         Install python deps into .venv
  fuzz            Run fuzzing
  imatinib        Run imatinib system tests
  cb-driver       Run covariate boundaries driver

  help
EOF
}

recompute_paths() {
  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    BUILD_ROOT="$REPO_ROOT/build/coverage"
    MAIN_BUILD_DIR="$BUILD_ROOT/main"
    GTEST_BUILD_DIR="$BUILD_ROOT/gtest"
  else
    BUILD_ROOT="$REPO_ROOT/build"
    MAIN_BUILD_DIR="$BUILD_ROOT/$CONFIG/main"
    GTEST_BUILD_DIR="$BUILD_ROOT/$CONFIG/gtest"
  fi

  CDSS_CLI="$MAIN_BUILD_DIR/tucuxi_cdss_cli"
}

parse_global_opts() {
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --debug)    CONFIG="debug"; shift ;;
      --release)  CONFIG="release"; shift ;;
      --coverage) COVERAGE_MODE=1; shift ;;
      *) break ;;
    esac
  done
  recompute_paths
}