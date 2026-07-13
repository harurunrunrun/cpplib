#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from dataclasses import dataclass
from pathlib import Path


MAXIMUM = (1 << 63) - 1
MINIMUM = -(1 << 63)


@dataclass
class Case:
    n: int
    root: int
    edges: list[tuple[int, int, int]]
    queries: list[tuple[int, int]]


def brute_answers(case: Case) -> list[tuple[int, int]]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(case.n)]
    for left, right, weight in case.edges:
        graph[left].append((right, weight))
        graph[right].append((left, weight))

    answers: list[tuple[int, int]] = []
    for source, target in case.queries:
        if source == target:
            answers.append((MAXIMUM, MINIMUM))
            continue
        parent = [-1] * case.n
        parent_weight = [0] * case.n
        parent[source] = source
        queue = deque([source])
        while queue and parent[target] == -1:
            vertex = queue.popleft()
            for to, weight in graph[vertex]:
                if parent[to] != -1:
                    continue
                parent[to] = vertex
                parent_weight[to] = weight
                queue.append(to)
        minimum = MAXIMUM
        maximum = MINIMUM
        vertex = target
        while vertex != source:
            minimum = min(minimum, parent_weight[vertex])
            maximum = max(maximum, parent_weight[vertex])
            vertex = parent[vertex]
        answers.append((minimum, maximum))
    return answers


def encode(cases: list[Case]) -> tuple[list[str], list[str]]:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        input_lines.append(f"{case.n} {len(case.queries)} {case.root}")
        input_lines.extend(
            f"{left} {right} {weight}" for left, right, weight in case.edges
        )
        input_lines.extend(f"{left} {right}" for left, right in case.queries)
        output_lines.extend(
            f"{minimum} {maximum}" for minimum, maximum in brute_answers(case)
        )
    return input_lines, output_lines


def write_case(
    directory: Path,
    name: str,
    input_lines: list[str],
    output_lines: list[str],
) -> None:
    (directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def manual_cases() -> list[Case]:
    return [
        Case(1, 0, [], [(0, 0), (0, 0)]),
        Case(
            7,
            3,
            [
                (0, 1, 5),
                (1, 2, -8),
                (1, 3, 13),
                (3, 4, MINIMUM),
                (3, 5, MAXIMUM),
                (5, 6, -1),
            ],
            [(0, 2), (2, 4), (4, 6), (0, 6), (3, 3), (5, 6)],
        ),
    ]


def random_cases() -> list[Case]:
    rng = random.Random(20260714)
    result: list[Case] = []
    special = [MINIMUM, MAXIMUM, -1, 0, 1]
    for _ in range(45):
        n = rng.randint(1, 55)
        edges: list[tuple[int, int, int]] = []
        for vertex in range(1, n):
            weight = (
                rng.choice(special)
                if rng.randrange(7) == 0
                else rng.randint(-10**15, 10**15)
            )
            edges.append((vertex, rng.randrange(vertex), weight))
        rng.shuffle(edges)
        queries = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(rng.randint(90, 210))
        ]
        queries.extend((vertex, vertex) for vertex in range(n))
        result.append(Case(n, rng.randrange(n), edges, queries))
    return result


def medium_path_case() -> Case:
    n = 4096
    edges = [
        (
            vertex - 1,
            vertex,
            MINIMUM if vertex == 1 else MAXIMUM if vertex == n - 1 else vertex - 2048,
        )
        for vertex in range(1, n)
    ]
    queries = [
        (0, n - 1),
        (1, n - 2),
        (n - 1, 0),
        (2048, 2048),
    ]
    queries.extend((index, n - 1 - index) for index in range(1024))
    return Case(n, n - 1, edges, queries)


def large_star() -> tuple[list[str], list[str]]:
    n = 100_000

    def weight(leaf: int) -> int:
        if leaf == 1:
            return MINIMUM
        if leaf == 2:
            return MAXIMUM
        return ((leaf * 1_000_003) % 2_000_000_001) - 1_000_000_000

    rng = random.Random(314159265)
    edges = [(0, leaf, weight(leaf)) for leaf in range(1, n)]
    rng.shuffle(edges)
    queries: list[tuple[int, int]] = []
    for index in range(100_000):
        if index % 19 == 0:
            vertex = (index * 37) % n
            queries.append((vertex, vertex))
        elif index % 7 == 0:
            queries.append((0, 1 + (index * 97) % (n - 1)))
        else:
            queries.append(
                (
                    1 + (index * 97) % (n - 1),
                    1 + (index * 1009 + 17) % (n - 1),
                )
            )

    input_lines = ["1", f"{n} {len(queries)} {n - 1}"]
    input_lines.extend(
        f"{left} {right} {edge_weight}"
        for left, right, edge_weight in edges
    )
    input_lines.extend(f"{left} {right}" for left, right in queries)

    output_lines: list[str] = []
    for left, right in queries:
        if left == right:
            answer = (MAXIMUM, MINIMUM)
        elif left == 0:
            answer = (weight(right), weight(right))
        elif right == 0:
            answer = (weight(left), weight(left))
        else:
            left_weight = weight(left)
            right_weight = weight(right)
            answer = (min(left_weight, right_weight), max(left_weight, right_weight))
        output_lines.append(f"{answer[0]} {answer[1]}")
    return input_lines, output_lines


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "case_00", *encode(manual_cases()))
    write_case(out_dir, "case_01", *encode(random_cases()))
    write_case(out_dir, "case_02", *encode([medium_path_case()]))
    write_case(out_dir, "case_03", *large_star())


if __name__ == "__main__":
    main()
