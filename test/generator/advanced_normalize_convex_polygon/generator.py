#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "advanced_common"))
from generator_utils import generate

CASES = [("5\n6\n0 0\n0 2\n2 2\n2 0\n1 0\n0 0\n3\n2 0\n1 0\n0 0\n4\n3 4\n3 4\n3 4\n3 4\n0\n5\n0 0\n2 0\n1 1\n2 2\n0 2\n", "4 0 0 2000000 0 2000000 2000000 0 2000000\n2 0 0 2000000 0\n1 3000000 4000000\n0\nERR\n")]
CASES.append((
    """2
5
0 3
2 -3
-3 1
3 1
-2 -3
8
0 0
2 0
2 2
0 2
0 0
2 0
2 2
0 2
""",
    """ERR
ERR
""",
))

if __name__ == "__main__":
    generate(CASES)
