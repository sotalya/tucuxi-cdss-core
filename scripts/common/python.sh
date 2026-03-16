#!/usr/bin/env bash
set -euo pipefail

# shellcheck source=/dev/null
source "$REPO_ROOT/scripts/common/common.sh"

VENV_DIR="$REPO_ROOT/.venv"
VENV_PY="$VENV_DIR/bin/python"
VENV_PIP="$VENV_DIR/bin/pip"

ensure_venv() {
  require_cmd python3

  if ! python3 -c "import venv" >/dev/null 2>&1; then
    die "python3-venv is missing. Ubuntu/Debian: sudo apt-get install python3-venv"
  fi

  if [[ ! -x "$VENV_PY" ]]; then
    echo "==> Creating venv at $VENV_DIR"
    python3 -m venv "$VENV_DIR"
  fi

  echo "==> Ensuring pip is up to date in venv"
  "$VENV_PY" -m pip install --upgrade pip setuptools wheel >/dev/null
}