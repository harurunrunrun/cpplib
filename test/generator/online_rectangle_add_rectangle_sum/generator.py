#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    directory: Path,
    index: int,
    low: int,
    high: int,
    commands: list[str],
    answers: list[int],
) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        f"{low} {high} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="utf-8",
    )


def make_random_case(rng: random.Random):
    low = -20
    high = 21
    cells = [[0] * (high - low) for _ in range(high - low)]
    commands: list[str] = []
    answers: list[int] = []
    for _ in range(6000):
        x_first = rng.randrange(low, high + 1)
        x_second = rng.randrange(low, high + 1)
        y_first = rng.randrange(low, high + 1)
        y_second = rng.randrange(low, high + 1)
        x_left, x_right = sorted((x_first, x_second))
        y_lower, y_upper = sorted((y_first, y_second))
        if rng.randrange(2) == 0:
            value = rng.randrange(-30, 31)
            commands.append(
                f"A {x_left} {x_right} {y_lower} {y_upper} {value}"
            )
            for x in range(x_left, x_right):
                for y in range(y_lower, y_upper):
                    cells[x - low][y - low] += value
        else:
            commands.append(f"Q {x_left} {x_right} {y_lower} {y_upper}")
            answers.append(
                sum(
                    cells[x - low][y - low]
                    for x in range(x_left, x_right)
                    for y in range(y_lower, y_upper)
                )
            )
    return low, high, commands, answers


def make_large_coordinate_case():
    low = 1_000_000_000_000
    high = low + 1_000_000
    width = high - low
    commands = [f"A {low} {high} {low} {high} 3"]
    answers: list[int] = []
    cell_delta: dict[tuple[int, int], int] = {}
    for index in range(800):
        x = low + (index * 1009) % width
        y = low + (index * 9176 + 13) % width
        delta = (index % 19) - 9
        commands.append(f"A {x} {x + 1} {y} {y + 1} {delta}")
        cell_delta[(x, y)] = cell_delta.get((x, y), 0) + delta
    for index, ((x, y), delta) in enumerate(cell_delta.items()):
        if index >= 700:
            break
        commands.append(f"Q {x} {x + 1} {y} {y + 1}")
        answers.append(3 + delta)
    commands.extend(
        [
            f"Q {low} {high} {low} {high}",
            f"Q {low} {low} {low} {high}",
            f"Q {high - 1} {high} {high - 1} {high}",
        ]
    )
    answers.extend(
        [
            3 * width * width + sum(cell_delta.values()),
            0,
            3 + cell_delta.get((high - 1, high - 1), 0),
        ]
    )
    return low, high, commands, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    write_case(directory, 0, *make_random_case(random.Random(2026071604)))
    write_case(directory, 1, *make_large_coordinate_case())


if __name__ == "__main__":
    main()
