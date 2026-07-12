#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def pow_sum(ratio: int, count: int) -> tuple[int, int]:
    if count == 0:
        return 1, 0
    power, total = pow_sum(ratio, count // 2)
    squared_power = power * power % MOD
    doubled_sum = total * (1 + power) % MOD
    if count % 2 == 0:
        return squared_power, doubled_sum
    return squared_power * ratio % MOD, (doubled_sum + squared_power) % MOD


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    queries: list[tuple[int, ...]] = [
        (0, 3, 2, 5),
        (0, 10, -3, 4),
        (0, -7, 5, 0),
        (1, 0, 0),
        (1, 1, 20),
        (1, -2, 19),
        (2, 2, 3, 5),
        (2, -5, 1, 17),
        (3, 0, 10**18),
        (3, 1, 10**18 + 7),
        (3, -1, 9_000_000_000_000_000_000),
        (3, 998244353, 8_765_432_101_234_567_890),
    ]
    for _ in range(180):
        query_type = rng.randrange(4)
        if query_type == 0:
            queries.append((0, rng.randrange(-1000, 1001), rng.randrange(-1000, 1001), rng.randrange(1001)))
        elif query_type == 1:
            queries.append((1, rng.randrange(-5, 6), rng.randrange(21)))
        elif query_type == 2:
            queries.append((2, rng.randrange(-10, 11), rng.randrange(-5, 6), rng.randrange(19)))
        else:
            queries.append((3, rng.randrange(-2 * MOD, 2 * MOD + 1), rng.randrange(10**18 + 1)))

    input_lines = [str(len(queries))]
    output_lines: list[str] = []
    for query in queries:
        input_lines.append(" ".join(map(str, query)))
        query_type = query[0]
        if query_type == 0:
            _, first, diff, count = query
            value = count * (2 * first + (count - 1) * diff) // 2
            output_lines.append(f"{value} {value % MOD}")
        elif query_type == 1:
            _, ratio, count = query
            value = sum(ratio**exponent for exponent in range(count))
            output_lines.append(f"{value} {value % MOD}")
        elif query_type == 2:
            _, first, ratio, count = query
            value = first * sum(ratio**exponent for exponent in range(count))
            output_lines.append(f"{value} {value % MOD}")
        else:
            _, ratio, count = query
            power, total = pow_sum(ratio % MOD, count)
            output_lines.append(f"{power} {total}")

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
