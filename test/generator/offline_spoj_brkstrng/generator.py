#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from spoj_dp_generators import main
main("brkstrng")
