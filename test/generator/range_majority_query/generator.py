#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def majority(values: list[int], left: int, right: int) -> int:
    counts = collections.Counter(values[left:right])
    value, count = counts.most_common(1)[0]
    return value if (right - left) // 2 < count else -1


def write_case(
    out_dir: Path,
    index: int,
    initial: list[int],
    operations: list[tuple[int, int, int]],
) -> None:
    values = initial.copy()
    output: list[str] = []
    for kind, first, second in operations:
        if kind == 0:
            values[first] = second
        else:
            output.append(str(majority(values, first, second)))
    lines = [f"{len(initial)} {len(operations)}", " ".join(map(str, initial))]
    lines.extend(f"{kind} {first} {second}" for kind, first, second in operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], list[tuple[int, int, int]]]] = [
        ([0], [(1, 0, 1), (0, 0, 7), (1, 0, 1)]),
        ([1, 2, 1, 1, 3, 1], [(1, 0, 6), (1, 1, 5), (0, 3, 2), (1, 0, 6)]),
        ([4] * 100, [(1, 0, 100), (0, 50, 3), (1, 1, 99)]),
    ]
    rng = random.Random(2026071302)
    for _ in range(20):
        size = rng.randrange(1, 150)
        initial = [rng.randrange(12) for _ in range(size)]
        operations: list[tuple[int, int, int]] = []
        for _ in range(500):
            if rng.randrange(3) == 0:
                operations.append((0, rng.randrange(size), rng.randrange(12)))
            else:
                left = rng.randrange(size)
                right = rng.randrange(left + 1, size + 1)
                operations.append((1, left, right))
        cases.append((initial, operations))

    for index, (initial, operations) in enumerate(cases):
        write_case(args.out_dir, index, initial, operations)


if __name__ == "__main__":
    main()
