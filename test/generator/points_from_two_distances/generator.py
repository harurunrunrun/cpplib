#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("6\n0 0 3 4 0 3\n0 0 2 4 0 2\n0 0 1 4 0 1\n1 2 0 1 2 0\n1 2 3 1 2 3\n0 0 -1 1 0 2\n","2\n2.0000000000 -2.2360679775\n2.0000000000 2.2360679775\n1\n2.0000000000 0.0000000000\n0\n1\n1.0000000000 2.0000000000\nERR\nERR\n")]
if __name__=="__main__":generate(CASES)
