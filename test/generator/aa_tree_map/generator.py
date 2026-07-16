#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def make_case(seed: int, queries: int, key_bound: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    keys: list[int] = []
    values: dict[int, int] = {}
    commands: list[str] = []
    output: list[str] = []
    for _ in range(queries):
        operation = rng.randrange(10)
        key = rng.randrange(-key_bound, key_bound + 1)
        value = rng.randrange(-(1 << 63), 1 << 63)
        position = bisect.bisect_left(keys, key)
        exists = position < len(keys) and keys[position] == key
        if operation == 0:
            commands.append(f"INSERT {key} {value}")
            output.append(str(int(not exists)))
            if not exists:
                keys.insert(position, key)
                values[key] = value
        elif operation == 1:
            commands.append(f"SET {key} {value}")
            output.append(str(int(not exists)))
            if not exists:
                keys.insert(position, key)
            values[key] = value
        elif operation == 2:
            commands.append(f"ERASE {key}")
            output.append(str(int(exists)))
            if exists:
                keys.pop(position)
                del values[key]
        elif operation == 3:
            commands.append(f"GET {key}")
            output.append(str(values[key]) if exists else "NONE")
        elif operation == 4:
            commands.append(f"LOWER {key}")
            output.append(
                f"{keys[position]} {values[keys[position]]}" if position < len(keys)
                else "NONE"
            )
        elif operation == 5:
            upper = bisect.bisect_right(keys, key)
            commands.append(f"UPPER {key}")
            output.append(
                f"{keys[upper]} {values[keys[upper]]}" if upper < len(keys)
                else "NONE"
            )
        elif operation == 6:
            commands.append(f"ORDER {key}")
            output.append(str(position))
        elif operation == 7 and keys:
            index = rng.randrange(len(keys))
            commands.append(f"KTH {index}")
            output.append(f"{keys[index]} {values[keys[index]]}")
        else:
            commands.append("SIZE")
            output.append(str(len(keys)))
    return commands, output


def write_case(directory: Path, index: int,
               commands: list[str], output: list[str]) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
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

    for index, parameters in enumerate([
        (2026071625, 3000, 20),
        (2026071626, 20000, 1000),
        (2026071627, 100000, 100000),
    ]):
        commands, output = make_case(*parameters)
        write_case(directory, index, commands, output)


if __name__ == "__main__":
    main()

