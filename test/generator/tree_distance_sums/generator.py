#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


Edge = tuple[int, int]


def solve_brute(n: int, edges: list[Edge]) -> list[int]:
    graph = [[] for _ in range(n)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    answer = []
    for source in range(n):
        distance = [-1] * n
        distance[source] = 0
        queue = deque([source])
        while queue:
            vertex = queue.popleft()
            for to in graph[vertex]:
                if distance[to] != -1:
                    continue
                distance[to] = distance[vertex] + 1
                queue.append(to)
        answer.append(sum(distance))
    return answer


def write_case(out_dir: Path, case_index: int, n: int, edges: list[Edge]) -> None:
    name = f"case_{case_index:03d}"
    lines = [f"{n} {len(edges)}", *(f"{u} {v}" for u, v in edges)]
    (out_dir / f"{name}.in").write_text(
        "\n".join(lines) + "\n", encoding="ascii"
    )
    answer = solve_brute(n, edges)
    output = "".join(f"{value}\n" for value in answer)
    (out_dir / f"{name}.out").write_text(output, encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[Edge]]] = [
        (0, []),
        (1, []),
        (2, [(0, 1)]),
        (3, [(0, 1), (1, 2)]),
        (6, [(0, 1), (0, 2), (0, 3), (0, 4), (0, 5)]),
    ]
    source = random.Random(220220)
    for _ in range(120):
        n = source.randint(1, 45)
        edges = [(vertex, source.randrange(vertex)) for vertex in range(1, n)]
        source.shuffle(edges)
        cases.append((n, edges))

    for case_index, case in enumerate(cases):
        write_case(out_dir, case_index, *case)


if __name__ == "__main__":
    main()
