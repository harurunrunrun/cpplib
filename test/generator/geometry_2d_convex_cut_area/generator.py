#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

SQUARE_CCW = "4\n0 0\n2 0\n2 2\n0 2\n"
SQUARE_CW = "4\n0 0\n0 2\n2 2\n2 0\n"
CASES = [(
    "7\n"
    + SQUARE_CCW + "1 -1 1 3\n"
    + SQUARE_CCW + "1 3 1 -1\n"
    + SQUARE_CCW + "-1 -1 3 -1\n"
    + SQUARE_CCW + "-1 3 3 3\n"
    + SQUARE_CCW + "0 0 2 2\n"
    + SQUARE_CW + "1 -1 1 3\n"
    + SQUARE_CCW + "1 1 1 1\n",
    "2.000000000\n2.000000000\n4.000000000\n0.000000000\n"
    "2.000000000\n2.000000000\nERR\n",
)]

if __name__ == "__main__":
    generate(CASES)
