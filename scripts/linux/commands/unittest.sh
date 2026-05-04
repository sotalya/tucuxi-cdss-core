#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_build_unittest() {
  echo "==> Building unittests ($CONFIG)"
  cmake -S "$REPO_ROOT/test/gtest" -B "$GTEST_BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$(cmake_build_type)"
  cmake --build "$GTEST_BUILD_DIR" -j"$(portable_nproc)"
}

cmd_run_unittest() {
  echo "==> Running unittestss ($CONFIG)"
  if [[ ! -x "$GTEST_BUILD_DIR/tucu-cdss-tests" ]]; then
    echo "==> unittest binary not found, building first..."
    cmd_build_unittest
  fi
  "$GTEST_BUILD_DIR/tucu-cdss-tests"
}