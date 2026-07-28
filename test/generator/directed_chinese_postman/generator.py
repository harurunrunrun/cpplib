#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import random
from pathlib import Path

INF = 10**30


def answer(
    n: int, edges: list[tuple[int, int, int]]
) -> int | None:
    if not edges:
        return 0
    incident = {
        vertex for u, v, _ in edges for vertex in (u, v)
    }
    graph = [[] for _ in range(n)]
    reverse = [[] for _ in range(n)]
    for u, v, _ in edges:
        graph[u].append(v)
        reverse[v].append(u)
    root = next(iter(incident))
    for current_graph in (graph, reverse):
        seen = {root}
        stack = [root]
        while stack:
            vertex = stack.pop()
            for to in current_graph[vertex]:
                if to not in seen:
                    seen.add(to)
                    stack.append(to)
        if not incident <= seen:
            return None
    distance = [[INF] * n for _ in range(n)]
    for vertex in range(n):
        distance[vertex][vertex] = 0
    balance = [0] * n
    base = 0
    for u, v, cost in edges:
        distance[u][v] = min(distance[u][v], cost)
        balance[u] += 1
        balance[v] -= 1
        base += cost
    for middle in range(n):
        for left in range(n):
            for right in range(n):
                distance[left][right] = min(
                    distance[left][right],
                    distance[left][middle] + distance[middle][right],
                )
    supplies = [
        vertex
        for vertex in range(n)
        for _ in range(max(0, -balance[vertex]))
    ]
    demands = [
        vertex
        for vertex in range(n)
        for _ in range(max(0, balance[vertex]))
    ]
    if not supplies:
        return base

    @functools.lru_cache(maxsize=None)
    def assign(index: int, mask: int) -> int:
        if index == len(supplies):
            return 0
        return min(
            distance[supplies[index]][demands[right]]
            + assign(index + 1, mask | (1 << right))
            for right in range(len(demands))
            if not (mask >> right & 1)
        )

    return base + assign(0, 0)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (0, []),
        (3, [(0, 1, 2), (1, 2, 3), (2, 0, 4)]),
        (3, [(0, 1, 1), (1, 0, 1), (0, 2, 3), (2, 0, 2), (0, 1, 4)]),
        (4, [(0, 1, 1), (1, 0, 1), (2, 3, 1), (3, 2, 1)]),
    ]
    while len(cases) < 185:
        n = rng.randint(1, 7)
        edges = [
            (u, v, rng.randint(0, 25))
            for u in range(n)
            for v in range(n)
            if rng.random() < 0.24
        ]
        balance = [0] * n
        for u, v, _ in edges:
            balance[u] += 1
            balance[v] -= 1
        if sum(max(0, value) for value in balance) <= 10:
            cases.append((n, edges))
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, edges in cases:
        input_lines.append(f"{n} {len(edges)}")
        input_lines.extend(f"{u} {v} {cost}" for u, v, cost in edges)
        result = answer(n, edges)
        output_lines.append(str(-1 if result is None else result))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
