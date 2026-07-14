#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [(
    "7\n"
    "0 0 3 0 1 2 5 2\n"
    "0 0 3 0 5 1 -1 1\n"
    "0 0 3 0 2 -4 2 7\n"
    "0 0 1 1 0 0 2 -2\n"
    "0 0 3 0 0 0 2 1\n"
    "1 1 1 1 0 0 2 0\n"
    "0 0 2 0 -3 4 -3 4\n",
    "2\n2\n1\n1\n0\nERR\nERR\n",
)]

if __name__ == "__main__":
    generate(CASES)
