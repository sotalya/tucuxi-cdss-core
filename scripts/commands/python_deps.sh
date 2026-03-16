#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/common/common.sh"
source "$REPO_ROOT/scripts/common/python.sh"

cmd_py_env() {
  ensure_venv
  echo "source \"$VENV_DIR/bin/activate\""
}

cmd_py_deps() {
  echo "==> Installing python deps into .venv"
  ensure_venv

  "$VENV_PIP" install -r "$REPO_ROOT/test/fuzzing/requirements.txt"
  "$VENV_PIP" install -r "$REPO_ROOT/test/covariate_boundaries_driver/requirements.txt"
  "$VENV_PIP" install -r "$REPO_ROOT/test/system/imatinib/requirements.txt"

  echo "==> Done."
  echo "==> To activate in your shell:"
  echo "   source \"$VENV_DIR/bin/activate\""
}