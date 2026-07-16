#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


Edge = tuple[int, int, int]


def solve_naive(
    vertex_count: int,
    edges: list[Edge],
    sources: list[int],
) -> tuple[bool, list[int | None], list[bool]]:
    distance: list[int | None] = [None] * vertex_count
    for source in sources:
        distance[source] = 0
    for _ in range(max(0, vertex_count - 1)):
        changed = False
        for source, target, cost in edges:
            if distance[source] is None:
                continue
            candidate = distance[source] + cost
            if distance[target] is None or candidate < distance[target]:
                distance[target] = candidate
                changed = True
        if not changed:
            break

    graph: list[list[int]] = [[] for _ in range(vertex_count)]
    affected = [False] * vertex_count
    queue: collections.deque[int] = collections.deque()
    for source, target, cost in edges:
        graph[source].append(target)
        if distance[source] is None:
            continue
        candidate = distance[source] + cost
        if distance[target] is None or candidate < distance[target]:
            if not affected[target]:
                affected[target] = True
                queue.append(target)
    while queue:
        vertex = queue.popleft()
        for target in graph[vertex]:
            if not affected[target]:
                affected[target] = True
                queue.append(target)
    return any(affected), distance, affected


def encode_case(
    vertex_count: int,
    edges: list[Edge],
    sources: list[int],
) -> list[str]:
    lines = [f"{vertex_count} {len(edges)} {len(sources)}"]
    lines.append(" ".join(map(str, sources)))
    lines.extend(f"{source} {target} {cost}" for source, target, cost in edges)
    return lines


def answer_line(
    vertex_count: int,
    answer: tuple[bool, list[int | None], list[bool]],
) -> str:
    has_cycle, distance, affected = answer
    tokens = [str(int(has_cycle))]
    for vertex in range(vertex_count):
        if affected[vertex]:
            tokens.append("N")
        elif distance[vertex] is None:
            tokens.append("U")
        else:
            tokens.append(str(distance[vertex]))
    return " ".join(tokens)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[Edge], list[int]]] = [
        (0, [], []),
        (1, [], []),
        (1, [], [0]),
        (1, [(0, 0, -1)], [0]),
        (3, [(0, 1, 4_000_000_000_000_000_000)], [0]),
        (3, [(0, 1, -4_000_000_000_000_000_000), (1, 2, 3)], [0]),
        (4, [(2, 3, -1), (3, 2, -1)], [0]),
        (4, [(0, 2, 5), (1, 2, -1), (2, 3, 2)], [0, 1, 1]),
    ]
    rng = random.Random(20260716)
    for _ in range(600):
        vertex_count = rng.randrange(1, 9)
        edge_count = rng.randrange(22)
        edges = [
            (
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randrange(-12, 17),
            )
            for _ in range(edge_count)
        ]
        source_count = rng.randrange(vertex_count + 2)
        sources = [
            rng.randrange(vertex_count)
            for _ in range(source_count)
        ]
        cases.append((vertex_count, edges, sources))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, sources in cases:
        input_lines.extend(encode_case(vertex_count, edges, sources))
        output_lines.append(answer_line(
            vertex_count,
            solve_naive(vertex_count, edges, sources),
        ))
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )

    vertex_count = 100_000
    edges: list[Edge] = []
    distances = [0] * vertex_count
    for vertex in range(vertex_count - 1):
        cost = vertex % 7 - 3
        edges.append((vertex, vertex + 1, cost))
        distances[vertex + 1] = distances[vertex] + cost
    large_input = ["1", *encode_case(vertex_count, edges, [0])]
    large_output = "0 " + " ".join(map(str, distances))
    (out_dir / "case_01.in").write_text(
        "\n".join(large_input) + "\n", encoding="utf-8"
    )
    (out_dir / "case_01.out").write_text(
        large_output + "\n", encoding="utf-8"
    )

    edges.append((vertex_count - 1, vertex_count - 1, -1))
    cycle_input = ["1", *encode_case(vertex_count, edges, [0])]
    cycle_tokens = ["0"]
    cycle_tokens.extend(map(str, distances[:-1]))
    cycle_tokens.append("N")
    cycle_output = "1 " + " ".join(cycle_tokens[1:])
    (out_dir / "case_02.in").write_text(
        "\n".join(cycle_input) + "\n", encoding="utf-8"
    )
    (out_dir / "case_02.out").write_text(
        cycle_output + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
