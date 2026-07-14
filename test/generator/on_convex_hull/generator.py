#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4 3\n0 0\n2 0\n2 2\n0 2\n0 1\n1 1\n3 1\n", "1 1\n0 0\n0 0\n")]

if __name__ == "__main__":
    generate(CASES)
