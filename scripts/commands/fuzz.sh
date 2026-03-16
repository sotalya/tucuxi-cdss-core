#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/common/common.sh"
source "$REPO_ROOT/scripts/common/cmake.sh"

cmd_fuzz() {
  echo "==> Running fuzzing ($CONFIG)"
  ensure_cli_built

  RUN_ID="$(date +%Y%m%d-%H%M%S)"
  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    OUT_DIR="$BUILD_ROOT/fuzzing/$RUN_ID"
  else
    OUT_DIR="$BUILD_ROOT/$CONFIG/fuzzing/$RUN_ID"
  fi

  mkdir -p "$OUT_DIR"

  pushd "$REPO_ROOT/test/fuzzing" >/dev/null

  "$PYTHON_BIN" main.py \
    --cdss_cli_path "$CDSS_CLI" \
    --drug_files_dir "$REPO_ROOT/../tucuxi-drugs/drugfiles" \
    --language_files_dir "$REPO_ROOT/language" \
    --template_dir "$MAIN_BUILD_DIR/html_template" \
    --template_name "template" \
    --out_dir_path "$OUT_DIR/output" \
    --logfile_name "000_LOG.txt"

  popd >/dev/null

  echo "==> Output: $OUT_DIR/output"
}