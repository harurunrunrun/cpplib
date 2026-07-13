#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4\n7\n0 0\n2 0\n2 2\n0 2\n1 1\n0 0\n1 0\n4\n3 0\n1 0\n0 0\n2 0\n1\n5 -2\n0\n", "4\n0.0000000000 0.0000000000\n2.0000000000 0.0000000000\n2.0000000000 2.0000000000\n0.0000000000 2.0000000000\n2\n0.0000000000 0.0000000000\n3.0000000000 0.0000000000\n1\n5.0000000000 -2.0000000000\n0\n")]

if __name__ == "__main__":
    generate(CASES)
