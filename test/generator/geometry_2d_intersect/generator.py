#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("8\nSS 0 0 2 2 0 2 2 0\nSS 0 0 1 0 2 0 3 0\nSS 0 0 2 0 2 0 2 2\nSS 0 0 3 0 1 0 2 0\nSS 1 1 1 1 1 1 1 1\nLP 0 0 2 0 3 0\nLP 0 0 2 0 3 1\nLP 1 1 1 1 1 1\n", "1\n0\n1\n1\n1\n1\n0\nERR\n")]

if __name__ == "__main__":
    generate(CASES)
