#!/bin/bash
#
#
# E.g. ./tools/linux/run.sh --window-size=900x700
#

GIT_ROOT="$(git rev-parse --show-toplevel)"

"$GIT_ROOT/build/bin/tooter" --resources=resources $*


