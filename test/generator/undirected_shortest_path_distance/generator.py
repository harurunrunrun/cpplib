#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> None:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for left, right, cost in edges:
        graph[left].append((right, cost))
        graph[right].append((left, cost))
    distance: list[int | None] = [None] * vertex_count
    distance[source] = 0
    queue = [(0, source)]
    while queue:
        current, vertex = heapq.heappop(queue)
        if distance[vertex] != current:
            continue
        for neighbor, cost in graph[vertex]:
            candidate = current + cost
            if distance[neighbor] is None or candidate < distance[neighbor]:
                distance[neighbor] = candidate
                heapq.heappush(queue, (candidate, neighbor))

    name = f"case_{index:03d}"
    input_lines = [
        f"{vertex_count} {len(edges)} {source} {target}",
        *(f"{left} {right} {cost}" for left, right, cost in edges),
    ]
    answer = distance[target]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "unreachable\n" if answer is None else f"{answer}\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [], 0, 0)
    write_case(out_dir, 1, 4, [(0, 1, 5), (1, 2, 6)], 0, 3)
    write_case(out_dir, 2, 3, [(0, 1, 5), (1, 2, 6), (0, 2, 20)], 0, 2)

    rng = random.Random(819201)
    for index in range(3, 63):
        vertex_count = rng.randint(1, 50)
        edges = [
            (left, right, rng.randint(0, 1000))
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
            if rng.randrange(8) == 0
        ]
        for _ in range(vertex_count // 3):
            edges.append((
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randint(0, 1000),
            ))
        write_case(
            out_dir,
            index,
            vertex_count,
            edges,
            rng.randrange(vertex_count),
            rng.randrange(vertex_count),
        )


if __name__ == "__main__":
    main()
