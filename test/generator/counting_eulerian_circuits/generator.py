#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MOD = 998244353


def brute(edges: list[tuple[int, int]]) -> int:
    if not edges:
        return 0
    start = edges[0][0]
    full = (1 << len(edges)) - 1

    def search(vertex: int, used: int) -> int:
        if used == full:
            return int(vertex == start)
        answer = 0
        for edge in range(1, len(edges)):
            if (used >> edge) & 1 or edges[edge][0] != vertex:
                continue
            answer += search(edges[edge][1], used | (1 << edge))
        return answer

    return search(edges[0][1], 1) % MOD


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    known: int | None = None,
) -> None:
    input_lines = [f"{n} {len(edges)}"]
    input_lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    answer = known if known is not None else brute(edges)
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer % MOD}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [(0, 0)])
    loops = [(0, 0)] * 10
    write_case(out_dir, 1, 1, loops, math.factorial(9))
    write_case(out_dir, 2, 4, [(0, 1), (1, 2), (2, 3), (3, 0)])
    write_case(out_dir, 3, 4, [(0, 1), (1, 0), (2, 3), (3, 2)], 0)
    write_case(out_dir, 4, 3, [(0, 1), (1, 2)], 0)

    rng = random.Random(2026071614)
    for index in range(5, 14):
        n = rng.randrange(1, 6)
        edges = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(rng.randrange(1, 10))
        ]
        write_case(out_dir, index, n, edges)

    n = 40
    copies = 20
    cycle = [
        (vertex, (vertex + 1) % n)
        for _ in range(copies)
        for vertex in range(n)
    ]
    expected = pow(copies, n - 1, MOD) * pow(
        math.factorial(copies - 1) % MOD, n, MOD
    ) % MOD
    write_case(out_dir, 14, n, cycle, expected)

    complete_n = 300
    complete = [
        (left, right)
        for left in range(complete_n)
        for right in range(complete_n)
        if left != right
    ]
    complete_expected = pow(complete_n, complete_n - 2, MOD) * pow(
        math.factorial(complete_n - 2) % MOD, complete_n, MOD
    ) % MOD
    write_case(out_dir, 15, complete_n, complete, complete_expected)


if __name__ == "__main__":
    main()
