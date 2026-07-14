#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
from pathlib import Path

runpy.run_path(
    str(
        Path(__file__).resolve().parents[1]
        / "offline_spoj_findsr"
        / "generator.py"
    ),
    run_name="__main__",
)
