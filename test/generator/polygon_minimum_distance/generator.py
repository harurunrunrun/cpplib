#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
out = Path(args.out_dir)
out.mkdir(parents=True, exist_ok=True)

concave = "6\n0 0\n3 0\n3 1\n1 1\n1 3\n0 3\n"
(out / "case_00.in").write_text(
    "6 4\n0 0\n3 0\n3 1\n1 1\n1 3\n0 3\n5 0\n6 0\n6 1\n5 1\n",
    encoding="utf-8",
)
(out / "case_00.out").write_text("2000000\n", encoding="utf-8")
(out / "case_01.in").write_text(
    "6 4\n0 0\n3 0\n3 1\n1 1\n1 3\n0 3\n"
    "0.2 0.2\n0.8 0.2\n0.8 0.8\n0.2 0.8\n",
    encoding="utf-8",
)
(out / "case_01.out").write_text("0\n", encoding="utf-8")
