#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(coin_count: int, operations: list[tuple[int, int, int]]) -> str:
    coins = [0] * coin_count
    answers: list[str] = []
    for operation, left, right in operations:
        if operation == 0:
            for index in range(left, right + 1):
                coins[index] ^= 1
        else:
            answers.append(str(sum(coins[left : right + 1])))
    return "".join(answer + "\n" for answer in answers)


def write_case(
    out_dir: Path,
    case_id: int,
    coin_count: int,
    operations: list[tuple[int, int, int]],
) -> None:
    body = [f"{coin_count} {len(operations)}"]
    body.extend(f"{operation} {left} {right}" for operation, left, right in operations)
    stem = out_dir / f"case_{case_id:02d}"
    stem.with_suffix(".in").write_text("\n".join(body) + "\n", encoding="utf-8")
    stem.with_suffix(".out").write_text(solve(coin_count, operations), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        args.out_dir,
        0,
        1,
        [(1, 0, 0), (0, 0, 0), (1, 0, 0), (0, 0, 0), (1, 0, 0)],
    )

    rng = random.Random(20260714)
    for case_id, coin_count in enumerate((2, 17, 64), start=1):
        operations: list[tuple[int, int, int]] = []
        for _ in range(250):
            left = rng.randrange(coin_count)
            right = rng.randrange(left, coin_count)
            operations.append((rng.randrange(2), left, right))
        write_case(args.out_dir, case_id, coin_count, operations)


if __name__ == "__main__":
    main()
