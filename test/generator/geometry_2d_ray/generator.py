#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("1\n0 1 2 3\n","0.0000000000 1.0000000000\n2.0000000000 3.0000000000\n")]
if __name__=="__main__":generate(CASES)
