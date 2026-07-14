#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n4 0 0 4 0 4 3 0 3 -1 1 5 1\n4 0 0 4 0 4 3 0 3 -1 0 5 0\n4 0 0 4 0 4 3 0 3 1 1 1 1\n","2\n0.0000000000 1.0000000000\n4.0000000000 1.0000000000\n2\n0.0000000000 0.0000000000\n4.0000000000 0.0000000000\nERR\n")]
if __name__=="__main__":generate(CASES)
