#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/common/common.sh"
source "$REPO_ROOT/scripts/common/python.sh"

cmd_coverage() {
  local GTEST_SRC_DIR="$REPO_ROOT/test/gtest"

  local COVERAGE_ROOT="$REPO_ROOT/build/coverage"
  local BUILD_DIR="$COVERAGE_ROOT/gtest"
  local RAW_INFO="$COVERAGE_ROOT/lcov/coverage.raw.info"
  local FILTERED_INFO="$COVERAGE_ROOT/lcov/coverage.info"
  local HTML_DIR="$COVERAGE_ROOT/html"
  local BASE_DIR="$REPO_ROOT"

  # ---- options coverage (locales) ----
  local RUN_GTEST=1
  local RUN_FUZZ=0
  local RUN_IMATINIB=0
  local RUN_CBDRIVER=0

  coverage_usage() {
    cat <<'EOF'
Usage:
  scripts/run coverage [options]

Default:
  Runs gtests only.

Options:
  --gtest              Run gtests (default ON unless --only is used)
  --no-gtest           Do not run gtests
  --fuzz               Add fuzz system tests
  --imatinib           Add imatinib system tests
  --cb-driver          Add covariate boundaries driver system tests
  --system             Shortcut for --fuzz --imatinib --cb-driver
  --all                Shortcut for --gtest --system
  --only <what>        Run only one target (no gtests):
                       <what> = gtest | fuzz | imatinib | cb-driver | system
  -h, --help           Show this help

Env:
  COVERAGE_SETUP_PY=1  Create/ensure venv + install deps 
EOF
  }

  # Parse args passed to "coverage" command
  local -a args=("$@")
  while [[ $# -gt 0 ]]; do
    case "$1" in
      -h|--help)
        echo "$BUILD_DIR"
        coverage_usage
        return 0
        ;;
      --gtest)
        RUN_GTEST=1
        shift
        ;;
      --no-gtest)
        RUN_GTEST=0
        shift
        ;;
      --fuzz)
        RUN_FUZZ=1
        shift
        ;;
      --imatinib)
        RUN_IMATINIB=1
        shift
        ;;
      --cb-driver)
        RUN_CBDRIVER=1
        shift
        ;;
      --system)
        RUN_FUZZ=1
        RUN_IMATINIB=1
        RUN_CBDRIVER=1
        shift
        ;;
      --all)
        RUN_GTEST=1
        RUN_FUZZ=1
        RUN_IMATINIB=1
        RUN_CBDRIVER=1
        shift
        ;;
      --only)
        shift
        if [[ $# -lt 1 ]]; then
          die "--only requires an argument (gtest|fuzz|imatinib|cb-driver|system)" 2
        fi
        RUN_GTEST=0
        RUN_FUZZ=0
        RUN_IMATINIB=0
        RUN_CBDRIVER=0
        case "$1" in
          gtest) RUN_GTEST=1 ;;
          fuzz) RUN_FUZZ=1 ;;
          imatinib) RUN_IMATINIB=1 ;;
          cb-driver) RUN_CBDRIVER=1 ;;
          system)
            RUN_FUZZ=1
            RUN_IMATINIB=1
            RUN_CBDRIVER=1
            ;;
          *) die "Unknown --only target: $1" 2 ;;
        esac
        shift
        ;;
      *)
        die "Unknown coverage option: $1 (try: scripts/run coverage --help)" 2
        ;;
    esac
  done

  if [[ $RUN_GTEST -eq 0 && $RUN_FUZZ -eq 0 && $RUN_IMATINIB -eq 0 && $RUN_CBDRIVER -eq 0 ]]; then
    die "Nothing to run. Use --gtest and/or --fuzz/--imatinib/--cb-driver (or --all)." 2
  fi

  echo "==> Coverage: prerequisites"
  require_cmd cmake
  require_cmd lcov
  require_cmd genhtml
  require_cmd python3

  local PY_RUN=""
  if [[ "${COVERAGE_SETUP_PY:-0}" == "1" ]]; then
    echo "==> Coverage: setting up python venv + deps (COVERAGE_SETUP_PY=1)"
    ensure_venv
    "$VENV_PIP" install -r "$REPO_ROOT/test/fuzzing/requirements.txt"
    "$VENV_PIP" install -r "$REPO_ROOT/test/covariate_boundaries_driver/requirements.txt"
    "$VENV_PIP" install -r "$REPO_ROOT/test/system/imatinib/requirements.txt"
    PY_RUN="$VENV_PY"
  else
    if [[ -x "$VENV_PY" ]]; then
      PY_RUN="$VENV_PY"
    else
      PY_RUN="${PYTHON_BIN:-python3}"
    fi
  fi
  
  echo "==> Coverage: clean coverage dirs"
  rm -rf "$COVERAGE_ROOT"
  mkdir -p "$BUILD_DIR" "$(dirname "$RAW_INFO")" "$HTML_DIR"

  if [[ $RUN_GTEST -eq 1 ]]; then
    echo "==> Coverage: configure gtests (coverage flags ON)"
    cmake -S "$GTEST_SRC_DIR" -B "$BUILD_DIR" -Dconfig_coverage=ON -DCMAKE_BUILD_TYPE=Debug

    echo "==> Coverage: build gtests"
    cmake --build "$BUILD_DIR" -j"$(nproc)"

    echo "==> Coverage: run gtests"
    set +e
    "$BUILD_DIR/tucu-cdss-tests"
    local TEST_RC=$?
    set -e
    if [[ $TEST_RC -ne 0 ]]; then
      echo "==> WARN: gtests returned non-zero ($TEST_RC). Generating coverage anyway."
    fi
  else
    echo "==> Coverage: skipping gtests"
  fi

  # ---- system tests (python) ----
  export PYTHON_BIN="$PY_RUN"
  local FUZZ_RC=0
  local IMATINIB_RC=0
  local CBDRIVER_RC=0

  set +e
  if [[ $RUN_FUZZ -eq 1 ]]; then
    echo "==> Coverage: run fuzz system tests (python) using coverage build"
    "$REPO_ROOT/scripts/run" fuzz --coverage
    FUZZ_RC=$?
  fi

  if [[ $RUN_IMATINIB -eq 1 ]]; then
    echo "==> Coverage: run imatinib system tests (python) using coverage build"
    "$REPO_ROOT/scripts/run" imatinib --coverage
    IMATINIB_RC=$?
  fi

  if [[ $RUN_CBDRIVER -eq 1 ]]; then
    echo "==> Coverage: run cb-driver system tests (python) using coverage build"
    "$REPO_ROOT/scripts/run" cb-driver --coverage
    CBDRIVER_RC=$?
  fi
  set -e

  if [[ $RUN_FUZZ -eq 1 && $FUZZ_RC -ne 0 ]]; then
    echo "==> WARN: fuzz returned non-zero ($FUZZ_RC)."
  fi
  if [[ $RUN_IMATINIB -eq 1 && $IMATINIB_RC -ne 0 ]]; then
    echo "==> WARN: imatinib returned non-zero ($IMATINIB_RC)."
  fi
  if [[ $RUN_CBDRIVER -eq 1 && $CBDRIVER_RC -ne 0 ]]; then
    echo "==> WARN: cb-driver returned non-zero ($CBDRIVER_RC)."
  fi

  lcov_version() {
    lcov --version | grep -Eo '([0-9]+\.[0-9]+(\.[0-9]+)?)' | head -n1
  }

  version_ge_115() {
    local v="$1"
    [[ "$(printf "1.15\n%s\n" "$v" | sort -V | head -n1)" == "1.15" ]]
  }

  local LCOV_VER
  LCOV_VER="$(lcov_version)"

  local -a LCOV_IGNORE_ARGS=()
  if version_ge_115 "$LCOV_VER"; then
    echo "==> lcov version $LCOV_VER (>= 1.15): enabling --ignore-errors negative,mismatch,gcov"
    LCOV_IGNORE_ARGS=(--ignore-errors negative,mismatch,gcov)
  else
    echo "==> lcov version $LCOV_VER (< 1.15): running without --ignore-errors"
  fi

  echo "==> Coverage: capture lcov"
  set +e
  lcov --capture \
    --directory "$COVERAGE_ROOT" \
    --base-directory "$BASE_DIR" \
    "${LCOV_IGNORE_ARGS[@]}" \
    --no-external \
    --output-file "$RAW_INFO" >/dev/null
  local LCOV_RC=$?
  set -e
  if [[ $LCOV_RC -ne 0 ]]; then
    echo "==> WARN: lcov returned non-zero ($LCOV_RC). Continuing."
  fi

  echo "==> Coverage: filter report (keep only project code)"
  lcov --extract "$RAW_INFO" \
    "$REPO_ROOT/src/*" \
    --output-file "$FILTERED_INFO" >/dev/null

  echo "==> Coverage: generate HTML"
  rm -rf "$HTML_DIR"
  genhtml "$FILTERED_INFO" --output-directory "$HTML_DIR" >/dev/null

  echo "==> DONE: Coverage report at:"
  echo "   $HTML_DIR/index.html"
}