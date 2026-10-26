#!/bin/sh
# Build the Magnetoids demo from your own copy of it:
#     ./build.sh "Magnetoids - Demo (Oregan).zip"   (a RISC OS zip of !MGNTOIDS; or a directory holding it)
# then: cd recomp && ./magnetoids
# The steps are recipe.json's "build", run by SuperSpider's tools/weave.py.
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
SUPERSPIDER="${SUPERSPIDER:-$HOME/stuff/superspider}"
[ $# -gt 0 ] || { echo "usage: $0 MAGNETOIDS.zip | DIRECTORY" >&2; exit 2; }
(cd "$SUPERSPIDER" && dune build)
WEAVE_RECIPES="$HERE" exec python3 -I "$SUPERSPIDER/tools/weave.py" build "$@"
