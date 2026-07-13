#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    random_source = random.Random(2026071303)
    cases: list[list[float]] = [[0.0], [1.5, -2.0], [0.0] * 8]
    for dimension in range(1, 9):
        cases.append([
            random_source.uniform(-100.0, 100.0) for _ in range(dimension)
        ])
    text = [str(len(cases))]
    for target in cases:
        text.append(f"{len(target)} " + " ".join(f"{value:.12f}" for value in target))
    (args.out_dir / "cases.in").write_text("\n".join(text) + "\n", encoding="utf-8")
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
