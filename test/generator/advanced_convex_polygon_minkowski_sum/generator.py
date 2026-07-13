#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "advanced_common"))
from generator_utils import generate

CASES = [("5\n4\n0 0\n1 0\n1 1\n0 1\n4\n0 0\n1 0\n1 1\n0 1\n3\n0 0\n2 0\n0 2\n1\n1 -1\n2\n0 0\n2 0\n2\n0 0\n0 3\n0\n1\n2 5\n5\n0 0\n2 0\n1 1\n2 2\n0 2\n1\n0 0\n", "4 0 0 2000000 0 2000000 2000000 0 2000000\n3 1000000 -1000000 3000000 -1000000 1000000 1000000\n4 0 0 2000000 0 2000000 3000000 0 3000000\n0\nERR\n")]

if __name__ == "__main__":
    generate(CASES)
