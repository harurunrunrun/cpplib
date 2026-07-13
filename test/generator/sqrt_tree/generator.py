#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 1_000_000_007


def compose(
    left: tuple[int, int],
    right: tuple[int, int],
) -> tuple[int, int]:
    return (
        right[0] * left[0] % MOD,
        (right[0] * left[1] + right[1]) % MOD,
    )


def product(values: list[tuple[int, int]]) -> tuple[int, int]:
    result = values[0]
    for value in values[1:]:
        result = compose(result, value)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    sizes = [1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 255]
    for case_id, n in enumerate(sizes):
        values = [
            (rng.randrange(1, MOD), rng.randrange(MOD))
            for _ in range(n)
        ]
        queries = [(0, n)]
        if n <= 31:
            queries.extend((left, right) for left in range(n) for right in range(left + 1, n + 1))
        else:
            for _ in range(1200):
                left = rng.randrange(n)
                right = rng.randrange(left + 1, n + 1)
                queries.append((left, right))

        input_lines = [f"{n} {len(queries)}"]
        input_lines.extend(f"{a} {b}" for a, b in values)
        input_lines.extend(f"{left} {right}" for left, right in queries)

        all_product = product(values)
        output_lines = [f"{n} 0 {all_product[0]} {all_product[1]}"]
        output_lines.append(" ".join(f"{a}:{b}" for a, b in values))
        for left, right in queries:
            answer = product(values[left:right])
            output_lines.append(f"{answer[0]} {answer[1]}")

        (out_dir / f"case_{case_id:02d}.in").write_text(
            "\n".join(input_lines) + "\n",
            encoding="utf-8",
        )
        (out_dir / f"case_{case_id:02d}.out").write_text(
            "\n".join(output_lines) + "\n",
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
