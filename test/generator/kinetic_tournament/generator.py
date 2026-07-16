#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def best(lines: list[tuple[int, int] | None], left: int, right: int,
         time: int) -> str:
    candidates = []
    for index in range(left, right):
        line = lines[index]
        if line is None:
            continue
        slope, intercept = line
        candidates.append((slope * time + intercept, slope, index))
    if not candidates:
        return "EMPTY"
    value, _, index = min(candidates)
    return f"{value} {index}"


def make_case(seed: int, size: int, count: int) -> tuple[list[str], list[str], int]:
    rng = random.Random(seed)
    time = -10**6
    initial_time = time
    lines: list[tuple[int, int] | None] = [None] * size
    commands: list[str] = []
    output: list[str] = []
    for _ in range(count):
        operation = rng.randrange(10)
        if operation < 4:
            index = rng.randrange(size)
            slope = rng.randrange(-10**6, 10**6 + 1)
            intercept = rng.randrange(-10**12, 10**12 + 1)
            lines[index] = (slope, intercept)
            commands.append(f"S {index} {slope} {intercept}")
        elif operation == 4:
            index = rng.randrange(size)
            lines[index] = None
            commands.append(f"E {index}")
        elif operation < 7:
            time += rng.randrange(0, 10000)
            commands.append(f"T {time}")
        else:
            left = rng.randrange(size + 1)
            right = rng.randrange(size + 1)
            if left > right:
                left, right = right, left
            commands.append(f"Q {left} {right}")
            output.append(best(lines, left, right, time))
    return commands, output, initial_time


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    for index, (seed, size, count) in enumerate([
        (2026071622, 1, 100), (2026071623, 97, 10000),
        (2026071624, 20000, 50000),
    ]):
        commands, output, initial_time = make_case(seed, size, count)
        (directory / f"case_{index:02d}.in").write_text(
            f"{size} {len(commands)} {initial_time}\n" + "\n".join(commands) + "\n",
            encoding="utf-8",
        )
        (directory / f"case_{index:02d}.out").write_text(
            "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
        )


if __name__ == "__main__":
    main()
