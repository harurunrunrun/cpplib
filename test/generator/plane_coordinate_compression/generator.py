#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MIN_I64 = -(1 << 63)
MAX_I64 = (1 << 63) - 1


def compress(
    values: list[int], predecessor: bool, successor: bool
) -> list[int]:
    candidates: list[int] = []
    for value in values:
        if predecessor and value != MIN_I64:
            candidates.append(value - 1)
        candidates.append(value)
        if successor and value != MAX_I64:
            candidates.append(value + 1)
    return sorted(set(candidates))


def write_case(
    out_dir: Path,
    index: int,
    xs: list[int],
    ys: list[int],
    x_predecessor: bool,
    x_successor: bool,
    y_predecessor: bool,
    y_successor: bool,
) -> None:
    compressed_x = compress(xs, x_predecessor, x_successor)
    compressed_y = compress(ys, y_predecessor, y_successor)
    x_index = {value: position for position, value in enumerate(compressed_x)}
    y_index = {value: position for position, value in enumerate(compressed_y)}

    input_lines = [
        f"{len(xs)} {len(ys)} {int(x_predecessor)} {int(x_successor)} "
        f"{int(y_predecessor)} {int(y_successor)}",
        " ".join(map(str, xs)),
        " ".join(map(str, ys)),
    ]
    output_lines = [
        f"{len(compressed_x)} {len(compressed_y)}",
        " ".join(map(str, compressed_x)),
        " ".join(map(str, compressed_y)),
        " ".join(str(x_index[value]) for value in xs),
        " ".join(str(y_index[value]) for value in ys),
    ]
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[int], list[int], bool, bool, bool, bool]] = [
        ([], [], False, False, False, False),
        ([0], [0], False, False, False, False),
        ([0, 10, 10], [-5, 5], True, True, False, False),
        ([MIN_I64, MAX_I64], [MIN_I64, MAX_I64], True, True, True, True),
        ([MIN_I64], [MAX_I64], True, False, False, True),
        ([-2, -1, 0, 1, 2], [7, 7, 7], False, True, True, False),
    ]

    generator = random.Random(2026071603)
    for _ in range(40):
        nx = generator.randrange(0, 100)
        ny = generator.randrange(0, 100)
        xs = [generator.randrange(-10**12, 10**12 + 1) for _ in range(nx)]
        ys = [generator.randrange(-10**12, 10**12 + 1) for _ in range(ny)]
        if xs and generator.randrange(4) == 0:
            xs[0] = MIN_I64
        if ys and generator.randrange(4) == 0:
            ys[-1] = MAX_I64
        cases.append(
            (
                xs,
                ys,
                bool(generator.randrange(2)),
                bool(generator.randrange(2)),
                bool(generator.randrange(2)),
                bool(generator.randrange(2)),
            )
        )

    cases.append(
        (
            list(range(-500, 500)),
            list(range(1000, 2000)),
            True,
            True,
            True,
            True,
        )
    )
    for index, case in enumerate(cases):
        write_case(args.out_dir, index, *case)


if __name__ == "__main__":
    main()
