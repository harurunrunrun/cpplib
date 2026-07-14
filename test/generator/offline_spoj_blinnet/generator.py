#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


Case = tuple[int, list[tuple[int, int, int]], int | None]


def mst_cost_independent(vertex_count: int, edges: list[tuple[int, int, int]]) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for left, right, cost in edges:
        graph[left].append((cost, right))
        graph[right].append((cost, left))
    visited = [False] * vertex_count
    queue = [(0, 0)]
    result = 0
    visited_count = 0
    while queue:
        cost, vertex = heapq.heappop(queue)
        if visited[vertex]:
            continue
        visited[vertex] = True
        visited_count += 1
        result += cost
        for edge in graph[vertex]:
            if not visited[edge[1]]:
                heapq.heappush(queue, edge)
    assert visited_count == vertex_count
    return result


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[Case],
    *,
    blank_lines: bool = False,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= 10
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case_index, (vertex_count, edges, answer) in enumerate(cases):
        assert 1 <= vertex_count <= 10_000
        adjacency: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
        for left, right, cost in edges:
            adjacency[left].append((right, cost))
            adjacency[right].append((left, cost))
        if blank_lines:
            input_lines.append("")
        input_lines.append(str(vertex_count))
        for vertex, neighbors in enumerate(adjacency):
            input_lines.extend([f"city{vertex}", str(len(neighbors))])
            input_lines.extend(f"{neighbor + 1} {cost}" for neighbor, cost in neighbors)
        if answer is None:
            answer = mst_cost_independent(vertex_count, edges)
        output_lines.append(str(answer))
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
            (4, [(0, 1, 1), (0, 2, 3), (1, 2, 1), (1, 3, 4), (2, 3, 1)], 3),
            (3, [(0, 1, 1), (0, 2, 3), (1, 2, 7)], 4),
        ],
        blank_lines=True,
    )

    rng = random.Random(100000104)
    random_cases: list[Case] = []
    for _ in range(180):
        vertex_count = rng.randint(1, 35)
        edges: list[tuple[int, int, int]] = []
        for vertex in range(1, vertex_count):
            edges.append((vertex, rng.randrange(vertex), rng.randint(1, 10_000)))
        possible = [
            (left, right)
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
            if all((left, right) != (a, b) and (left, right) != (b, a) for a, b, _ in edges)
        ]
        rng.shuffle(possible)
        for left, right in possible[: rng.randint(0, min(50, len(possible)))]:
            edges.append((left, right, rng.randint(1, 10_000)))
        random_cases.append((vertex_count, edges, None))
    for offset in range(0, len(random_cases), 10):
        write_suite(out_dir, f"random_{offset // 10:02d}", random_cases[offset:offset + 10])

    vertex_count = 10_000
    chain = [(vertex, vertex + 1, 1) for vertex in range(vertex_count - 1)]
    write_suite(
        out_dir,
        "maximum_cities",
        [(vertex_count, chain, vertex_count - 1)],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
