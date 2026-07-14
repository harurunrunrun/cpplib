#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
from pathlib import Path

runpy.run_path(
    str(
        Path(__file__).resolve().parents[1]
        / "offline_atcoder_abc317_g"
        / "generator.py"
    ),
    run_name="__main__",
)
