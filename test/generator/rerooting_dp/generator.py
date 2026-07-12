#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def distances(graph: list[list[int]], root: int) -> list[int]:
    result = [-1] * len(graph)
    result[root] = 0
    queue = deque([root])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if result[to] == -1:
                result[to] = result[vertex] + 1
                queue.append(to)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (1, []),
        (2, [(0, 1)]),
        (6, [(vertex - 1, vertex) for vertex in range(1, 6)]),
        (7, [(0, vertex) for vertex in range(1, 7)]),
        (7, [(0, 1), (0, 2), (1, 3), (1, 4), (2, 5), (5, 6)]),
    ]
    for _ in range(100):
        n = rng.randrange(1, 101)
        edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, n)]
        rng.shuffle(edges)
        cases.append((n, edges))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, edges in cases:
        input_lines.append(str(n))
        input_lines.extend(f"{u} {v}" for u, v in edges)
        graph = [[] for _ in range(n)]
        for u, v in edges:
            graph[u].append(v)
            graph[v].append(u)
        all_distances = [distances(graph, root) for root in range(n)]
        output_lines.append(" ".join(map(str, [n, *(max(row) for row in all_distances)])))
        output_lines.append(" ".join(map(str, [n, *(sum(row) for row in all_distances)])))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
