#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(output_directory: Path, index: int, rewards: list[int]) -> None:
    stem = f"case_{index:02d}"
    iterations = max(1000, len(rewards) * 100)
    (output_directory / f"{stem}.in").write_text(
        f"{len(rewards)} {iterations}\n{' '.join(map(str, rewards))}\n", encoding="utf-8"
    )
    best = max(range(len(rewards)), key=rewards.__getitem__)
    (output_directory / f"{stem}.out").write_text(f"{best}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)
    random_engine = random.Random(161803398)

    cases = [[42], [-1, 9], [100, -50, 101, 99]]
    for size in [5, 10, 30, 100]:
        rewards = random_engine.sample(range(-100_000, 100_001), size)
        cases.append(rewards)
    for index, rewards in enumerate(cases):
        write_case(output_directory, index, rewards)


if __name__ == "__main__":
    main()
