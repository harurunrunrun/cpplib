#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def convolution(convex: list[int], arbitrary: list[int]) -> list[int]:
    return [
        min(
            convex[left] + arbitrary[index - left]
            for left in range(max(0, index - len(arbitrary) + 1), min(len(convex), index + 1))
        )
        for index in range(len(convex) + len(arbitrary) - 1)
    ]


def write_case(
    out_dir: Path, index: int, convex: list[int], arbitrary: list[int]
) -> None:
    name = f"case_{index:03d}"
    lines = [
        f"{len(convex)} {len(arbitrary)}",
        " ".join(map(str, convex)),
        " ".join(map(str, arbitrary)),
    ]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        " ".join(map(str, convolution(convex, arbitrary))) + "\n",
        encoding="ascii",
    )


def random_convex(rng: random.Random, size: int) -> list[int]:
    value = rng.randrange(-10000, 10001)
    slope = rng.randrange(-100, 101)
    result = [value]
    for _ in range(1, size):
        slope += rng.randrange(0, 10)
        value += slope
        result.append(value)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [0], [0])
    write_case(out_dir, 1, [0, 1, 4], [3, -2])
    rng = random.Random(2026071402)
    for index in range(2, 42):
        convex_size = rng.randrange(1, 45)
        arbitrary_size = rng.randrange(1, 45)
        write_case(
            out_dir,
            index,
            random_convex(rng, convex_size),
            [rng.randrange(-100000, 100001) for _ in range(arbitrary_size)],
        )


if __name__ == "__main__":
    main()
