#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("6\n4\n0 0\n2 0\n2 2\n0 2\n1 1\n4\n0 0\n0 2\n2 2\n2 0\n2 1\n4\n0 0\n2 0\n2 2\n0 2\n3 1\n2\n0 0\n2 0\n1 0\n1\n3 4\n3 4\n0\n0 0\n", "2\n1\n0\n1\n1\n0\n")]

if __name__ == "__main__":
    generate(CASES)
