#!/usr/bin/env bash
set -euo pipefail
calculator=${1:?Pass the calculator executable}
"$calculator" '{"first":2,"second":3,"operation":"+"}'
psql -X -v ON_ERROR_STOP=1 -c 'TABLE calculations'
"$calculator" '{"first":3,"second":2,"operation":"+"}'
psql -X -v ON_ERROR_STOP=1 -c 'SELECT count(*) AS rows_after_cache_hit FROM calculations'
if "$calculator" '{"first":10,"second":0,"operation":"/"}'; then
    echo 'Expected division by zero to fail' >&2
    exit 1
fi
psql -X -v ON_ERROR_STOP=1 -c 'TABLE calculations'
test "$(psql -X -At -v ON_ERROR_STOP=1 -c 'SELECT count(*) FROM calculations')" = 2
