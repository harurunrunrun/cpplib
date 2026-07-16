#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path

Edge = tuple[int, int]
Case = tuple[int, list[Edge], int, int]


def solve(case: Case) -> int:
    vertex_count, edges, source, sink = case
    capacity = [[0] * vertex_count for _ in range(vertex_count)]
    internal_capacity = len(edges) + 1
    for left, right in edges:
        if left == right:
            continue
        capacity[left][right] += (
            1 if left == source or right == sink
            else internal_capacity
        )

    answer = 0
    while True:
        parent = [-1] * vertex_count
        parent[source] = source
        queue = collections.deque([source])
        while queue and parent[sink] == -1:
            vertex = queue.popleft()
            for target, value in enumerate(capacity[vertex]):
                if value > 0 and parent[target] == -1:
                    parent[target] = vertex
                    queue.append(target)
        if parent[sink] == -1:
            return answer
        pushed = 10**30
        vertex = sink
        while vertex != source:
            pushed = min(pushed, capacity[parent[vertex]][vertex])
            vertex = parent[vertex]
        vertex = sink
        while vertex != source:
            previous = parent[vertex]
            capacity[previous][vertex] -= pushed
            capacity[vertex][previous] += pushed
            vertex = previous
        answer += pushed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (2, [(1, 0)], 1, 0),
        (4, [(2, 0), (0, 3), (2, 1), (1, 3)], 2, 3),
        (
            6,
            [(4, 0), (4, 1), (0, 2), (1, 2), (2, 3), (3, 5)],
            4,
            5,
        ),
    ]
    source = random.Random(31008)
    for _ in range(150):
        vertex_count = source.randint(2, 12)
        terminal_source, terminal_sink = source.sample(
            range(vertex_count), 2
        )
        edges = [
            (
                source.randrange(vertex_count),
                source.randrange(vertex_count),
            )
            for _ in range(source.randint(0, 60))
        ]
        cases.append(
            (vertex_count, edges, terminal_source, terminal_sink)
        )

    input_lines = [str(len(cases))]
    output_lines = []
    for vertex_count, edges, terminal_source, terminal_sink in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} "
            f"{terminal_source} {terminal_sink}"
        )
        input_lines.extend(f"{left} {right}" for left, right in edges)
        output_lines.append(str(solve(
            (vertex_count, edges, terminal_source, terminal_sink)
        )))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
