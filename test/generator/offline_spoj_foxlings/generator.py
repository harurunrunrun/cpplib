#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(vertex_count: int, edges: list[tuple[int, int]]) -> int:
    parent: dict[int, int] = {}
    size: dict[int, int] = {}

    def leader(vertex: int) -> int:
        parent.setdefault(vertex, vertex)
        size.setdefault(vertex, 1)
        root = vertex
        while parent[root] != root:
            root = parent[root]
        while parent[vertex] != vertex:
            next_vertex = parent[vertex]
            parent[vertex] = root
            vertex = next_vertex
        return root

    answer = vertex_count
    for left, right in edges:
        left = leader(left)
        right = leader(right)
        if left == right:
            continue
        if size[left] < size[right]:
            left, right = right, left
        parent[right] = left
        size[left] += size[right]
        answer -= 1
    return answer


def write_case(out_dir: Path, name: str, tests: list[tuple[int, list[tuple[int, int]]]]) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for vertex_count, edges in tests:
        input_lines.append(f"{vertex_count} {len(edges)}")
        input_lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
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
        (4, []),
        (3, [(0, 0), (0, 1), (1, 0)]),
    ])

    source = random.Random(136)
    random_tests = []
    for _ in range(50):
        vertex_count = source.randint(1, 80)
        edge_count = source.randint(0, 240)
        edges = [
            (source.randrange(vertex_count), source.randrange(vertex_count))
            for _ in range(edge_count)
        ]
        random_tests.append((vertex_count, edges))
    write_case(out_dir, "random", random_tests)

    maximum_edges = [(index, index + 1) for index in range(100000)]
    write_case(out_dir, "maximum", [(10**9, maximum_edges)])


if __name__ == "__main__":
    main()
