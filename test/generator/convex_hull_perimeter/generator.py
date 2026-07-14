#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("3\n4\n0 0\n2 0\n2 1\n0 1\n3\n0 0\n1 0\n3 0\n1\n2 5\n", "6.0000000000\n6.0000000000\n0.0000000000\n")]

if __name__ == "__main__":
    generate(CASES)
