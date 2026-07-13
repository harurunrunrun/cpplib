#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("5\n0 0 2 2 0 2 2 0\n0 0 2 0 2 0 2 2\n0 0 1 0 2 0 3 0\n0 0 3 0 1 0 2 0\n1 0 1 0 0 0 2 0\n", "1.0000000000 1.0000000000\n2.0000000000 0.0000000000\nERR\nERR\n1.0000000000 0.0000000000\n")]

if __name__ == "__main__":
    generate(CASES)
