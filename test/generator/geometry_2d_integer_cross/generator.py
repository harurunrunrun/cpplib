#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n1 0 0 1\n0 1 1 0\n-9223372036854775808 -9223372036854775808 9223372036854775807 -9223372036854775808\n","1\n-1\n170141183460469231722463931679029329920\n")]
if __name__=="__main__":generate(CASES)
