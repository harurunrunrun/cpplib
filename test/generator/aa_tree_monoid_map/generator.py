#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


Command = tuple[int, ...]


class Fenwick:
    def __init__(self, size: int) -> None:
        self.data = [0] * (size + 1)

    def add(self, index: int, value: int) -> None:
        index += 1
        while index < len(self.data):
            self.data[index] += value
            index += index & -index

    def prefix(self, end: int) -> int:
        result = 0
        while end > 0:
            result += self.data[end]
            end -= end & -end
        return result

    def kth(self, order: int) -> int:
        index = 0
        step = 1 << (len(self.data).bit_length() - 1)
        while step:
            candidate = index + step
            if candidate < len(self.data) and self.data[candidate] <= order:
                order -= self.data[candidate]
                index = candidate
            step >>= 1
        return index


def solve(commands: list[Command]) -> list[str]:
    coordinates = sorted({command[1] for command in commands
                          if command[0] in (0, 1)})
    index = {key: position for position, key in enumerate(coordinates)}
    count = Fenwick(len(coordinates))
    total = Fenwick(len(coordinates))
    values: dict[int, int] = {}
    output: list[str] = []

    for command in commands:
        kind = command[0]
        if kind == 0:
            key, value = command[1], command[2]
            position = index[key]
            if key not in values:
                count.add(position, 1)
                total.add(position, value)
            else:
                total.add(position, value - values[key])
            values[key] = value
        elif kind == 1:
            key = command[1]
            if key in values:
                position = index[key]
                count.add(position, -1)
                total.add(position, -values.pop(key))
        elif kind == 2:
            output.append(str(values.get(command[1], -1)))
        elif kind == 3:
            end = bisect.bisect_right(coordinates, command[1])
            output.append(str(count.prefix(end)))
        elif kind == 4:
            order = command[1]
            size = count.prefix(len(coordinates))
            if order < 0 or order >= size:
                output.append("-1")
            else:
                position = count.kth(order)
                key = coordinates[position]
                output.append(f"{key} {values[key]}")
        else:
            lower, upper = command[1], command[2]
            left = bisect.bisect_left(coordinates, lower)
            right = bisect.bisect_left(coordinates, upper)
            output.append(str(total.prefix(right) - total.prefix(left)))
    return output


def random_commands(seed: int, query_count: int, key_bound: int) -> list[Command]:
    generator = random.Random(seed)
    values: dict[int, int] = {}
    commands: list[Command] = []
    special_keys = [0, 1, key_bound // 2, key_bound - 1, 10**9 - 1]

    def key() -> int:
        if generator.randrange(12) == 0:
            return generator.choice(special_keys)
        return generator.randrange(key_bound)

    for _ in range(query_count):
        operation = generator.randrange(100)
        if operation < 30:
            current_key = key()
            value = generator.randrange(10**9 + 1)
            commands.append((0, current_key, value))
            values[current_key] = value
        elif operation < 44:
            current_key = key()
            commands.append((1, current_key))
            values.pop(current_key, None)
        elif operation < 58:
            commands.append((2, key()))
        elif operation < 72:
            commands.append((3, key()))
        elif operation < 84:
            if values and generator.randrange(4) != 0:
                order = generator.randrange(len(values))
            else:
                order = len(values) + generator.randrange(8)
            commands.append((4, order))
        else:
            lower = key()
            upper = key()
            if lower == upper:
                upper = min(10**9, lower + 1)
                if lower == upper:
                    lower -= 1
            if lower > upper:
                lower, upper = upper, lower
            commands.append((5, lower, upper))
    return commands


def write_case(directory: Path, index: int, commands: list[Command]) -> None:
    input_lines = [str(len(commands))]
    input_lines.extend(" ".join(map(str, command)) for command in commands)
    output = solve(commands)
    (directory / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[Command]] = [
        [],
        [
            (2, 0),
            (3, 0),
            (4, 0),
            (5, 0, 1),
            (0, 0, 7),
            (0, 10**9 - 1, 11),
            (0, 5, 13),
            (3, 5),
            (4, 0),
            (4, 2),
            (5, 0, 10**9),
            (0, 5, 17),
            (2, 5),
            (1, 0),
            (1, 0),
            (4, 2),
            (5, 5, 10**9 - 1),
        ],
        random_commands(2026071702, 5000, 31),
        random_commands(2026071703, 50000, 100000),
        random_commands(2026071704, 150000, 10**9),
    ]
    for index, commands in enumerate(cases):
        write_case(arguments.out_dir, index, commands)


if __name__ == "__main__":
    main()
