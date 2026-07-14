#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Edge = tuple[int, int, int]


def solve(vertex_count: int, edges: list[Edge]) -> int:
    parent = list(range(vertex_count))
    size = [1] * vertex_count

    def leader(vertex: int) -> int:
        while parent[vertex] != vertex:
            parent[vertex] = parent[parent[vertex]]
            vertex = parent[vertex]
        return vertex

    answer = 0
    used = 0
    for left, right, weight in sorted(edges, key=lambda edge: edge[2]):
        left = leader(left)
        right = leader(right)
        if left == right:
            continue
        if size[left] < size[right]:
            left, right = right, left
        parent[right] = left
        size[left] += size[right]
        answer += weight
        used += 1
    assert used == vertex_count - 1
    return answer


def write_case(out_dir: Path, name: str, vertex_count: int, edges: list[Edge], answer: int | None = None) -> None:
    input_lines = [f"{vertex_count} {len(edges)}"]
    input_lines.extend(f"{left + 1} {right + 1} {weight}" for left, right, weight in edges)
    if answer is None:
        answer = solve(vertex_count, edges)
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def random_graph(source: random.Random, vertex_count: int, edge_count: int) -> list[Edge]:
    pairs = {(vertex, source.randrange(vertex)) for vertex in range(1, vertex_count)}
    while len(pairs) < edge_count:
        left = source.randrange(vertex_count)
        right = source.randrange(vertex_count)
        if left != right:
            pairs.add(tuple(sorted((left, right))))
    order = list(pairs)
    source.shuffle(order)
    weights = list(range(1, len(order) + 1))
    source.shuffle(weights)
    return [(left, right, weights[index]) for index, (left, right) in enumerate(order)]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official", 4, [
        (0, 1, 10), (1, 2, 15), (0, 2, 5), (3, 1, 2), (3, 2, 40),
    ], 17)
    write_case(out_dir, "single", 1, [])

    source = random.Random(147)
    for case_index in range(35):
        vertex_count = source.randint(2, 90)
        maximum_edges = vertex_count * (vertex_count - 1) // 2
        edge_count = source.randint(vertex_count - 1, min(maximum_edges, 350))
        write_case(
            out_dir,
            f"random_{case_index:02d}",
            vertex_count,
            random_graph(source, vertex_count, edge_count),
        )

    vertex_count = 10000
    edges: list[Edge] = [
        (vertex - 1, vertex, vertex) for vertex in range(1, vertex_count)
    ]
    used = {tuple(sorted((left, right))) for left, right, _ in edges}
    while len(edges) < 100000:
        left = source.randrange(vertex_count)
        right = source.randrange(vertex_count)
        pair = tuple(sorted((left, right)))
        if left == right or pair in used:
            continue
        used.add(pair)
        edges.append((pair[0], pair[1], len(edges) + 1))
    write_case(out_dir, "maximum", vertex_count, edges, 49995000)


if __name__ == "__main__":
    main()
