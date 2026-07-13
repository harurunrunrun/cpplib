#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "advanced_common"))
from generator_utils import generate

CASES = [("2\n", "1 4 1 3000000 4000000 2 1\n1 4 1 3000000 4000000 2 1\n")]

if __name__ == "__main__":
    generate(CASES)
