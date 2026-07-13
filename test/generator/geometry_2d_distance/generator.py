#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common"))
from generator_utils import generate

CASES = [("13\nPP 0 0 3 4\nLP 0 0 2 0 2 3\nSP 0 0 2 0 1 3\nSP 0 0 2 0 -1 0\nSP 0 0 2 0 4 0\nSP 1 1 1 1 4 5\nSS 0 0 2 2 0 2 2 0\nSS 0 0 2 0 0 3 2 3\nLL 0 0 2 0 0 2 2 2\nLL 0 0 2 0 1 -1 1 1\nLS 0 0 2 0 1 2 3 2\nLS 0 0 2 0 1 -1 1 1\nLP 1 1 1 1 2 2\n", "5.0000000000\n3.0000000000\n3.0000000000\n1.0000000000\n2.0000000000\n5.0000000000\n0.0000000000\n3.0000000000\n2.0000000000\n0.0000000000\n2.0000000000\n0.0000000000\nERR\n")]

if __name__ == "__main__":
    generate(CASES)
