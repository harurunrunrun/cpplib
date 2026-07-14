#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate
CASES = [("4\n4\n0 0\n2 0\n2 2\n0 2\n-1 1 3 1\n4\n0 0\n2 0\n2 2\n0 2\n-1 3 3 3\n4\n0 0\n2 0\n2 2\n0 2\n-1 0 3 0\n1\n0 0\n1 1 1 1\n", "1 1\n0 0\n1 1\nERR\n")]
if __name__ == "__main__": generate(CASES)
