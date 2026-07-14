#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [(
    "6\n"
    "0 0 2 10 0 2\n"
    "0 0 2 4 0 2\n"
    "0 0 3 2 0 1\n"
    "0 0 3 0 0 1\n"
    "0 0 3 0 0 3\n"
    "0 0 -1 4 0 2\n",
    "4 0.000000000 -2.000000000 0.000000000 2.000000000 "
    "0.800000000 -1.833030278 0.800000000 1.833030278\n"
    "3 0.000000000 -2.000000000 0.000000000 2.000000000 "
    "2.000000000 0.000000000\n"
    "1 3.000000000 0.000000000\n"
    "0\nDOMAIN\nINVALID\n",
)]

if __name__ == "__main__":
    generate(CASES)
