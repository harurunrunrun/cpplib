#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n2 0 3 4 -2 0 0 1 0\n0 0 0 1 0\n0 1 1 1 1\n","2.0000000000\nINF\nERR\n")]
if __name__=="__main__":generate(CASES)
