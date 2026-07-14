#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("5\n0 0 4 0 2 3\n0 0 4 0 -1 2\n0 0 4 0 7 -1\n1 1 1 1 9 9\n0 0 0.000001 0 0.000001 1\n","2.0000000000 0.0000000000\n0.0000000000 0.0000000000\n4.0000000000 0.0000000000\n1.0000000000 1.0000000000\n0.0000010000 0.0000000000\n")]
if __name__=="__main__":generate(CASES)
