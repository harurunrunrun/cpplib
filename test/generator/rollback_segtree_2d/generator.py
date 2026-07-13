#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def rectangle_sum(
    values: list[list[int]],
    top: int,
    left: int,
    bottom: int,
    right: int,
) -> int:
    return sum(
        values[row][col]
        for row in range(top, bottom)
        for col in range(left, right)
    )


def make_case(
    out_dir: Path,
    index: int,
    height: int,
    width: int,
    query_count: int,
    seed: int,
) -> None:
    rng = random.Random(seed)
    initial = [
        [rng.randint(-30, 30) for _ in range(width)]
        for _ in range(height)
    ]
    states = [[row[:] for row in initial]]
    commands: list[str] = []
    output: list[str] = []

    for _ in range(query_count):
        choice = rng.randrange(12)
        if choice < 5 or len(states) == 1:
            row = rng.randrange(height)
            col = rng.randrange(width)
            value = rng.randint(-50, 50)
            next_values = [line[:] for line in states[-1]]
            if choice < 4:
                commands.append(f"SET {row} {col} {value}")
                next_values[row][col] = value
            else:
                commands.append(f"ADD {row} {col} {value}")
                next_values[row][col] += value
            states.append(next_values)
        elif choice == 5:
            commands.append("UNDO")
            states.pop()
        elif choice == 6:
            target = rng.randrange(len(states))
            commands.append(f"ROLLBACK {target}")
            del states[target + 1:]
        elif choice < 8:
            row = rng.randrange(height)
            col = rng.randrange(width)
            commands.append(f"GET {row} {col}")
            output.append(str(states[-1][row][col]))
        elif choice < 10:
            top, bottom = sorted((rng.randrange(height + 1), rng.randrange(height + 1)))
            left, right = sorted((rng.randrange(width + 1), rng.randrange(width + 1)))
            commands.append(f"PROD {top} {left} {bottom} {right}")
            output.append(str(rectangle_sum(states[-1], top, left, bottom, right)))
        elif choice == 10:
            commands.append("ALL")
            output.append(str(rectangle_sum(states[-1], 0, 0, height, width)))
        else:
            commands.append("SNAPSHOT")
            output.append(str(len(states) - 1))

    input_lines = [f"{height} {width} {len(commands)}"]
    input_lines.extend(" ".join(map(str, row)) for row in initial)
    input_lines.extend(commands)
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, parameters in enumerate((
        (1, 1, 80, 2026071501),
        (4, 5, 260, 2026071502),
        (8, 8, 420, 2026071503),
    )):
        make_case(out_dir, index, *parameters)


if __name__ == "__main__":
    main()
