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

    random_source = random.Random(2026071309)
    cases: list[tuple[int, list[list[int]]]] = []
    for target in (0, 1000, -777):
        islands = [
            [random_source.randint(-2000, 2000) for _ in range(8)]
            for _ in range(3)
        ]
        cases.append((target, islands))
    for _ in range(20):
        target = random_source.randint(-10000, 10000)
        islands = [
            [random_source.randint(-20000, 20000) for _ in range(8)]
            for _ in range(3)
        ]
        cases.append((target, islands))

    text = [str(len(cases))]
    for target, islands in cases:
        text.append(str(target))
        text.extend(" ".join(map(str, island)) for island in islands)
    (args.out_dir / "cases.in").write_text("\n".join(text) + "\n", encoding="utf-8")
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
