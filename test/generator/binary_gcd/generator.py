#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def write_case(path: Path, commands: list[str], answers: list[int]) -> None:
    path.mkdir(parents=True, exist_ok=True)
    (path / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (path / "case_00.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)

    rng = random.Random(20260716)
    commands: list[str] = []
    answers: list[int] = []

    unsigned_edges = [
        (0, 0),
        (0, (1 << 64) - 1),
        ((1 << 64) - 1, (1 << 64) - 1),
        (1 << 63, 1 << 62),
        ((1 << 64) - 1, (1 << 64) - 3),
    ]
    signed_edges = [
        (-(1 << 63), 0),
        (-(1 << 63), -(1 << 63)),
        (-(1 << 63), (1 << 63) - 1),
        (-48, 18),
    ]
    for first, second in unsigned_edges:
        commands.append(f"U {first} {second}")
        answers.append(math.gcd(first, second))
    for first, second in signed_edges:
        commands.append(f"S {first} {second}")
        answers.append(math.gcd(first, second))

    for _ in range(5000):
        if rng.randrange(2):
            first = rng.getrandbits(64)
            second = rng.getrandbits(64)
            commands.append(f"U {first} {second}")
        else:
            first = rng.randrange(-(1 << 63), 1 << 63)
            second = rng.randrange(-(1 << 63), 1 << 63)
            commands.append(f"S {first} {second}")
        answers.append(math.gcd(first, second))

    write_case(out_dir, commands, answers)

    large_commands: list[str] = []
    large_answers: list[int] = []
    for index in range(100_000):
        first = ((1 << 64) - 1) - index
        second = (index + 1) * 2
        large_commands.append(f"U {first} {second}")
        large_answers.append(math.gcd(first, second))
    (out_dir / "case_01.in").write_text(
        str(len(large_commands)) + "\n" + "\n".join(large_commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_01.out").write_text(
        "\n".join(map(str, large_answers)) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
