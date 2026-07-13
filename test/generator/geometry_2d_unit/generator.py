#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("4\n3 4\n-5 0\n0 0\n0.00000000005 0\n", "0.6000000000 0.8000000000\n-1.0000000000 0.0000000000\nERR\nERR\n")]

if __name__ == "__main__":
    generate(CASES)
