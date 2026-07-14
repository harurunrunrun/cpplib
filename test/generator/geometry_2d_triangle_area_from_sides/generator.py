#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("4\n3 4 5\n1 1 2\n1 2 4\n-1 2 2\n","6.0000000000\n0.0000000000\nERR\nERR\n")]
if __name__=="__main__":generate(CASES)
