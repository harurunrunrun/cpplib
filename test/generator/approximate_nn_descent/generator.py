#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
from pathlib import Path

runpy.run_path(
    str(Path(__file__).resolve().parents[1] / "ann_exact_cases.py"),
    run_name="__main__",
)
