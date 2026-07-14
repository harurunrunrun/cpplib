#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate
CASES = [("3\n4\n0 0\n4 0\n4 2\n0 2\n1\n3 4\n0\n", "1 1 8.0000000000 4.0000000000 2.0000000000\n1 1 0.0000000000 0.0000000000 0.0000000000\n0 1 0.0000000000 0.0000000000 0.0000000000\n")]
if __name__ == "__main__": generate(CASES)
