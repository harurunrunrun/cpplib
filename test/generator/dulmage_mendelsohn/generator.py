#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def matching(
    left_size: int, right_size: int, edges: list[tuple[int, int]]
) -> tuple[list[int], list[int]]:
    graph = [[] for _ in range(left_size)]
    for left, right in edges:
        graph[left].append(right)
    left_match = [-1] * left_size
    right_match = [-1] * right_size

    def augment(left: int, seen: list[bool]) -> bool:
        for right in graph[left]:
            if seen[right]:
                continue
            seen[right] = True
            if right_match[right] == -1 or augment(right_match[right], seen):
                left_match[left] = right
                right_match[right] = left
                return True
        return False

    for left in range(left_size):
        augment(left, [False] * right_size)
    return left_match, right_match


def solve(
    left_size: int, right_size: int, edges: list[tuple[int, int]]
) -> tuple[int, int, int, int]:
    left_match, right_match = matching(left_size, right_size, edges)
    vertex_count = left_size + right_size
    graph = [[] for _ in range(vertex_count)]
    reverse = [[] for _ in range(vertex_count)]
    for left, right in edges:
        right_vertex = left_size + right
        arc = (
            (right_vertex, left)
            if left_match[left] == right
            else (left, right_vertex)
        )
        graph[arc[0]].append(arc[1])
        reverse[arc[1]].append(arc[0])

    def reachable(adjacency: list[list[int]], sources: list[int]) -> list[bool]:
        seen = [False] * vertex_count
        queue = collections.deque(sources)
        for source in sources:
            seen[source] = True
        while queue:
            vertex = queue.popleft()
            for to in adjacency[vertex]:
                if not seen[to]:
                    seen[to] = True
                    queue.append(to)
        return seen

    from_left = reachable(
        graph, [left for left in range(left_size) if left_match[left] == -1]
    )
    to_right = reachable(
        reverse,
        [
            left_size + right
            for right in range(right_size)
            if right_match[right] == -1
        ],
    )
    counts = [0, 0, 0]
    for vertex in range(vertex_count):
        if from_left[vertex]:
            counts[0] += 1
        elif to_right[vertex]:
            counts[2] += 1
        else:
            counts[1] += 1
    return sum(value != -1 for value in left_match), *counts


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, int, list[tuple[int, int]]]] = [
        (0, 0, []),
        (2, 1, [(0, 0), (1, 0)]),
        (1, 2, [(0, 0), (0, 1)]),
        (3, 3, [(0, 0), (1, 1), (2, 2), (0, 1)]),
    ]
    for _ in range(250):
        left_size = rng.randint(0, 10)
        right_size = rng.randint(0, 10)
        edges = [
            (left, right)
            for left in range(left_size)
            for right in range(right_size)
            if rng.random() < 0.28
        ]
        cases.append((left_size, right_size, edges))
    input_lines = [str(len(cases))]
    output_lines = []
    for left_size, right_size, edges in cases:
        input_lines.append(f"{left_size} {right_size} {len(edges)}")
        input_lines.extend(f"{left} {right}" for left, right in edges)
        output_lines.append(" ".join(map(str, solve(
            left_size, right_size, edges
        ))))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
