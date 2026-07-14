#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"geometry_2d_common"));from generator_utils import generate
CASES=[("2\n0 0 1 0 0 0 0 1\n0 0 1 0 0 1 1 1\n","0.7071067812\n-0.7071067812\n0.0000000000\n0.7071067812\n0.7071067812\n0.0000000000\nERR\n")]
if __name__=="__main__":generate(CASES)
