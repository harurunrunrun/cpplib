#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(vertex_count: int, edges: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    seen = [False] * vertex_count
    answer = 0
    for start in range(vertex_count):
        if seen[start]:
            continue
        answer += 1
        seen[start] = True
        stack = [start]
        while stack:
            vertex = stack.pop()
            for next_vertex in graph[vertex]:
                if not seen[next_vertex]:
                    seen[next_vertex] = True
                    stack.append(next_vertex)
    return answer


def write_case(out_dir: Path, name: str, tests: list[tuple[int, list[tuple[int, int]]]]) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for vertex_count, edges in tests:
        input_lines.append(f"{vertex_count} {len(edges)}")
        input_lines.extend(f"{left} {right}" for left, right in edges)
        output_lines.append(str(solve(vertex_count, edges)))
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "basic", [
        (5, [(0, 1), (1, 2), (3, 4)]),
        (1, []),
        (4, [(0, 0), (0, 1), (0, 1)]),
    ])

    source = random.Random(137)
    random_tests = []
    for _ in range(50):
        vertex_count = source.randint(1, 100)
        edge_count = source.randint(0, 350)
        edges = [
            (source.randrange(vertex_count), source.randrange(vertex_count))
            for _ in range(edge_count)
        ]
        random_tests.append((vertex_count, edges))
    write_case(out_dir, "random", random_tests)

    write_case(out_dir, "maximum", [
        (100000, [(index, index + 1) for index in range(99999)])
    ])


if __name__ == "__main__":
    main()
