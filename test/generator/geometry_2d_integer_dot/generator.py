#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("3\n1 2 3 4\n-9223372036854775808 -9223372036854775808 -9223372036854775808 -9223372036854775808\n9223372036854775807 9223372036854775807 9223372036854775807 9223372036854775807\n","11\n170141183460469231731687303715884105728\n170141183460469231694793815568465002498\n")]
if __name__=="__main__":generate(CASES)
