#!/usr/bin/env python3
import runpy
from pathlib import Path
runpy.run_path(str(Path(__file__).parents[1] / "plus_data_structures" / "checker.py"), run_name="__main__")
