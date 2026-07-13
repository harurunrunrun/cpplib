#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "advanced_common"))
from generator_utils import generate

CASES = [("5\n0\n1\n0 0 2 0\n4\n0 0 2 0\n2 0 2 2\n2 2 0 2\n0 2 0 0\n2\n1 1 1 0\n0 0 0 1\n1\n1 1 1 1\n", "U\nU\nB 4 0 0 2000000 0 2000000 2000000 0 2000000\nE\nERR\n")]

if __name__ == "__main__":
    generate(CASES)
