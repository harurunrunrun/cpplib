#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(root / "scripts"))
from polyhedron3_test_cases import polyhedron3_mesh_text

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
args = parser.parse_args()
out = Path(args.out_dir)
out.mkdir(parents=True, exist_ok=True)
tetrahedron = (
    "4 4\n0.2 0.2 0.2\n0.8 0.2 0.2\n0.2 0.8 0.2\n0.2 0.2 0.8\n"
    "0 2 1\n0 1 3\n0 3 2\n1 2 3\n"
)
(out / "case_00.in").write_text(
    polyhedron3_mesh_text() + tetrahedron, encoding="utf-8"
)
(out / "case_00.out").write_text("1\n", encoding="utf-8")
