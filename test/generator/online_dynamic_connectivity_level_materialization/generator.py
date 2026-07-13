#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    n = 262144
    (out_dir / "case_00.in").write_text(f"{n}\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text(
        f"1 0\n1 1 1\n1 2 {n - 2} 0 2 2\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
