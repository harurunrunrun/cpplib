#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def brute(values: list[int], ranges: list[tuple[int, int]]) -> list[int]:
    answers = []
    for left, right in ranges:
        counts = collections.Counter(values[left:right])
        answers.append(sum(count // 2 for count in counts.values()))
    return answers


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    ranges: list[tuple[int, int]],
) -> None:
    lines = [f"{len(values)} {len(ranges)}", " ".join(map(str, values))]
    lines.extend(f"{left} {right}" for left, right in ranges)
    answers = brute(values, ranges)
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + ("\n" if answers else ""),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([], [(0, 0)]),
        ([5], [(0, 0), (0, 1)]),
        ([1, 1, 1, 1], [(0, 4), (1, 3), (2, 4)]),
        ([-10**12, 7, -10**12, 7, 7], [(0, 5), (1, 4)]),
    ]
    rng = random.Random(242_242_07)
    for _ in range(96):
        n = rng.randrange(0, 45)
        palette = [rng.randint(-10**12, 10**12) for _ in range(rng.randint(1, 12))]
        values = [rng.choice(palette) for _ in range(n)]
        ranges = []
        for _ in range(rng.randint(0, 50)):
            left = rng.randrange(n + 1)
            right = rng.randrange(left, n + 1)
            ranges.append((left, right))
        cases.append((values, ranges))

    for index, (values, ranges) in enumerate(cases):
        write_case(out_dir, index, values, ranges)


if __name__ == "__main__":
    main()
