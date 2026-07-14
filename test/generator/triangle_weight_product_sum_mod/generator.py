#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MODULUS = 998244353


def brute_answer(
    vertex_count: int,
    edges: list[tuple[int, int]],
    weights: list[int],
) -> int:
    adjacent = [[False] * vertex_count for _ in range(vertex_count)]
    for left, right in edges:
        adjacent[left][right] = True
        adjacent[right][left] = True
    answer = 0
    for first in range(vertex_count):
        for second in range(first + 1, vertex_count):
            if not adjacent[first][second]:
                continue
            for third in range(second + 1, vertex_count):
                if adjacent[first][third] and adjacent[second][third]:
                    answer += weights[first] * weights[second] * weights[third]
    return answer % MODULUS


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int]],
    weights: list[int],
) -> None:
    name = f"case_{index:03d}"
    lines = [f"{vertex_count} {len(edges)}", " ".join(map(str, weights))]
    lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        f"{brute_answer(vertex_count, edges, weights)}\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [], [0])
    write_case(out_dir, 1, 3, [(0, 1), (1, 2), (2, 0)], [2, 3, 5])
    write_case(
        out_dir,
        2,
        4,
        [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)],
        [MODULUS - 1, MODULUS - 2, MODULUS - 3, MODULUS - 4],
    )

    rng = random.Random(620117)
    for index in range(3, 65):
        vertex_count = rng.randint(1, 18)
        weights = [rng.randrange(MODULUS) for _ in range(vertex_count)]
        edges = [
            (left, right)
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
            if rng.randrange(4) == 0
        ]
        write_case(out_dir, index, vertex_count, edges, weights)


if __name__ == "__main__":
    main()
