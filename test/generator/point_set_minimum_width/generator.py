#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
out = Path(args.out_dir)
out.mkdir(parents=True, exist_ok=True)
cases = [
    ("6\n0 0\n4 0\n4 1\n2 1\n2 3\n0 3\n", "3000000\n"),
    ("2\n0 0\n7 3\n", "0\n"),
]
for index, (data, answer) in enumerate(cases):
    (out / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
    (out / f"case_{index:02d}.out").write_text(answer, encoding="utf-8")
