#!/bin/sh
# Code/data analysis of the Magnetoids demo with SuperSpider
# (~/stuff/superspider): loadcode.json and proggy.json, which recomp/'s
# "make regen" translates to C with superspider gen.
#   LOADCODE  the absolute !Run runs, at &8000: the logo, then it loads
#             PROGGY straight after itself (&825C) and jumps there
#   PROGGY    the game, at &825C (position-independent: it finds its
#             workspace and relocates its shape table from its own PC).
#             --global-r12: its workspace pointer (&17704) stays in r12
#             throughout; routines save their return address in it and
#             return through it after re-establishing r12 PC-relative
# The .roots files add entry points to the analyses (recomp/: "make roots"
# merges the gap entries a --trace-gaps run logged into proggy.roots).
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
SUPERSPIDER="${SUPERSPIDER:-$HOME/stuff/superspider}"
SS="$SUPERSPIDER/_build/default/bin/main.exe"
G="$HERE/../game/!MGNTOIDS"
LOADCODE="$(ls "$G"/LOADCODE,* | head -1)"   # whatever their ,load-exec suffixes
PROGGY="$(ls "$G"/PROGGY,* | head -1)"
END=$(printf '%x' $((0x825C + $(wc -c < "$PROGGY"))))
(cd "$SUPERSPIDER" && dune build)
"$SS" analyse "$LOADCODE" 8000 --full --extern 825c-$END --roots "$HERE/loadcode.roots" -o "$HERE/loadcode.json"
"$SS" analyse "$PROGGY" 825c --full --global-r12 --roots "$HERE/proggy.roots" -o "$HERE/proggy.json"
