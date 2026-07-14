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
    operations: list[tuple[int, ...]],
) -> None:
    versions = [initial[:]]
    lines = [
        f"{len(initial)} {len(operations)}",
        " ".join(map(str, initial)),
    ]
    output: list[str] = []
    for operation in operations:
        if operation[0] == 0:
            _, version, position, delta = operation
            next_values = versions[version][:]
            next_values[position] += delta
            versions.append(next_values)
            lines.append(f"A {version} {position} {delta}")
            output.append(str(len(versions) - 1))
        else:
            _, version, left, right = operation
            lines.append(f"S {version} {left} {right}")
            output.append(str(sum(versions[version][left:right])))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_case(rng: random.Random, size: int, count: int):
    initial = [rng.randint(-10**9, 10**9) for _ in range(size)]
    operations: list[tuple[int, ...]] = []
    version_count = 1
    for step in range(count):
        if step % 3:
            operations.append(
                (
                    0,
                    rng.randrange(version_count),
                    rng.randrange(size),
                    rng.randint(-10**9, 10**9),
                )
            )
            version_count += 1
        else:
            left = rng.randrange(size + 1)
            right = rng.randrange(size + 1)
            if left > right:
                left, right = right, left
            operations.append((1, rng.randrange(version_count), left, right))
    return initial, operations


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([5], [(1, 0, 0, 1), (0, 0, 0, 7), (1, 0, 0, 1), (1, 1, 0, 1)]),
        ([1, 2, 3], [(0, 0, 1, 10), (0, 0, 1, -8), (1, 1, 0, 3), (1, 2, 0, 3)]),
    ]
    rng = random.Random(2026071403)
    for size in (2, 7, 31, 127, 256):
        cases.append(random_case(rng, size, 360))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
