#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n0 0 1 0 -2 3\n0 0 1 0 2 3\n1 1 1 1 2 2\n","3.6055512755\n3.0000000000\nERR\n")]
if __name__=="__main__":generate(CASES)
