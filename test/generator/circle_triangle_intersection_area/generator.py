#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate
CASES = [("3\n0 0 10 0 0 1 0 0 1\n0 0 1 -10 -10 10 -10 0 10\n0 0 1 2 0 3 0 2 1\n", "0.5000000000\n3.1415926536\n0.0000000000\n")]
if __name__ == "__main__": generate(CASES)
