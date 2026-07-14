#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n0 0 4 0 0 3\n0 0 0 3 4 0\n0 0 1 1 2 2\n","6.0000000000\n-6.0000000000\n0.0000000000\n")]
if __name__=="__main__":generate(CASES)
