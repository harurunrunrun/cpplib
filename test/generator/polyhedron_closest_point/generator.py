#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(root / "scripts"))
from polyhedron3_test_cases import write_polyhedron3_case

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
write_polyhedron3_case(args.out_dir, "1000000 1750000 500000\n", "1.5 1.75 0.5\n")
