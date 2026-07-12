#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def floor_sum(n: int, m: int, a: int, b: int) -> int:
    a_quotient, a_remainder = divmod(a, m)
    b_quotient, b_remainder = divmod(b, m)
    answer = n * (n - 1) // 2 * a_quotient + n * b_quotient
    while True:
        if a_remainder >= m:
            answer += n * (n - 1) // 2 * (a_remainder // m)
            a_remainder %= m
        if b_remainder >= m:
            answer += n * (b_remainder // m)
            b_remainder %= m
        y_max = a_remainder * n + b_remainder
        if y_max < m:
            return answer
        n, b_remainder = divmod(y_max, m)
        m, a_remainder = a_remainder, m


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    maximum = (1 << 63) - 1
    minimum = -(1 << 63)
    queries = [
        (0, 1, 0, 0),
        (1, 1, 0, 0),
        (10, 7, 3, 4),
        (10, 7, -3, 4),
        (10, 7, 3, -4),
        (10, 7, -3, -4),
        (1, maximum, minimum, minimum),
        (2, maximum, minimum, minimum),
        (100, 1, -1, 0),
        (maximum, maximum, 1, 0),
        (maximum, maximum, 1, maximum),
        (maximum, maximum, -1, maximum - 1),
        (maximum, maximum, 0, maximum),
        (maximum, maximum, 0, -maximum),
    ]
    rng = random.Random(20260713)
    for _ in range(1500):
        n = rng.randrange(0, 1000)
        m = rng.randrange(1, 10**9 + 1)
        a = rng.randrange(-10**12, 10**12 + 1)
        b = rng.randrange(-10**12, 10**12 + 1)
        queries.append((n, m, a, b))

    results = [floor_sum(*query) for query in queries]
    for result in results:
        if not minimum <= result <= maximum:
            raise RuntimeError("generated result does not fit in long long")

    (out_dir / "case_00.in").write_text(
        str(len(queries)) + "\n" + "\n".join(" ".join(map(str, query)) for query in queries) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(map(str, results)) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
