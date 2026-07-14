#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


Case = tuple[int, list[tuple[int, int, int]], int, int, int | None]


def solve_independent(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
) -> int | None:
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
    return distance[target]


def write_suite(out_dir: Path, name: str, cases: list[Case], *, final_newline: bool = True) -> None:
    assert cases
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, source, target, answer in cases:
        assert 2 <= vertex_count <= 100_000
        assert 1 <= len(edges) <= 100_000
        input_lines.append(f"{vertex_count} {len(edges)} {source + 1} {target + 1}")
        input_lines.extend(f"{left + 1} {right + 1} {cost}" for left, right, cost in edges)
        if answer is None:
            answer = solve_independent(vertex_count, edges, source, target)
        output_lines.append("NONE" if answer is None else str(answer))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(
        out_dir,
        "official",
        [
            (4, [(0, 1, 5), (2, 3, 5)], 0, 3, None),
            (4, [(0, 1, 5), (1, 2, 5), (2, 3, 5), (3, 1, 6)], 0, 3, 11),
        ],
    )

    rng = random.Random(100000103)
    random_cases: list[Case] = []
    for _ in range(250):
        vertex_count = rng.randint(2, 35)
        possible = [(left, right) for left in range(vertex_count) for right in range(left + 1, vertex_count)]
        rng.shuffle(possible)
        edge_count = rng.randint(1, min(120, len(possible)))
        edges = [(left, right, rng.randint(1, 1000)) for left, right in possible[:edge_count]]
        source = rng.randrange(vertex_count)
        target = rng.randrange(vertex_count)
        random_cases.append((vertex_count, edges, source, target, None))
    write_suite(out_dir, "random_independent", random_cases)

    vertex_count = 100_000
    chain = [(vertex, vertex + 1, 1000) for vertex in range(vertex_count - 1)]
    chain.append((0, vertex_count - 1, 999))
    write_suite(
        out_dir,
        "maximum_connected",
        [(vertex_count, chain, 0, vertex_count - 1, 999)],
    )

    disconnected = [(vertex, vertex + 1, 1) for vertex in range(vertex_count - 2)]
    disconnected.extend([(0, 2, 1), (1, 3, 1)])
    write_suite(
        out_dir,
        "maximum_disconnected",
        [(vertex_count, disconnected, 0, vertex_count - 1, None)],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
