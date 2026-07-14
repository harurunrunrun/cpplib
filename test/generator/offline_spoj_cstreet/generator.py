#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


Case = tuple[int, int, list[tuple[int, int, int]], int | None]


def mst_cost_independent(vertex_count: int, edges: list[tuple[int, int, int]]) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for left, right, cost in edges:
        graph[left].append((cost, right))
        graph[right].append((cost, left))
    visited = [False] * vertex_count
    queue = [(0, 0)]
    result = 0
    count = 0
    while queue:
        cost, vertex = heapq.heappop(queue)
        if visited[vertex]:
            continue
        visited[vertex] = True
        result += cost
        count += 1
        for edge in graph[vertex]:
            if not visited[edge[1]]:
                heapq.heappush(queue, edge)
    assert count == vertex_count
    return result


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[Case],
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= 100
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for price, vertex_count, edges, answer in cases:
        assert 1 <= vertex_count <= 1000
        assert 1 <= len(edges) <= 300_000
        input_lines.extend([str(price), str(vertex_count), str(len(edges))])
        input_lines.extend(f"{left + 1} {right + 1} {cost}" for left, right, cost in edges)
        if answer is None:
            answer = price * mst_cost_independent(vertex_count, edges)
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
        [(2, 5, [(0, 1, 1), (1, 2, 2), (1, 3, 6), (4, 1, 1), (4, 0, 3), (3, 4, 2), (2, 3, 3)], 12)],
    )

    rng = random.Random(100000105)
    random_cases: list[Case] = []
    for _ in range(240):
        vertex_count = rng.randint(2, 35)
        edges: list[tuple[int, int, int]] = []
        used: set[tuple[int, int]] = set()
        for vertex in range(1, vertex_count):
            parent = rng.randrange(vertex)
            edges.append((parent, vertex, rng.randint(1, 1000)))
            used.add((parent, vertex))
        possible = [
            (left, right)
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
            if (left, right) not in used
        ]
        rng.shuffle(possible)
        for left, right in possible[: rng.randint(0, min(80, len(possible)))]:
            edges.append((left, right, rng.randint(1, 1000)))
        random_cases.append((rng.randint(1, 1000), vertex_count, edges, None))
    for offset in range(0, len(random_cases), 100):
        write_suite(out_dir, f"random_{offset // 100:02d}", random_cases[offset:offset + 100])

    many_cases = [(case + 1, 2, [(0, 1, case + 1)], (case + 1) ** 2) for case in range(100)]
    write_suite(out_dir, "maximum_test_count", many_cases)

    vertex_count = 1000
    maximum_edges = [(vertex, vertex + 1, 1) for vertex in range(vertex_count - 1)]
    used = {(left, right) for left, right, _ in maximum_edges}
    for left in range(vertex_count):
        for right in range(left + 1, vertex_count):
            if len(maximum_edges) == 300_000:
                break
            if (left, right) not in used:
                maximum_edges.append((left, right, 2 + (left + right) % 999))
        if len(maximum_edges) == 300_000:
            break
    assert len(maximum_edges) == 300_000
    write_suite(
        out_dir,
        "maximum_vertices_and_edges",
        [(4, vertex_count, maximum_edges, 4 * (vertex_count - 1))],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
