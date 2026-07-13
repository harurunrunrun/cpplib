#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from geometry_2d_circle_support import run

run(Path(__file__).resolve().parent.name)
