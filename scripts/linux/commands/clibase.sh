#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_clibase() {
  echo "==> Running clibase system tests ($CONFIG)"
  ensure_cli_built

  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    OUT_DIR="$BUILD_ROOT/system/clibase/output"
  else
    OUT_DIR="$BUILD_ROOT/$CONFIG/system/clibase/output"
  fi
  mkdir -p "$OUT_DIR"

  "$PYTHON_BIN" "$REPO_ROOT/test/system/clibase/clitest.py" \
    --cdss-cli "$CDSS_CLI" \
    --drug-files-dir "$REPO_ROOT/test/drugfiles" \
    --language-files-dir "$REPO_ROOT/language" \
    --template-dir "$MAIN_BUILD_DIR/html_template" \
    --template-name "template" \
    --out-dir "$OUT_DIR"

  echo "==> Output: $OUT_DIR"
}
