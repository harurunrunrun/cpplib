#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_brute(first: list[int], second: list[int]) -> int:
    return sum(
        first[left] >= first[right] and second[left] <= second[right]
        for left in range(len(first))
        for right in range(len(first))
    )


def write_case(
    out_dir: Path,
    case_index: int,
    first: list[int],
    second: list[int],
) -> None:
    name = f"case_{case_index:03d}"
    lines = [
        str(len(first)),
        " ".join(map(str, first)),
        " ".join(map(str, second)),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        f"{solve_brute(first, second)}\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([], []),
        ([0], [0]),
        ([50, 100, 150], [1, 3, 2]),
        ([123456789, 123456, 123], [987, 987654, 987654321]),
        ([7, 7, 7, 7], [4, 4, 4, 4]),
        ([3, 1, 4, 1, 5, 9, 2, 6, 5, 3], [2, 7, 1, 8, 2, 8, 1, 8, 2, 8]),
    ]
    source = random.Random(231231)
    for _ in range(140):
        n = source.randint(0, 60)
        first = [source.randint(-8, 8) for _ in range(n)]
        second = [source.randint(-8, 8) for _ in range(n)]
        cases.append((first, second))

    for case_index, case in enumerate(cases):
        write_case(out_dir, case_index, *case)


if __name__ == "__main__":
    main()
