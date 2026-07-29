#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import runpy

runpy.run_path(
    str(Path(__file__).resolve().parents[3] / "scripts" / "standalone_checker.py"),
    run_name="__main__",
)
