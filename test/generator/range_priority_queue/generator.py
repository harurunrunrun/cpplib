#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def best(heaps: list[list[int]], left: int, right: int) -> tuple[int, int]:
    candidates = [(-heaps[index][0], index) for index in range(left, right) if heaps[index]]
    return max(candidates, key=lambda item: (item[0], -item[1]))


def make_case(seed: int, buckets: int, queries: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    heaps: list[list[int]] = [[] for _ in range(buckets)]
    commands: list[str] = []
    output: list[str] = []
    for _ in range(queries):
        left = rng.randrange(buckets + 1)
        right = rng.randrange(buckets + 1)
        if left > right:
            left, right = right, left
        nonempty = any(heaps[index] for index in range(left, right))
        operation = rng.randrange(10)
        if operation < 5 or not nonempty:
            index = rng.randrange(buckets)
            value = rng.randrange(-10**12, 10**12 + 1)
            commands.append(f"PUSH {index} {value}")
            heapq.heappush(heaps[index], -value)
        elif operation < 7:
            value, index = best(heaps, left, right)
            commands.append(f"TOP {left} {right}")
            output.append(f"{value} {index}")
        elif operation < 9:
            value, index = best(heaps, left, right)
            commands.append(f"POP {left} {right}")
            output.append(str(value))
            heapq.heappop(heaps[index])
        else:
            commands.append(f"EMPTY {left} {right}")
            output.append(str(int(not nonempty)))
    for index, heap in enumerate(heaps):
        commands.append(f"COUNT {index}")
        output.append(str(len(heap)))
    return commands, output


def write_case(directory: Path, index: int, buckets: int,
               commands: list[str], output: list[str]) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        f"{buckets} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    commands = [
        "EMPTY 0 4", "PUSH 0 5", "PUSH 2 9", "PUSH 1 9", "PUSH 2 -4",
        "TOP 0 4", "POP 2 4", "TOP 0 4", "POP 0 2", "EMPTY 0 2",
        "COUNT 0", "COUNT 1", "COUNT 2", "COUNT 3",
    ]
    output = ["1", "9 1", "9", "9 1", "9", "0", "1", "0", "1", "0"]
    write_case(directory, 0, 4, commands, output)
    for index, (seed, buckets, queries) in enumerate(
        [(2026071602, 1, 2000), (2026071603, 37, 10000),
         (2026071604, 1000, 30000)], start=1
    ):
        commands, output = make_case(seed, buckets, queries)
        write_case(directory, index, buckets, commands, output)


if __name__ == "__main__":
    main()

