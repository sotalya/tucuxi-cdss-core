#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/common/common.sh"
source "$REPO_ROOT/scripts/common/cmake.sh"

cmd_imatinib() {
  echo "==> Running imatinib system tests ($CONFIG)"
  ensure_cli_built

  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    OUT_DIR="$BUILD_ROOT/system/imatinib/output"
  else
    OUT_DIR="$BUILD_ROOT/$CONFIG/system/imatinib/output"
  fi
  mkdir -p "$OUT_DIR"

  "$PYTHON_BIN" "$REPO_ROOT/test/system/imatinib/imatinib_tests.py" \
    "$REPO_ROOT/config.xml" \
    "$REPO_ROOT/test/system/imatinib" \
    --cdss-cli "$CDSS_CLI" \
    --drug-files-dir "$REPO_ROOT/../tucuxi-drugs/drugfiles" \
    --language-files-dir "$REPO_ROOT/language" \
    --template-dir "$MAIN_BUILD_DIR/html_template" \
    --template-name "template" \
    --out-dir "$OUT_DIR"

  echo "==> Output: $OUT_DIR"
}