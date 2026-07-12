#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int, int]]) -> tuple[int, int]:
    exists = [[False] * n for _ in range(n)]
    cost = [[0] * n for _ in range(n)]
    for u, v, value in edges:
        if u == v:
            continue
        if not exists[u][v] or value < cost[u][v]:
            exists[u][v] = exists[v][u] = True
            cost[u][v] = cost[v][u] = value
    used = [False] * n
    best_size = 0
    best_cost = 0

    def dfs(size: int, total: int) -> None:
        nonlocal best_size, best_cost
        vertex = next((i for i in range(n) if not used[i]), -1)
        if vertex == -1:
            if best_size < size or (best_size == size and total < best_cost):
                best_size = size
                best_cost = total
            return
        used[vertex] = True
        dfs(size, total)
        for other in range(vertex + 1, n):
            if not used[other] and exists[vertex][other]:
                used[other] = True
                dfs(size + 1, total + cost[vertex][other])
                used[other] = False
        used[vertex] = False

    dfs(0, 0)
    return best_size, best_cost


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int, int]],
    expected: tuple[int, int] | None = None,
) -> None:
    name = f"case_{index:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v} {cost}" for u, v, cost in edges]]) + "\n",
        encoding="utf-8",
    )
    size, total = solve(n, edges) if expected is None else expected
    (out_dir / f"{name}.out").write_text(f"{size} {total}\n", encoding="utf-8")


def preferred_complete_graph(n: int) -> tuple[list[tuple[int, int, int]], tuple[int, int]]:
    preferred: dict[tuple[int, int], int] = {}
    for vertex in range(0, n - 1, 2):
        preferred[(vertex, vertex + 1)] = -4_000_000_000_000_000 + vertex
    edges = []
    for u in range(n):
        for v in range(u + 1, n):
            edges.append((u, v, preferred.get((u, v), 3_000_000_000_000_000 + u * n + v)))
    expected_cost = sum(preferred.values())
    return edges, (n // 2, expected_cost)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], tuple[int, int] | None]] = [
        (0, [], None),
        (5, [(0, 1, 5), (1, 2, 1), (2, 0, 1), (3, 4, -2)], None),
        (4, [(0, 1, 10), (1, 0, -5), (2, 3, 7), (1, 1, -100)], None),
        (2, [(0, 1, -(1 << 63))], None),
        (4, [(0, 1, -9_000_000_000_000_000_000), (2, 3, 9_000_000_000_000_000_000)], None),
    ]
    blossom_weight_cases = [
        [(1, 2, 9), (1, 3, 9), (2, 3, 10), (2, 4, 8), (3, 5, 8), (4, 5, 10), (5, 6, 6)],
        [(1, 2, 10), (1, 7, 10), (2, 3, 12), (3, 4, 20), (3, 5, 20), (4, 5, 25), (5, 6, 10), (6, 7, 10), (7, 8, 8)],
        [(1, 2, 8), (1, 3, 8), (2, 3, 10), (2, 4, 12), (3, 5, 12), (4, 5, 14), (4, 6, 12), (5, 7, 12), (6, 7, 14), (7, 8, 12)],
        [(1, 2, 23), (1, 5, 22), (1, 6, 15), (2, 3, 25), (3, 4, 22), (4, 5, 25), (4, 8, 14), (5, 7, 13)],
        [(1, 2, 19), (1, 3, 20), (1, 8, 8), (2, 3, 25), (2, 4, 18), (3, 5, 18), (4, 5, 13), (4, 7, 7), (5, 6, 7)],
        [(1, 2, 45), (1, 5, 45), (2, 3, 50), (3, 4, 45), (4, 5, 50), (1, 6, 30), (3, 9, 35), (4, 8, 35), (5, 7, 26), (9, 10, 5)],
        [(1, 2, 45), (1, 5, 45), (2, 3, 50), (3, 4, 45), (4, 5, 50), (1, 6, 30), (3, 9, 35), (4, 8, 26), (5, 7, 40), (9, 10, 5)],
    ]
    for weighted_edges in blossom_weight_cases:
        n = max(max(u, v) for u, v, _ in weighted_edges) + 1
        cases.append((n, [(u, v, -weight) for u, v, weight in weighted_edges], None))
    rng = random.Random(20260829)
    for _ in range(50):
        n = rng.randrange(13)
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(3) != 0:
                    edges.append((u, v, rng.randrange(-10**12, 10**12 + 1)))
                    if rng.randrange(5) == 0:
                        edges.append((v, u, rng.randrange(-10**12, 10**12 + 1)))
        if n and rng.randrange(2):
            vertex = rng.randrange(n)
            edges.append((vertex, vertex, -(1 << 63)))
        cases.append((n, edges, None))

    for n in (64, 121):
        edges, expected = preferred_complete_graph(n)
        cases.append((n, edges, expected))

    for index, (n, edges, expected) in enumerate(cases):
        write_case(out_dir, index, n, edges, expected)


if __name__ == "__main__":
    main()
