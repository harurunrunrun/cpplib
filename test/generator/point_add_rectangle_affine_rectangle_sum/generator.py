#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    directory: Path,
    index: int,
    commands: list[str],
    answers: list[int],
) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="utf-8",
    )


def make_random_case(rng: random.Random):
    points: dict[tuple[int, int], int] = {}
    commands: list[str] = []
    answers: list[int] = []
    operations = ["P", "P", "A", "Q", "Q", "N"]
    for _ in range(8000):
        operation = rng.choice(operations)
        if operation == "P":
            x = rng.randrange(-30, 31)
            y = rng.randrange(-30, 31)
            delta = rng.randrange(-100, 101)
            key = (x, y)
            answers.append(int(key not in points))
            points[key] = points.get(key, 0) + delta
            commands.append(f"P {x} {y} {delta}")
        elif operation == "A":
            x_left, x_right = sorted(
                (rng.randrange(-31, 32), rng.randrange(-31, 32))
            )
            y_lower, y_upper = sorted(
                (rng.randrange(-31, 32), rng.randrange(-31, 32))
            )
            multiplier = rng.choice([-1, 0, 1])
            addend = rng.randrange(-20, 21)
            commands.append(
                f"A {x_left} {x_right} {y_lower} {y_upper} "
                f"{multiplier} {addend}"
            )
            for key in list(points):
                x, y = key
                if x_left <= x < x_right and y_lower <= y < y_upper:
                    points[key] = multiplier * points[key] + addend
        elif operation == "Q":
            x_left, x_right = sorted(
                (rng.randrange(-31, 32), rng.randrange(-31, 32))
            )
            y_lower, y_upper = sorted(
                (rng.randrange(-31, 32), rng.randrange(-31, 32))
            )
            commands.append(f"Q {x_left} {x_right} {y_lower} {y_upper}")
            answers.append(
                sum(
                    value
                    for (x, y), value in points.items()
                    if x_left <= x < x_right and y_lower <= y < y_upper
                )
            )
        else:
            commands.append("N")
            answers.append(len(points))
    return commands, answers


def make_large_case():
    base = 1_000_000_000_000
    point_count = 12_000
    points: dict[tuple[int, int], int] = {}
    commands: list[str] = []
    answers: list[int] = []
    for index in range(point_count):
        key = (base + index, base + (index * 7919) % (2 * point_count))
        value = (index % 101) - 50
        points[key] = value
        commands.append(f"P {key[0]} {key[1]} {value}")
        answers.append(1)
    for iteration in range(240):
        x_left = base + (iteration * 43) % point_count
        x_right = min(base + point_count, x_left + 3000)
        y_lower = base + (iteration * 97) % (2 * point_count)
        y_upper = min(base + 2 * point_count, y_lower + 5000)
        if iteration % 3 == 0:
            multiplier = [-1, 0, 1][iteration % 3]
            addend = (iteration % 17) - 8
            commands.append(
                f"A {x_left} {x_right} {y_lower} {y_upper} "
                f"{multiplier} {addend}"
            )
            for key in list(points):
                if x_left <= key[0] < x_right and y_lower <= key[1] < y_upper:
                    points[key] = multiplier * points[key] + addend
        else:
            commands.append(f"Q {x_left} {x_right} {y_lower} {y_upper}")
            answers.append(
                sum(
                    value
                    for key, value in points.items()
                    if x_left <= key[0] < x_right and y_lower <= key[1] < y_upper
                )
            )
    duplicate = (base + 1234, base + (1234 * 7919) % (2 * point_count))
    commands.extend(
        [
            f"P {duplicate[0]} {duplicate[1]} 7",
            f"Q {base} {base + point_count} {base} {base + 2 * point_count}",
            "N",
        ]
    )
    answers.extend([0, sum(points.values()) + 7, point_count])
    return commands, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    write_case(directory, 0, *make_random_case(random.Random(2026071605)))
    write_case(directory, 1, *make_large_case())


if __name__ == "__main__":
    main()
