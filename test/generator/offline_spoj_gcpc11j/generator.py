#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def solve(vertex_count: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)

    def eccentricity(start: int) -> int:
        distance = [-1] * vertex_count
        distance[start] = 0
        queue = deque([start])
        while queue:
            vertex = queue.popleft()
            for next_vertex in graph[vertex]:
                if distance[next_vertex] == -1:
                    distance[next_vertex] = distance[vertex] + 1
                    queue.append(next_vertex)
        return max(distance)

    return min(eccentricity(vertex) for vertex in range(vertex_count))


def write_case(out_dir: Path, name: str, tests: list[tuple[int, list[tuple[int, int]]]], answers: list[int] | None = None) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for index, (vertex_count, edges) in enumerate(tests):
        input_lines.append(str(vertex_count))
        input_lines.extend(f"{left} {right}" for left, right in edges)
        output_lines.append(str(solve(vertex_count, edges) if answers is None else answers[index]))
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "basic", [
        (2, [(1, 0)]),
        (5, [(3, 2), (2, 1), (0, 2), (2, 4)]),
        (6, [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5)]),
    ])

    source = random.Random(141)
    tests = []
    for _ in range(50):
        vertex_count = source.randint(2, 100)
        edges = [(vertex, source.randrange(vertex)) for vertex in range(1, vertex_count)]
        source.shuffle(edges)
        tests.append((vertex_count, edges))
    write_case(out_dir, "random", tests)

    maximum_edges = [(vertex - 1, vertex) for vertex in range(1, 100000)]
    write_case(out_dir, "maximum", [(100000, maximum_edges)], [50000])


if __name__ == "__main__":
    main()
