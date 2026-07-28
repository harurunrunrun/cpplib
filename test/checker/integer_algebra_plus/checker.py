#!/usr/bin/env python3

from pathlib import Path
import subprocess
import sys


def main() -> int:
    root = Path(__file__).resolve().parents[3]
    checker = root / "scripts" / "standalone_checker.py"
    return subprocess.call([sys.executable, str(checker), *sys.argv[1:]])


if __name__ == "__main__":
    raise SystemExit(main())
