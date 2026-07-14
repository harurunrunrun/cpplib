#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate
CASES = [("2\n4\n0 0\n1 0\n1 1\n0 1\n4\n3 0\n4 0\n4 1\n3 1\n1\n0 0\n1\n3 4\n", "4.1231056256\n5.0000000000\n")]
if __name__ == "__main__": generate(CASES)
