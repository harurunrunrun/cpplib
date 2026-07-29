#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

from oracle import first_fit_decreasing


def write_case(output: Path, index: int, capacity: int, items: list[int]) -> None:
    stem = f"case_{index:02d}"
    (output / f"{stem}.in").write_text(
        f"{len(items)} {capacity}\n{' '.join(map(str, items))}\n",
        encoding="utf-8",
    )
    (output / f"{stem}.out").write_text(
        f"{first_fit_decreasing(items, capacity)}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    engine = random.Random(20260729)
    cases = [(10, []), (10, [10]), (10, [6, 4, 6, 4, 5, 5])]
    for count, capacity in ((20, 50), (100, 1000), (500, 10_000)):
        cases.append(
            (capacity, [engine.randrange(0, capacity + 1) for _ in range(count)])
        )
    for index, (capacity, items) in enumerate(cases):
        write_case(output, index, capacity, items)


if __name__ == "__main__":
    main()
