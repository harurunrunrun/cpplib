#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import argparse
from pathlib import Path
parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
out = Path(args.out_dir)
out.mkdir(parents=True, exist_ok=True)
(out / "case_00.in").write_text("12\n", encoding="utf-8")
(out / "case_00.out").write_text("6000000\n", encoding="utf-8")
