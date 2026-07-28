#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import argparse
from pathlib import Path
parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True, type=Path)
args = parser.parse_args()
args.out_dir.mkdir(parents=True, exist_ok=True)
(args.out_dir / "case_00.in").write_text("4\n", encoding="utf-8")
(args.out_dir / "case_00.out").write_text("OK\n" * 4, encoding="utf-8")
