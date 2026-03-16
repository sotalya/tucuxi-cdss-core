#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/common/common.sh"
source "$REPO_ROOT/scripts/common/cmake.sh"

cmd_build_gtest() {
  echo "==> Building gtests ($CONFIG)"
  cmake -S "$REPO_ROOT/test/gtest" -B "$GTEST_BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$(cmake_build_type)"
  cmake --build "$GTEST_BUILD_DIR" -j"$(nproc)"
}

cmd_run_gtest() {
  echo "==> Running gtests ($CONFIG)"
  if [[ ! -x "$GTEST_BUILD_DIR/tucu-cdss-tests" ]]; then
    echo "==> gtest binary not found, building first..."
    cmd_build_gtest
  fi
  "$GTEST_BUILD_DIR/tucu-cdss-tests"
}