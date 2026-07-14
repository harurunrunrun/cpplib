#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("2\n4 0 0 4 0 4 3 0 3\n3 0 0 2 0 0 2\n","14 6\n6 0\n")]
if __name__=="__main__":generate(CASES)
