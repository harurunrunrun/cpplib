#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"))
from generator_utils import generate
CASES=[("4\n1 1\n1 1.00000000005\n1 1.0000000002\n-3 -3\n","1\n1\n0\n1\n")]
if __name__=="__main__":generate(CASES)
