#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n4 0 0 4 0 4 2 0 2 -1 1 5 1\n8 0 0 6 0 6 6 4 6 4 2 2 2 2 6 0 6 -1 4 7 4\n8 0 0 6 0 6 6 4 6 4 2 2 2 2 6 0 6 2 2 4 4\n","1 1\n4.0000000000\n4.0000000000\n2 1\n8.0000000000\n20.0000000000\n2 1\n12.0000000000\n16.0000000000\n")]
if __name__=="__main__":generate(CASES)
