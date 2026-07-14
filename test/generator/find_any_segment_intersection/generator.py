#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_2d_segment_sweep_common"))
from generator_utils import generate_sweep_cases
if __name__ == "__main__":
    generate_sweep_cases("find")
