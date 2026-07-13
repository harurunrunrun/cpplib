#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071304)
    rows, inner, columns = 31, 27, 29
    first = [[rng.randrange(-50, 51) for _ in range(inner)] for _ in range(rows)]
    second = [[rng.randrange(-50, 51) for _ in range(columns)] for _ in range(inner)]
    lines = [f"{rows} {inner} {columns}"]
    lines.extend(" ".join(map(str, row)) for row in first)
    lines.extend(" ".join(map(str, row)) for row in second)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
