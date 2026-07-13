#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def write_case(out_dir: Path, index: int, initial: list[int], k: int,
               commands: list[str], output: list[str]) -> None:
    lines = [f"{len(initial)} {k} {len(commands)}", " ".join(map(str, initial))]
    lines.extend(commands)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_case(seed: int, initial_size: int,
                query_count: int) -> tuple[list[int], int, list[str], list[str]]:
    rng = random.Random(seed)
    initial = [rng.randrange(-1000, 1001) for _ in range(initial_size)]
    values = sorted(initial)
    k = rng.randrange(len(values) + 1)
    current_k = k
    commands: list[str] = []
    output: list[str] = []
    for _ in range(query_count):
        operation = rng.randrange(12)
        if operation < 3 or not values:
            value = rng.randrange(-1000, 1001)
            commands.append(f"INSERT {value}")
            bisect.insort(values, value)
        elif operation < 5 and len(values) > current_k:
            index = rng.randrange(len(values))
            value = values.pop(index)
            commands.append(f"ERASE {value}")
        elif operation < 7:
            current_k = rng.randrange(len(values) + 1)
            commands.append(f"SETK {current_k}")
        elif operation < 9:
            commands.append("SMALL")
            output.append(str(sum(values[:current_k])))
        elif operation < 11:
            commands.append("LARGE")
            output.append(str(sum(values[len(values) - current_k:])))
        else:
            commands.extend(["SIZE", "K"])
            output.extend([str(len(values)), str(current_k)])
    return initial, k, commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    boundary_initial = [5, -2, 5, 7, -2]
    boundary_commands = [
        "SMALL", "LARGE", "SETK 5", "SMALL", "LARGE", "SETK 4",
        "ERASE 5", "SMALL", "LARGE", "SETK 2", "SMALL", "LARGE",
        "INSERT -100", "SMALL", "LARGE", "SIZE", "K",
    ]
    boundary_output = [
        "0", "0", "13", "13", "8", "8", "-4", "12", "-102", "12", "5", "2",
    ]
    write_case(out_dir, 0, boundary_initial, 0, boundary_commands, boundary_output)

    for index, seed in enumerate([2026071325, 2026071326, 2026071327], start=1):
        initial, k, commands, output = random_case(seed, 120, 3000)
        write_case(out_dir, index, initial, k, commands, output)

    rng = random.Random(2026071328)
    large_initial = [rng.randrange(-10**9, 10**9 + 1) for _ in range(100000)]
    sorted_values = sorted(large_initial)
    prefix = [0]
    for value in sorted_values:
        prefix.append(prefix[-1] + value)
    total = prefix[-1]
    large_commands: list[str] = []
    large_output: list[str] = []
    for _ in range(12000):
        k = rng.randrange(len(sorted_values) + 1)
        large_commands.extend([f"SETK {k}", "SMALL", "LARGE"])
        large_output.extend([str(prefix[k]), str(total - prefix[len(sorted_values) - k])])
    write_case(out_dir, 4, large_initial, 0, large_commands, large_output)


if __name__ == "__main__":
    main()
