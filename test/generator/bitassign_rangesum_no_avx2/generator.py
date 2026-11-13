#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
from pathlib import Path

runpy.run_path(
    str(Path(__file__).resolve().parents[3] / "scripts" / "standalone_empty_generator.py"),
    run_name="__main__",
)
