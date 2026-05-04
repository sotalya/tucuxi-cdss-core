#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

echo "==> Building main project ($CONFIG)"
cmake -S "$REPO_ROOT" -B "$MAIN_BUILD_DIR" \
  -DCMAKE_BUILD_TYPE="$(cmake_build_type)" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build "$MAIN_BUILD_DIR" -j"$(portable_nproc)"