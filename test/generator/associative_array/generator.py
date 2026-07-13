#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(out_dir: Path, index: int, commands: list[str], output: list[str]) -> None:
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
    )


def random_case(seed: int, query_count: int, key_range: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    table: dict[int, int] = {}
    commands: list[str] = []
    output: list[str] = []
    for _ in range(query_count):
        operation = rng.randrange(12)
        key = rng.randrange(key_range)
        if operation < 4:
            value = rng.randrange(-(10**18), 10**18 + 1)
            commands.append(f"SET {key} {value}")
            table[key] = value
        elif operation < 6:
            value = rng.randrange(-10**9, 10**9 + 1)
            commands.append(f"ADD {key} {value}")
            table[key] = table.get(key, 0) + value
        elif operation < 8:
            commands.append(f"GET {key}")
            output.append(str(table.get(key, 0)))
        elif operation == 8:
            commands.append(f"ERASE {key}")
            output.append("1" if key in table else "0")
            table.pop(key, None)
        elif operation == 9:
            commands.append(f"CONTAINS {key}")
            output.append("1" if key in table else "0")
        elif operation == 10:
            commands.append("SIZE")
            output.append(str(len(table)))
        else:
            commands.append("CLEAR")
            table.clear()
    return commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    commands = [
        "GET 0", "SET 0 7", "SET 18446744073709551615 -9",
        "GET 0", "GET 18446744073709551615", "SIZE", "ERASE 0",
        "ERASE 0", "CONTAINS 0", "ADD 0 12", "ADD 0 -5", "GET 0",
        "CLEAR", "SIZE", "GET 18446744073709551615",
    ]
    output = ["0", "7", "-9", "2", "1", "0", "0", "7", "0", "0"]
    write_case(out_dir, 0, commands, output)

    for index, seed in enumerate([2026071411, 2026071412, 2026071413], start=1):
        commands, output = random_case(seed, 8000, 3000)
        write_case(out_dir, index, commands, output)

    commands, output = random_case(2026071414, 80000, 150000)
    write_case(out_dir, 4, commands, output)


if __name__ == "__main__":
    main()
