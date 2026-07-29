#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

from oracle import best_move


def write_case(output: Path, index: int, rewards: list[int]) -> None:
    stem = f"case_{index:02d}"
    iterations = max(200, len(rewards) * 50)
    (output / f"{stem}.in").write_text(
        f"{len(rewards)} {iterations}\n{' '.join(map(str, rewards))}\n",
        encoding="utf-8",
    )
    (output / f"{stem}.out").write_text(
        f"{best_move(rewards)}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    engine = random.Random(20260729)
    cases = [[5], [-10, 11], [4, 100, -2, 99]]
    for size in (5, 15, 50):
        cases.append(engine.sample(range(-1_000_000, 1_000_001), size))
    for index, rewards in enumerate(cases):
        write_case(output, index, rewards)


if __name__ == "__main__":
    main()
