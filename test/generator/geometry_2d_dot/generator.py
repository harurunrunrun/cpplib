#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4\n1 2 3 4\n1 0 0 7\n-2 5 4 -1\n0.5 0.25 2 4\n", "11.0000000000\n0.0000000000\n-13.0000000000\n2.0000000000\n")]

if __name__ == "__main__":
    generate(CASES)
