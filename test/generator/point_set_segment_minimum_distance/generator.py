#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n2 2 3 8 0 0 0 4 0\n0 0 0 4 0\n1 4 5 1 1 1 1\n","3.0000000000\nINF\n5.0000000000\n")]
if __name__=="__main__":generate(CASES)
