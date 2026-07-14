#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate
CASES = [("5\n-1 0 1 0 1\n-4 0 4 0 5\n0 0 3 0 1\n2 3 2 3 4\n2 3 2 3 0\n", "1\n0.0000000000 0.0000000000 1.0000000000\n2\n0.0000000000 -3.0000000000 5.0000000000\n0.0000000000 3.0000000000 5.0000000000\n0\nERR\n1\n2.0000000000 3.0000000000 0.0000000000\n")]
if __name__ == "__main__": generate(CASES)
