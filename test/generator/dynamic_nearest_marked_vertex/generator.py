#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[int, list[tuple[int, int, int]], bool, list[tuple[str, int]]]


def all_distances(n: int, edges: list[tuple[int, int, int]]) -> list[list[int]]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for left, right, weight in edges:
        graph[left].append((right, weight))
        graph[right].append((left, weight))
    result = [[0] * n for _ in range(n)]
    for source in range(n):
        stack = [(source, -1, 0)]
        while stack:
            vertex, parent, distance = stack.pop()
            result[source][vertex] = distance
            for to, weight in graph[vertex]:
                if to != parent:
                    stack.append((to, vertex, distance + weight))
    return result


def expected_output(case: Case) -> list[str]:
    n, edges, initially_marked, operations = case
    distance = all_distances(n, edges)
    marked = [initially_marked] * n
    output: list[str] = []
    for operation, vertex in operations:
        if operation == "T":
            marked[vertex] = not marked[vertex]
        elif operation == "M":
            marked[vertex] = True
        elif operation == "U":
            marked[vertex] = False
        elif operation == "Q":
            candidates = [distance[vertex][other] for other in range(n) if marked[other]]
            output.append(str(min(candidates)) if candidates else "NONE")
        else:
            output.append(f"{int(marked[vertex])} {sum(marked)}")
    return output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        (1, [], False, [("Q", 0), ("T", 0), ("Q", 0), ("S", 0), ("U", 0), ("Q", 0)]),
        (1, [], True, [("Q", 0), ("M", 0), ("S", 0), ("T", 0), ("Q", 0)]),
        (
            5,
            [(0, 1, 0), (1, 2, 3), (2, 3, 0), (3, 4, 8)],
            False,
            [("M", 0), ("M", 4), ("Q", 2), ("T", 0), ("Q", 1), ("S", 4)],
        ),
        (
            7,
            [(0, vertex, vertex) for vertex in range(1, 7)],
            True,
            [("Q", 6), ("U", 6), ("Q", 6), ("U", 0), ("Q", 6), ("S", 0)],
        ),
    ]

    rng = random.Random(20260713)
    operations_alphabet = ("T", "M", "U", "Q", "Q", "Q", "S")
    for case_index in range(80):
        n = rng.randint(2, 32)
        edges = [
            (vertex, rng.randrange(vertex), rng.randint(0, 20))
            for vertex in range(1, n)
        ]
        rng.shuffle(edges)
        operation_count = rng.randint(40, 140)
        operations = [
            (rng.choice(operations_alphabet), rng.randrange(n))
            for _ in range(operation_count)
        ]
        operations.extend(("Q", vertex) for vertex in range(n))
        cases.append((n, edges, bool(case_index & 1), operations))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, edges, initially_marked, operations in cases:
        input_lines.append(f"{n} {len(operations)} {int(initially_marked)}")
        input_lines.extend(f"{left} {right} {weight}" for left, right, weight in edges)
        input_lines.extend(f"{operation} {vertex}" for operation, vertex in operations)
        output_lines.extend(expected_output((n, edges, initially_marked, operations)))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
