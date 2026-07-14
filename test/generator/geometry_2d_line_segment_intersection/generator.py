#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("4\n0 0 2 0 1 -1 1 1\n0 0 2 0 3 -1 3 1\n0 0 2 0 1 0 3 0\n0 0 2 0 1 0 1 0\n","1.0000000000 0.0000000000\n3.0000000000 0.0000000000\nERR\n1.0000000000 0.0000000000\n")]
if __name__=="__main__":generate(CASES)
