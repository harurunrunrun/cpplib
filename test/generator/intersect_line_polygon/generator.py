#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
out = Path(args.out_dir)
out.mkdir(parents=True, exist_ok=True)
polygon = "0 0\n3 0\n3 1\n1 1\n1 3\n0 3\n"
cases = [
    ("6 -1 2 4 2\n" + polygon, "1\n"),
    ("6 -1 4 4 4\n" + polygon, "0\n"),
]
for index, (data, answer) in enumerate(cases):
    (out / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
    (out / f"case_{index:02d}.out").write_text(answer, encoding="utf-8")
