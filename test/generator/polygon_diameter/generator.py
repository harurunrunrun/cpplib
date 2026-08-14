#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import argparse
from pathlib import Path
parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
out = Path(args.out_dir)
out.mkdir(parents=True, exist_ok=True)
data = "6\n0 0\n3 0\n3 1\n1 1\n1 4\n0 4\n"
(out / "case_00.in").write_text(data, encoding="utf-8")
(out / "case_00.out").write_text("5000000\n", encoding="utf-8")
