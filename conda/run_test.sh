#!/usr/bin/env bash
set -euo pipefail

cd test/e2e/

test -d cases
test -f conftest.py

export OMP_WAIT_POLICY=PASSIVE
pytest -k gpu_grappa --echo-log-on-failure
