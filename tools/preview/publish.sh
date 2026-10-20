#!/usr/bin/env bash
# Regenerate the pattern preview and copy it into a checkout of the GitHub
# Pages site, where it is served at https://costyn.github.io/uzumaru/preview/
#
#   tools/preview/publish.sh ../costyn.github.io
#
# Then commit and push the Pages repo. Needs the renderer (tools/preview/build.sh).
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
PAGES="${1:?usage: publish.sh <path to costyn.github.io checkout>}"
OUT="$PAGES/uzumaru/preview/index.html"
mkdir -p "$(dirname "$OUT")"
"$HERE/build.sh" >/dev/null
rm -rf "$(dirname "$OUT")/clips"
PREVIEW_SPLIT=1 python3 "$HERE/make_preview.py" html "$OUT"
echo "wrote $OUT ($(du -h "$OUT" | cut -f1)) and $(ls "$(dirname "$OUT")/clips" | wc -l) clips ($(du -sh "$(dirname "$OUT")/clips" | cut -f1))"
