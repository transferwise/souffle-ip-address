#!/usr/bin/env bash
#
# Usage: test.sh <datalog file>.
#
# It is assumed that a file containing expected output is next to that file with
# extension '.out'.

set -euo pipefail

TEST_DL=$1
EXP_OUT="$(dirname "$TEST_DL")/$(basename "$TEST_DL" .dl).out"

OUT=$(mktemp)
echo "$TEST_DL"
souffle -D- "$TEST_DL" > "$OUT"
diff "$OUT" "$EXP_OUT"
