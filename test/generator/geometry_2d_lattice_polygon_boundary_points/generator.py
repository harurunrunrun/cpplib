#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("2\n4 0 0 4 0 4 3 0 3\n4 -9223372036854775808 0 9223372036854775807 0 9223372036854775807 1 -9223372036854775808 1\n","14\n36893488147419103232\n")]
if __name__=="__main__":generate(CASES)
