#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


Edge = tuple[int, int, int]
Case = tuple[int, list[Edge], int, int, int]


def component_oracle(vertex_count: int, input_edges: list[Edge]) -> list[int]:
    adjacency: list[list[int]] = [[] for _ in range(vertex_count)]
    normalized = set()
    for left, right, weight in input_edges:
        assert 0 <= left < vertex_count and 0 <= right < vertex_count and weight >= 0
        if left != right and weight != 0:
            normalized.add((min(left, right), max(left, right)))
    for left, right in normalized:
        adjacency[left].append(right)
        adjacency[right].append(left)
    labels = [-1] * vertex_count
    next_label = 0
    for start in range(vertex_count):
        if labels[start] != -1:
            continue
        labels[start] = next_label
        queue = deque([start])
        while queue:
            vertex = queue.popleft()
            for neighbor in adjacency[vertex]:
                if labels[neighbor] == -1:
                    labels[neighbor] = next_label
                    queue.append(neighbor)
        next_label += 1
    return labels


def clique_groups(sizes: list[int], rng: random.Random) -> tuple[int, list[Edge]]:
    edges: list[Edge] = []
    first = 0
    for size in sizes:
        weight = rng.randrange(2, 12)
        for left in range(first, first + size):
            for right in range(left + 1, first + size):
                edges.append((left, right, weight))
                if rng.randrange(5) == 0:
                    split = rng.randrange(weight + 1)
                    edges[-1] = (left, right, weight - split)
                    edges.append((right, left, split))
        if size:
            edges.append((first, first, 10**9))
        first += size
    return first, edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071309)

    cases: list[Case] = [
        (0, [], 0, 20, 100),
        (1, [], 1, 20, 100),
        (4, [], 2, 20, 100),
        (2, [(0, 1, 3)], 3, 20, 100),
        (4, [(0, 1, 5), (2, 3, 7)], 4, 20, 100),
        (5, [(0, 1, 2), (1, 0, 3), (0, 0, 999), (2, 3, 4), (3, 4, 4), (2, 4, 4)], 5, 20, 100),
    ]
    for _ in range(20):
        sizes: list[int] = []
        remaining = rng.randrange(1, 13)
        while remaining:
            size = rng.randrange(1, min(5, remaining) + 1)
            sizes.append(size)
            remaining -= size
        vertex_count, edges = clique_groups(sizes, rng)
        cases.append((vertex_count, edges, rng.getrandbits(64), 20, 100))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for identifier, (vertex_count, edges, seed, maximum_levels, maximum_passes) in enumerate(cases):
        input_lines.append(
            f"{identifier} {vertex_count} {len(edges)} {seed} {maximum_levels} {maximum_passes}"
        )
        input_lines.extend(f"{left} {right} {weight}" for left, right, weight in edges)
        labels = component_oracle(vertex_count, edges)
        output_lines.append(
            " ".join(["CASE", str(identifier), str(len(set(labels))), *map(str, labels)])
        )
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
