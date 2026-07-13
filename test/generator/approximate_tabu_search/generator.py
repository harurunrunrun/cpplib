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
    rng = random.Random(2026071316)
    cases: list[tuple[int, int, list[int]]] = []
    for size in [3, 4, 5, 10, 31, 100]:
        for _ in range(5):
            score = list(range(size))
            rng.shuffle(score)
            cases.append((size, rng.randrange(size), score))
    (args.out_dir / "cases.in").write_text(
        f"{len(cases)}\n" + "".join(
            f"{size} {initial}\n{' '.join(map(str, score))}\n"
            for size, initial, score in cases
        ),
        encoding="utf-8",
    )
    (args.out_dir / "cases.out").write_text(
        "0\n" * len(cases), encoding="utf-8"
    )


if __name__ == "__main__":
    main()
