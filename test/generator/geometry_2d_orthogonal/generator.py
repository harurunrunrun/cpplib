#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("7\nP 1 0 0 2\nP 1 1 1 -1\nP 1 2 2 1\nP 0 0 3 4\nL 0 0 2 0 1 1 1 4\nL 0 0 2 2 1 0 3 2\nL 0 0 0 0 1 1 2 1\n", "1\n1\n0\n1\n1\n0\nERR\n")]

if __name__ == "__main__":
    generate(CASES)
