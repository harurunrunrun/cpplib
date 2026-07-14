#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
from pathlib import Path

runpy.run_path(
    str(
        Path(__file__).resolve().parents[1]
        / "offline_atcoder_abc340_d"
        / "generator.py"
    ),
    run_name="__main__",
)
