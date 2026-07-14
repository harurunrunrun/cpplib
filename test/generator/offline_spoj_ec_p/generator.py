#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
from pathlib import Path

runpy.run_path(
    str(Path(__file__).resolve().parents[3] / "scripts" / "generate_spoj_graph_cases.py"),
    init_globals={"PROBLEM": "ec_p"},
    run_name="__main__",
)
