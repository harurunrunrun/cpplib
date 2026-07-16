#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Edge = tuple[int, int, int]
Case = tuple[int, list[Edge], int]


def minimum_spanning_tree_cost(
    vertex_count: int,
    edges: list[Edge],
) -> int | None:
    parent = list(range(vertex_count))
    size = [1] * vertex_count

    def leader(vertex: int) -> int:
        while parent[vertex] != vertex:
            parent[vertex] = parent[parent[vertex]]
            vertex = parent[vertex]
        return vertex

    total = 0
    selected = 0
    for left, right, cost in sorted(edges, key=lambda edge: edge[2]):
        left = leader(left)
        right = leader(right)
        if left == right:
            continue
        if size[left] < size[right]:
            left, right = right, left
        parent[right] = left
        size[left] += size[right]
        total += cost
        selected += 1
    if selected != max(0, vertex_count - 1):
        return None
    return total


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (0, [], -1),
        (0, [], 0),
        (1, [], -5),
        (1, [], 0),
        (3, [(0, 1, -7), (1, 2, 3), (0, 2, 20)], -4),
        (3, [(0, 1, -7), (1, 2, 3), (0, 2, 20)], -5),
        (4, [(0, 1, 1), (2, 3, 1)], 100),
    ]
    source = random.Random(31001)
    for _ in range(120):
        vertex_count = source.randint(1, 12)
        possible = [
            (left, right)
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
        ]
        source.shuffle(possible)
        selected = possible[:source.randint(0, len(possible))]
        edges = [
            (left, right, source.randint(-30, 50))
            for left, right in selected
        ]
        cost = minimum_spanning_tree_cost(vertex_count, edges)
        budget = source.randint(-100, 150) if cost is None else (
            cost + source.randint(-3, 3)
        )
        cases.append((vertex_count, edges, budget))

    input_lines = [str(len(cases))]
    output_lines = []
    for vertex_count, edges, budget in cases:
        input_lines.append(f"{vertex_count} {len(edges)} {budget}")
        input_lines.extend(
            f"{left} {right} {cost}" for left, right, cost in edges
        )
        cost = minimum_spanning_tree_cost(vertex_count, edges)
        output_lines.append(
            "YES" if cost is not None and cost <= budget else "NO"
        )

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
