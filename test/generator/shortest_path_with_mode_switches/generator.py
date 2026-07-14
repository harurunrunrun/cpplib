#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def brute(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    switch_vertices: list[int],
    source: int,
    target: int,
    initial_mode: int,
) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(2 * vertex_count)]
    for from_vertex, to_vertex, mode in edges:
        left = from_vertex + mode * vertex_count
        right = to_vertex + mode * vertex_count
        graph[left].append((right, 1))
        graph[right].append((left, 1))
    for vertex in switch_vertices:
        graph[vertex].append((vertex + vertex_count, 0))
        graph[vertex + vertex_count].append((vertex, 0))

    infinity = 10**9
    distance = [infinity] * (2 * vertex_count)
    start = source + initial_mode * vertex_count
    distance[start] = 0
    queue = [(0, start)]
    while queue:
        current_distance, vertex = heapq.heappop(queue)
        if current_distance != distance[vertex]:
            continue
        for next_vertex, cost in graph[vertex]:
            candidate = current_distance + cost
            if candidate < distance[next_vertex]:
                distance[next_vertex] = candidate
                heapq.heappush(queue, (candidate, next_vertex))
    answer = min(distance[target], distance[target + vertex_count])
    return -1 if answer == infinity else answer


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    switch_vertices: list[int],
    source: int,
    target: int,
    initial_mode: int,
) -> None:
    lines = [
        f"{vertex_count} {len(edges)} {len(switch_vertices)} "
        f"{source} {target} {initial_mode}"
    ]
    lines.extend(f"{u} {v} {mode}" for u, v, mode in edges)
    lines.append(" ".join(map(str, switch_vertices)))
    answer = brute(
        vertex_count,
        edges,
        switch_vertices,
        source,
        target,
        initial_mode,
    )
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (1, [], [], 0, 0, 0),
        (2, [(0, 1, 1)], [], 0, 1, 1),
        (2, [(0, 1, 0)], [], 0, 1, 1),
        (2, [(0, 1, 0)], [0], 0, 1, 1),
        (3, [(0, 1, 1), (1, 2, 0)], [1], 0, 2, 1),
    ]
    rng = random.Random(277_277_05)
    for _ in range(95):
        vertex_count = rng.randint(1, 10)
        candidates = [
            (u, v, mode)
            for u in range(vertex_count)
            for v in range(u + 1, vertex_count)
            for mode in range(2)
        ]
        rng.shuffle(candidates)
        edges = candidates[: rng.randint(0, min(len(candidates), 3 * vertex_count))]
        switch_vertices = [v for v in range(vertex_count) if rng.randrange(3) == 0]
        source = rng.randrange(vertex_count)
        target = rng.randrange(vertex_count)
        initial_mode = rng.randrange(2)
        cases.append(
            (
                vertex_count,
                edges,
                switch_vertices,
                source,
                target,
                initial_mode,
            )
        )

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
