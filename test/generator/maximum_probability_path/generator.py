#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import math
import random
from pathlib import Path


Edge = tuple[int, int, int]


def solve(
    size: int,
    edges: list[Edge],
    source: int,
) -> list[tuple[int, int]]:
    graph: list[list[tuple[int, float]]] = [[] for _ in range(size)]
    for left, right, half_units in edges:
        graph[left].append((right, half_units / 2.0))
    best = [0.0] * size
    reachable = [0] * size
    best[source] = 1.0
    reachable[source] = 1
    queue = [(-1.0, source)]
    while queue:
        negative_probability, vertex = heapq.heappop(queue)
        probability = -negative_probability
        if probability != best[vertex]:
            continue
        for to, weight in graph[vertex]:
            candidate = probability * weight
            if not reachable[to] or best[to] < candidate:
                reachable[to] = 1
                best[to] = candidate
                heapq.heappush(queue, (-candidate, to))
    return [
        (
            reachable[vertex],
            math.floor(best[vertex] * 1_000_000 + 0.5),
        )
        for vertex in range(size)
    ]


def write_case(
    out_dir: Path,
    name: str,
    size: int,
    edges: list[Edge],
    source: int,
) -> None:
    answer = solve(size, edges, source)
    input_lines = [
        f"{size} {len(edges)} {source}",
        *(f"{left} {right} {weight}" for left, right, weight in edges),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "".join(f"{reachable} {scaled}\n" for reachable, scaled in answer),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "single", 1, [], 0)
    write_case(out_dir, "fixed", 5, [
        (0, 1, 1), (1, 2, 1), (0, 2, 1),
        (2, 3, 2), (1, 3, 0), (3, 1, 1),
    ], 0)
    source = random.Random(910238)
    for case_index in range(30):
        size = source.randint(1, 9)
        edges = [
            (left, right, source.randrange(3))
            for left in range(size)
            for right in range(size)
            if left != right and source.randrange(5) == 0
        ]
        start = source.randrange(size)
        write_case(out_dir, f"random_{case_index:02d}", size, edges, start)


if __name__ == "__main__":
    main()
