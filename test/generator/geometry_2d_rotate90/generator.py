#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4\n1 2\n-3 4\n0 0\n2 -5\n", "-2.0000000000 1.0000000000\n-4.0000000000 -3.0000000000\n0.0000000000 0.0000000000\n5.0000000000 2.0000000000\n")]

if __name__ == "__main__":
    generate(CASES)
