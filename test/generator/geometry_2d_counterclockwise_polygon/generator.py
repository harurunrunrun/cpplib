#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("2\n4 0 0 0 1 1 1 1 0\n3 0 0 1 0 0 1\n","0.0000000000 0.0000000000\n1.0000000000 0.0000000000\n1.0000000000 1.0000000000\n0.0000000000 1.0000000000\n0.0000000000 0.0000000000\n1.0000000000 0.0000000000\n0.0000000000 1.0000000000\n")]
if __name__=="__main__":generate(CASES)
