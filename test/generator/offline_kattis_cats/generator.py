#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path

Edge = tuple[int, int, int]
Case = tuple[int, int, list[Edge]]


def minimum_spanning_tree_cost(cat_count: int, edges: list[Edge]) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(cat_count)]
    for first, second, distance in edges:
        graph[first].append((distance, second))
        graph[second].append((distance, first))
    used = [False] * cat_count
    queue = [(0, 0)]
    total = 0
    selected = 0
    while queue:
        distance, vertex = heapq.heappop(queue)
        if used[vertex]:
            continue
        used[vertex] = True
        total += distance
        selected += 1
        for edge in graph[vertex]:
            if not used[edge[1]]:
                heapq.heappush(queue, edge)
    assert selected == cat_count
    return total


def complete_edges(source: random.Random, cat_count: int) -> list[Edge]:
    return [
        (first, second, source.randint(1, 3000))
        for first in range(cat_count)
        for second in range(first + 1, cat_count)
    ]


def answer(case: Case) -> str:
    milk, cat_count, edges = case
    return "yes" if minimum_spanning_tree_cost(cat_count, edges) + cat_count <= milk else "no"


def write_file(out_dir: Path, name: str, cases: list[Case], answers: list[str] | None = None) -> None:
    lines = [str(len(cases))]
    for milk, cat_count, edges in cases:
        lines.append(f"{milk} {cat_count}")
        lines.extend(f"{first} {second} {distance}" for first, second, distance in edges)
    if answers is None:
        answers = [answer(case) for case in cases]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(answers) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official_edges = [
        (0, 1, 4), (0, 2, 3), (0, 3, 10), (0, 4, 15),
        (1, 2, 7), (1, 3, 3), (1, 4, 5), (2, 3, 4),
        (2, 4, 3), (3, 4, 8),
    ]
    write_file(out_dir, "official_and_boundaries", [
        (20, 5, official_edges),
        (1, 1, []),
        (7, 2, [(0, 1, 5)]),
        (6, 2, [(0, 1, 5)]),
    ])

    source = random.Random(146)
    random_cases: list[Case] = []
    for _ in range(45):
        cat_count = source.randint(2, 9)
        edges = complete_edges(source, cat_count)
        threshold = minimum_spanning_tree_cost(cat_count, edges) + cat_count
        milk = max(1, threshold + source.randint(-2, 2))
        random_cases.append((milk, cat_count, edges))
    write_file(out_dir, "random", random_cases)

    cat_count = 700
    maximum_edges = [
        (
            first,
            second,
            1 if second == first + 1 else 3000,
        )
        for first in range(cat_count)
        for second in range(first + 1, cat_count)
    ]
    write_file(
        out_dir,
        "large_complete",
        [(2 * cat_count - 1, cat_count, maximum_edges)],
        ["yes"],
    )


if __name__ == "__main__":
    main()
