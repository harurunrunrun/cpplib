#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4\n2 0\n2 1.570796326794896619\n3 3.141592653589793238\n-2 0\n", "2.0000000000 0.0000000000\n0.0000000000 2.0000000000\n-3.0000000000 0.0000000000\n-2.0000000000 0.0000000000\n")]

if __name__ == "__main__":
    generate(CASES)
