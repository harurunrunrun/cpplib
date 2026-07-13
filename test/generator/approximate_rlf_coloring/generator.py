#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]


def normalize(vertex_count: int, input_edges: list[Edge]) -> list[list[int]]:
    adjacency = [set() for _ in range(vertex_count)]
    for left, right in input_edges:
        assert 0 <= left < vertex_count and 0 <= right < vertex_count and left != right
        adjacency[left].add(right)
        adjacency[right].add(left)
    return [sorted(neighbors) for neighbors in adjacency]


def rlf(adjacency: list[list[int]]) -> tuple[list[int], int]:
    vertex_count = len(adjacency)
    colors = [vertex_count] * vertex_count
    uncolored = [True] * vertex_count
    remaining = vertex_count
    color = 0
    while remaining:
        seed = min(
            (vertex for vertex in range(vertex_count) if uncolored[vertex]),
            key=lambda vertex: (
                -sum(uncolored[neighbor] for neighbor in adjacency[vertex]), vertex
            ),
        )
        blocked = [False] * vertex_count
        colors[seed] = color
        uncolored[seed] = False
        remaining -= 1
        for neighbor in adjacency[seed]:
            if uncolored[neighbor]:
                blocked[neighbor] = True
        while True:
            candidates = [
                vertex for vertex in range(vertex_count)
                if uncolored[vertex] and not blocked[vertex]
            ]
            if not candidates:
                break
            selected = min(
                candidates,
                key=lambda vertex: (
                    -sum(uncolored[x] and blocked[x] for x in adjacency[vertex]),
                    -sum(uncolored[x] and not blocked[x] for x in adjacency[vertex]),
                    vertex,
                ),
            )
            colors[selected] = color
            uncolored[selected] = False
            remaining -= 1
            for neighbor in adjacency[selected]:
                if uncolored[neighbor]:
                    blocked[neighbor] = True
        color += 1
    return colors, color


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071317)
    cases: list[tuple[int, list[Edge]]] = [
        (0, []),
        (1, []),
        (5, []),
        (5, [(left, right) for left in range(5) for right in range(left + 1, 5)]),
        (6, [(vertex, (vertex + 1) % 6) for vertex in range(6)]),
        (5, [(vertex, (vertex + 1) % 5) for vertex in range(5)]),
        (4, [(0, 1), (1, 0), (0, 1), (1, 2), (2, 3)]),
    ]
    for _ in range(32):
        vertex_count = rng.randrange(1, 25)
        probability = rng.uniform(0.0, 0.55)
        edges: list[Edge] = []
        for left in range(vertex_count):
            for right in range(left + 1, vertex_count):
                if rng.random() < probability:
                    edges.append((left, right))
                    if rng.randrange(8) == 0:
                        edges.append((right, left))
        cases.append((vertex_count, edges))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for identifier, (vertex_count, edges) in enumerate(cases):
        input_lines.append(f"{identifier} {vertex_count} {len(edges)}")
        input_lines.extend(f"{left} {right}" for left, right in edges)
        colors, color_count = rlf(normalize(vertex_count, edges))
        output_lines.append(
            " ".join(["CASE", str(identifier), str(color_count), *map(str, colors)])
        )
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
