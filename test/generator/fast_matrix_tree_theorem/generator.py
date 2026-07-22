#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def determinant(matrix: list[list[int]]) -> int:
    size = len(matrix)
    answer = 1
    for column in range(size):
        pivot = next(
            (row for row in range(column, size) if matrix[row][column]),
            None,
        )
        if pivot is None:
            return 0
        if pivot != column:
            matrix[pivot], matrix[column] = matrix[column], matrix[pivot]
            answer = -answer
        pivot_value = matrix[column][column]
        answer = answer * pivot_value % MOD
        inverse = pow(pivot_value, MOD - 2, MOD)
        for row in range(column + 1, size):
            factor = matrix[row][column] * inverse % MOD
            for entry in range(column + 1, size):
                matrix[row][entry] = (
                    matrix[row][entry] - factor * matrix[column][entry]
                ) % MOD
    return answer % MOD


def expected(
    mode: int,
    n: int,
    root: int,
    edges: list[tuple[int, int, int]],
) -> int:
    if n == 0:
        return 1
    size = n - 1
    matrix = [[0] * size for _ in range(size)]

    def index(vertex: int, removed: int) -> int:
        return vertex if vertex < removed else vertex - 1

    removed = 0 if mode == 0 else root
    for source, target, weight in edges:
        weight %= MOD
        if source == target:
            continue
        if mode == 0:
            if source != removed:
                u = index(source, removed)
                matrix[u][u] = (matrix[u][u] + weight) % MOD
            if target != removed:
                v = index(target, removed)
                matrix[v][v] = (matrix[v][v] + weight) % MOD
            if source != removed and target != removed:
                u = index(source, removed)
                v = index(target, removed)
                matrix[u][v] = (matrix[u][v] - weight) % MOD
                matrix[v][u] = (matrix[v][u] - weight) % MOD
        elif mode == 1 and target != removed:
            v = index(target, removed)
            matrix[v][v] = (matrix[v][v] + weight) % MOD
            if source != removed:
                u = index(source, removed)
                matrix[v][u] = (matrix[v][u] - weight) % MOD
        elif mode == 2 and source != removed:
            u = index(source, removed)
            matrix[u][u] = (matrix[u][u] + weight) % MOD
            if target != removed:
                v = index(target, removed)
                matrix[u][v] = (matrix[u][v] - weight) % MOD
    return determinant(matrix)


def write_case(
    out_dir: Path,
    index: int,
    mode: int,
    n: int,
    root: int,
    edges: list[tuple[int, int, int]],
    answer: int | None = None,
) -> None:
    if answer is None:
        answer = expected(mode, n, root, edges)
    lines = [f"{mode} {n} {root} {len(edges)}"]
    lines.extend(f"{u} {v} {w}" for u, v, w in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[
        tuple[int, int, int, list[tuple[int, int, int]], int | None]
    ] = [
        (0, 0, -1, [], 1),
        (0, 1, -1, [(0, 0, 99)], 1),
        (0, 2, -1, [(0, 1, 3), (0, 1, 5)], 8),
        (1, 3, 0, [(0, 1, 2), (0, 2, 3), (1, 2, 5)], None),
        (2, 3, 0, [(1, 0, 2), (2, 0, 3), (2, 1, 5)], None),
    ]
    source = random.Random(2026072247)
    for n in range(1, 10):
        for mode in range(3):
            for _ in range(4):
                root = source.randrange(n) if mode else -1
                edge_count = source.randrange(0, min(20, n * n + 5))
                edges = [
                    (
                        source.randrange(n),
                        source.randrange(n),
                        source.randrange(-7, 8),
                    )
                    for _ in range(edge_count)
                ]
                cases.append((mode, n, root, edges, None))

    n = 257
    complete = [
        (source_vertex, target_vertex, 1)
        for source_vertex in range(n)
        for target_vertex in range(source_vertex + 1, n)
    ]
    cases.append((0, n, -1, complete, pow(n, n - 2, MOD)))

    for index, (mode, n, root, edges, answer) in enumerate(cases):
        write_case(args.out_dir, index, mode, n, root, edges, answer)


if __name__ == "__main__":
    main()
