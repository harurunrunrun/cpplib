#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    initial: list[int],
    operations: list[tuple[str, int, int]],
) -> None:
    values = initial[:]
    output: list[str] = []
    lines = [f"{len(values)} {len(operations)}", " ".join(map(str, values))]
    for kind, first, second in operations:
        lines.append(f"{kind} {first} {second}")
        if kind == "S":
            values[first] = second
        else:
            largest = sorted(values[first:second], reverse=True)[:2]
            output.append(str(largest[0] + largest[1]))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_case(rng: random.Random, size: int, count: int):
    initial = [rng.randint(-10**9, 10**9) for _ in range(size)]
    operations: list[tuple[str, int, int]] = []
    for step in range(count):
        if step % 3 == 0:
            operations.append(
                ("S", rng.randrange(size), rng.randint(-10**9, 10**9))
            )
        else:
            left = rng.randrange(size - 1)
            right = rng.randrange(left + 2, size + 1)
            operations.append(("Q", left, right))
    return initial, operations


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([-5, -5], [("Q", 0, 2), ("S", 0, -9), ("Q", 0, 2)]),
        ([7, 7, 7, 1], [("Q", 0, 4), ("Q", 1, 3), ("S", 2, 9), ("Q", 0, 3)]),
    ]
    rng = random.Random(2026071401)
    for size in (2, 3, 7, 31, 127, 256):
        cases.append(random_case(rng, size, 240))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
