#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import argparse, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from sparse_persistent_segtree_common import generate_persistent
parser = argparse.ArgumentParser(); parser.add_argument("--out-dir", required=True); args = parser.parse_args()
generate_persistent(Path(args.out_dir), True)
