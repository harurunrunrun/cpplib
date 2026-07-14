#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def solve(vertex_count: int, edges: list[tuple[int, int]]) -> bool:
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    color = [-1] * vertex_count
    for start in range(vertex_count):
        if color[start] != -1:
            continue
        color[start] = 0
        queue = deque([start])
        while queue:
            vertex = queue.popleft()
            for next_vertex in graph[vertex]:
                if color[next_vertex] == -1:
                    color[next_vertex] = color[vertex] ^ 1
                    queue.append(next_vertex)
                elif color[next_vertex] == color[vertex]:
                    return False
    return True


def write_case(out_dir: Path, name: str, tests: list[tuple[int, list[tuple[int, int]]]]) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for scenario, (vertex_count, edges) in enumerate(tests, 1):
        input_lines.append(f"{vertex_count} {len(edges)}")
        input_lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
        output_lines.append(f"Scenario #{scenario}:")
        output_lines.append(
            "No suspicious bugs found!" if solve(vertex_count, edges)
            else "Suspicious bugs found!"
        )
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "basic", [
        (3, [(0, 1), (1, 2)]),
        (3, [(0, 1), (1, 2), (2, 0)]),
        (1, [(0, 0)]),
        (4, []),
    ])

    source = random.Random(138)
    random_tests = []
    for _ in range(40):
        vertex_count = source.randint(1, 70)
        edge_count = source.randint(0, 260)
        edges = [
            (source.randrange(vertex_count), source.randrange(vertex_count))
            for _ in range(edge_count)
        ]
        random_tests.append((vertex_count, edges))
    write_case(out_dir, "random", random_tests)

    maximum_edges = [(0, 1)] * 1000000
    write_case(out_dir, "maximum", [(2000, maximum_edges)])


if __name__ == "__main__":
    main()
