#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[3]
raise SystemExit(subprocess.call([
    sys.executable,
    str(ROOT / "scripts" / "standalone_checker.py"),
    *sys.argv[1:],
]))
