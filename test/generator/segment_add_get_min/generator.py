#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(out_dir: Path, index: int, operations: list[str]) -> None:
    segments: list[tuple[int, int, int, int]] = []
    output: list[str] = []
    for operation in operations:
        tokens = operation.split()
        if tokens[0] == "ADD":
            left, right, slope, intercept = map(int, tokens[1:])
            segments.append((left, right, slope, intercept))
        elif tokens[0] == "LINE":
            slope, intercept = map(int, tokens[1:])
            segments.append((-(10**30), 10**30, slope, intercept))
        else:
            x = int(tokens[1])
            values = [slope * x + intercept for left, right, slope, intercept in segments if left <= x < right]
            output.append(str(min(values)) if values else "INFINITY")
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(operations)}\n" + "\n".join(operations) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
    )


def random_operations(seed: int, count: int) -> list[str]:
    rng = random.Random(seed)
    result: list[str] = []
    for _ in range(count):
        kind = rng.randrange(10)
        if kind < 6:
            left = rng.randrange(-10**9, 10**9)
            right = rng.randrange(left + 1, 10**9 + 1)
            slope = rng.randrange(-10**9, 10**9 + 1)
            intercept = rng.randrange(-(10**18), 10**18 + 1)
            result.append(f"ADD {left} {right} {slope} {intercept}")
        elif kind == 6:
            slope = rng.randrange(-10**9, 10**9 + 1)
            intercept = rng.randrange(-(10**18), 10**18 + 1)
            result.append(f"LINE {slope} {intercept}")
        else:
            result.append(f"QUERY {rng.randrange(-10**9, 10**9 + 1)}")
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [
        "QUERY 0", "ADD -5 5 2 3", "QUERY -5", "QUERY 4", "QUERY 5",
        "LINE -3 7", "QUERY -1000000000", "QUERY 0", "ADD 0 1 0 -100", "QUERY 0",
    ])
    write_case(out_dir, 1, ["ADD 0 10 1 2", "ADD -10 0 -2 3"])
    for index, seed in enumerate([2026071461, 2026071462, 2026071463], start=2):
        write_case(out_dir, index, random_operations(seed, 4500))


if __name__ == "__main__":
    main()
