#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n0 0 2 0\n1 -1 1 3\n2 2 2 2\n","1.0000000000 0.0000000000\n1.0000000000 2.0000000000\n1.0000000000 1.0000000000\n-3.0000000000 1.0000000000\nERR\n")]
if __name__=="__main__":generate(CASES)
