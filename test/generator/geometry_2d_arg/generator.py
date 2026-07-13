#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("5\n1 0\n0 1\n-1 0\n0 -1\n1 1\n", "0.0000000000\n1.5707963268\n3.1415926536\n-1.5707963268\n0.7853981634\n")]

if __name__ == "__main__":
    generate(CASES)
