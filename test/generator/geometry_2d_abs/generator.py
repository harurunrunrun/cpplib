#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4\n3 4\n0 0\n-5 0\n1 1\n", "5.0000000000\n0.0000000000\n5.0000000000\n1.4142135624\n")]

if __name__ == "__main__":
    generate(CASES)
