#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def write_case(directory: Path, index: int, values: list[int],
               commands: list[str], output: list[str]) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        f"{len(values)} {len(commands)}\n" + " ".join(map(str, values)) + "\n" +
        "\n".join(commands) + "\n", encoding="utf-8"
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def make_case(seed: int, size: int, queries: int) -> tuple[list[int], list[str], list[str]]:
    rng = random.Random(seed)
    values = [rng.randrange(-10**15, 10**15 + 1) for _ in range(size)]
    if size >= 4:
        values[-4:] = [-(1 << 63), (1 << 63) - 1, 0, 0]
    unique = sorted(set(values))
    unique_set = set(unique)
    commands: list[str] = []
    output: list[str] = []
    for _ in range(queries):
        operation = rng.randrange(5)
        probe = rng.choice(values) if rng.randrange(3) == 0 else rng.randrange(-10**18, 10**18)
        if operation == 0:
            probe = rng.choice(values)
            commands.append(f"INDEX {probe}")
            output.append(str(bisect.bisect_left(unique, probe)))
        elif operation == 1:
            commands.append(f"LOWER {probe}")
            output.append(str(bisect.bisect_left(unique, probe)))
        elif operation == 2:
            commands.append(f"UPPER {probe}")
            output.append(str(bisect.bisect_right(unique, probe)))
        elif operation == 3:
            commands.append(f"CONTAINS {probe}")
            output.append(str(int(probe in unique_set)))
        else:
            index = rng.randrange(len(unique))
            commands.append(f"VALUE {index}")
            output.append(str(unique[index]))
    return values, commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    values = [5, -1, 5, 9, 0]
    commands = ["INDEX -1", "INDEX 5", "LOWER 4", "UPPER 5",
                "CONTAINS 8", "VALUE 3"]
    output = ["0", "2", "2", "3", "0", "9"]
    write_case(directory, 0, values, commands, output)
    for index, parameters in enumerate(
        [(2026071606, 1, 500), (2026071607, 500, 5000),
         (2026071608, 50000, 20000)], start=1
    ):
        values, commands, output = make_case(*parameters)
        write_case(directory, index, values, commands, output)


if __name__ == "__main__":
    main()

