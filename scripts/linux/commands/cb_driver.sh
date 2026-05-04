#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_cb_driver() {
  echo "==> Running cb-driver ($CONFIG)"
  ensure_cli_built

  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    OUT_BASE="$BUILD_ROOT/cb_driver"
  else
    OUT_BASE="$BUILD_ROOT/$CONFIG/cb_driver"
  fi
  
  mkdir -p "$OUT_BASE"

  "$PYTHON_BIN" "$REPO_ROOT/test/covariate_boundaries_driver/cb_driver.py" \
    "$REPO_ROOT/config.xml" \
    "$REPO_ROOT/test/covariate_boundaries_driver/imatinib_template.tqf" \
    --cdss-cli "$MAIN_BUILD_DIR/tucuxi_cdss_cli" \
    --drug-files-dir "$REPO_ROOT/test/drugfiles" \
    --language-files-dir "$REPO_ROOT/language" \
    --html-templates-dir "$MAIN_BUILD_DIR/html_template" \
    --template-name "template" \
    --tqf-dir "$OUT_BASE/tqf" \
    --out-dir "$OUT_BASE/out" \
    > "$OUT_BASE/run.log" 2>&1

  echo "==> Output: $OUT_BASE/out"
}