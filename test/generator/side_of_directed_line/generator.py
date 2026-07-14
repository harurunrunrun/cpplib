#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("5\n0 0 2 0 1 1\n0 0 2 0 1 -1\n0 0 2 0 3 0\n2 0 0 0 1 1\n1 1 1 1 2 2\n","1\n-1\n0\n-1\nERR\n")]
if __name__=="__main__":generate(CASES)
