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

    random_source = random.Random(2026071301)
    cases: list[tuple[int, list[int]]] = [
        (0, [-100, -20, 30, 90]),
        (1000, [-1000, 0, 500, 999, 1500]),
        (-777, [-1000, -900, -800, -700, 100]),
    ]
    for _ in range(25):
        target = random_source.randint(-10000, 10000)
        population = [random_source.randint(-20000, 20000) for _ in range(12)]
        cases.append((target, population))
    text = [str(len(cases))]
    for target, population in cases:
        text.append(
            f"{target} {len(population)} " + " ".join(map(str, population))
        )
    (args.out_dir / "cases.in").write_text("\n".join(text) + "\n", encoding="utf-8")
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
