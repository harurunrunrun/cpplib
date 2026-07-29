#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest(
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, cost in edges:
        graph[u].append((v, cost))
    infinity = 10**40
    distance = [infinity] * n
    distance[source] = 0
    queue = [(0, source)]
    while queue:
        current, vertex = heapq.heappop(queue)
        if current != distance[vertex]:
            continue
        for to, cost in graph[vertex]:
            candidate = current + cost
            if candidate < distance[to]:
                distance[to] = candidate
                heapq.heappush(queue, (candidate, to))
    return -1 if distance[target] == infinity else distance[target]


def write_case(
    output: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{n} {len(edges)} {source} {target}"]
    lines.extend(f"{u} {v} {cost}" for u, v, cost in edges)
    (output / f"{stem}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    answer = shortest(n, edges, source, target)
    (output / f"{stem}.out").write_text(
        f"{answer} {answer} {answer} {answer}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], int, int]] = [
        (1, [], 0, 0),
        (4, [(0, 1, 2), (1, 2, 3)], 0, 3),
        (
            6,
            [
                (0, 1, 50),
                (0, 2, 2),
                (2, 1, 3),
                (1, 3, 4),
                (2, 4, 20),
                (3, 4, 1),
                (4, 5, 8),
                (1, 5, 100),
            ],
            0,
            5,
        ),
    ]
    engine = random.Random(20260729)
    for n, denominator in ((8, 3), (25, 6), (60, 12), (120, 24)):
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and engine.randrange(denominator) == 0:
                    edges.append((u, v, engine.randrange(0, 100_001)))
        cases.append((n, edges, engine.randrange(n), engine.randrange(n)))

    for index, case in enumerate(cases):
        write_case(output, index, *case)


if __name__ == "__main__":
    main()
