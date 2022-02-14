#!/bin/bash
#
#
#

GIT_ROOT="$(git rev-parse --show-toplevel)"

"$GIT_ROOT/build/bin/tooter" --resources=resources $*


