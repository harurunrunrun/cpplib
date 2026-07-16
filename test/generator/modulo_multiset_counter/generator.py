#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def make_small(seed: int, maximum: int, queries: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    counts = collections.Counter(range(1, maximum + 1))
    commands: list[str] = []
    output: list[str] = []
    for _ in range(queries):
        operation = rng.randrange(10)
        if operation < 3:
            modulus = rng.randrange(1, 200)
            commands.append(f"MOD {modulus}")
            next_counts: collections.Counter[int] = collections.Counter()
            for value, count in counts.items():
                next_counts[value % modulus] += count
            counts = next_counts
        elif operation < 7:
            value = rng.randrange(500)
            delta = rng.randrange(-1000, 1001)
            commands.append(f"ADD {value} {delta}")
            counts[value] += delta
            if counts[value] == 0:
                del counts[value]
        else:
            value = rng.randrange(500)
            commands.append(f"COUNT {value}")
            output.append(str(counts[value]))
    for value in range(200):
        commands.append(f"COUNT {value}")
        output.append(str(counts[value]))
    return commands, output


def write_case(directory: Path, index: int, maximum: int,
               commands: list[str], output: list[str]) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        f"{maximum} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    commands = ["COUNT 0", "COUNT 5", "MOD 3", "COUNT 0", "COUNT 1",
                "COUNT 2", "ADD 100 -7", "MOD 4", "COUNT 0"]
    output = ["0", "1", "1", "2", "2", "-6"]
    write_case(directory, 0, 5, commands, output)
    for index, parameters in enumerate(
        [(2026071612, 0, 3000), (2026071613, 100, 10000),
         (2026071614, 100000, 30000)], start=1
    ):
        commands, output = make_small(*parameters)
        write_case(directory, index, parameters[1], commands, output)

    maximum = (1 << 64) - 1
    commands = ["COUNT 1", "COUNT 18446744073709551615", "MOD 2",
                "COUNT 0", "COUNT 1", "MOD 1", "COUNT 0"]
    output = ["1", "1", str((1 << 63) - 1), str(1 << 63), str(maximum)]
    write_case(directory, 4, maximum, commands, output)


if __name__ == "__main__":
    main()

