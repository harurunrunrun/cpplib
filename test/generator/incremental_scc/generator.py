#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def answers(
    n: int, weights: list[int], edges: list[tuple[int, int]]
) -> list[int]:
    result: list[int] = []
    reach = [[False] * n for _ in range(n)]
    for vertex in range(n):
        reach[vertex][vertex] = True
    for from_vertex, to_vertex in edges:
        reach[from_vertex][to_vertex] = True
        for middle in range(n):
            for left in range(n):
                if not reach[left][middle]:
                    continue
                for right in range(n):
                    reach[left][right] |= reach[middle][right]
        value = 0
        for left in range(n):
            for right in range(left + 1, n):
                if reach[left][right] and reach[right][left]:
                    value += weights[left] * weights[right]
        result.append(value % MOD)
    return result


def write_case(
    out_dir: Path,
    index: int,
    weights: list[int],
    edges: list[tuple[int, int]],
    known: list[int] | None = None,
) -> None:
    input_lines = [
        f"{len(weights)} {len(edges)}",
        " ".join(map(str, weights)),
    ]
    input_lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    expected = known if known is not None else answers(
        len(weights), weights, edges
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, expected)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [7], [(0, 0), (0, 0)])
    write_case(
        out_dir,
        1,
        [1, 2, 3, 4],
        [(0, 1), (1, 2), (2, 0), (2, 3), (3, 2)],
    )
    rng = random.Random(2026071711)
    for index in range(2, 10):
        n = rng.randrange(2, 9)
        weights = [rng.randrange(MOD) for _ in range(n)]
        edges = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(rng.randrange(1, 22))
        ]
        write_case(out_dir, index, weights, edges)

    n = 500000
    edges = [(vertex, vertex + 1) for vertex in range(n - 1)]
    edges.append((n - 1, 0))
    final_answer = n * (n - 1) // 2 % MOD
    write_case(
        out_dir,
        10,
        [1] * n,
        edges,
        [0] * (n - 1) + [final_answer],
    )


if __name__ == "__main__":
    main()
