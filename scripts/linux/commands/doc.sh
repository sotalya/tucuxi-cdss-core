#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/python.sh"

cmd_doc() {
  local format="${1:-html}"

  ensure_venv

  if ! "$VENV_PY" -c "import sphinx" >/dev/null 2>&1; then
    die "Sphinx is not installed in .venv. Run: ./scripts/linux/run py-deps"
  fi

  require_cmd make
  mkdir -p "$DOC_BUILD_DIR"

  case "$format" in
    html)
      echo "==> Building documentation (html)"
      make -C "$REPO_ROOT/docs" \
        SPHINXBUILD="$VENV_PY -m sphinx" \
        BUILDDIR="$DOC_BUILD_DIR" \
        html
      echo "==> HTML docs: $DOC_BUILD_DIR/html/index.html"
      ;;
    pdf)
      echo "==> Building documentation (pdf)"
      make -C "$REPO_ROOT/docs" \
        SPHINXBUILD="$VENV_PY -m sphinx" \
        BUILDDIR="$DOC_BUILD_DIR" \
        latexpdf

      local pdf
      pdf="$(find "$DOC_BUILD_DIR/latex" -maxdepth 1 -type f -name '*.pdf' | head -n1 || true)"
      [[ -n "$pdf" ]] || die "PDF build finished but no PDF was found in $DOC_BUILD_DIR/latex"

      echo "==> PDF docs: $pdf"
      ;;
    -h|--help|help)
      cat <<'EOF'
Usage:
  ./scripts/linux/run doc [html|pdf]

Default:
  html
EOF
      ;;
    *)
      die "Unknown doc format: $format (expected: html or pdf)"
      ;;
  esac
}