#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from dataclasses import dataclass
from pathlib import Path


INT64_MIN = -(1 << 63)
INT64_MAX = (1 << 63) - 1


@dataclass
class Case:
    values: list[int]
    edges: list[tuple[int, int]]
    operations: list[tuple[int, ...]]


def path_vertices(
    source: int,
    target: int,
    graph: list[list[int]],
) -> list[int]:
    parent = [-1] * len(graph)
    parent[source] = source
    queue = deque([source])
    while queue:
        vertex = queue.popleft()
        if vertex == target:
            break
        for to in graph[vertex]:
            if parent[to] != -1:
                continue
            parent[to] = vertex
            queue.append(to)
    assert parent[target] != -1
    result: list[int] = []
    vertex = target
    while True:
        result.append(vertex)
        if vertex == source:
            break
        vertex = parent[vertex]
    result.reverse()
    return result


def maximum_subarray_allowing_empty(sequence: list[int]) -> int:
    answer = 0
    suffix = 0
    for value in sequence:
        suffix = max(0, suffix + value)
        answer = max(answer, suffix)
    return answer


def encode_brute(cases: list[Case]) -> tuple[list[str], list[str]]:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        n = len(case.values)
        assert len(case.edges) == n - 1
        graph: list[list[int]] = [[] for _ in range(n)]
        for left, right in case.edges:
            graph[left].append(right)
            graph[right].append(left)

        values = case.values[:]
        input_lines.append(f"{n} {len(case.operations)}")
        input_lines.append(" ".join(map(str, values)))
        input_lines.extend(f"{left} {right}" for left, right in case.edges)
        for operation in case.operations:
            input_lines.append(" ".join(map(str, operation)))
            kind, left, right = operation[:3]
            path = path_vertices(left, right, graph)
            if kind == 1:
                answer = maximum_subarray_allowing_empty(
                    [values[vertex] for vertex in path]
                )
                assert INT64_MIN <= answer <= INT64_MAX
                output_lines.append(str(answer))
            else:
                assigned = operation[3]
                for vertex in path:
                    values[vertex] = assigned

        for value in values:
            assert INT64_MIN <= value <= INT64_MAX
        assert max(abs(value) for value in values) * n <= INT64_MAX
    return input_lines, output_lines


def manual_cases() -> list[Case]:
    return [
        Case(
            [-5],
            [],
            [
                (1, 0, 0),
                (2, 0, 0, 7),
                (1, 0, 0),
                (2, 0, 0, -9),
                (1, 0, 0),
            ],
        ),
        Case(
            [-5, 4, -2, 7, -20, 6, 8],
            [(0, 1), (1, 2), (2, 3), (2, 4), (4, 5), (4, 6)],
            [
                (1, 0, 3),
                (1, 3, 0),
                (1, 3, 6),
                (1, 6, 3),
                (2, 3, 0, -3),
                (1, 0, 3),
                (2, 0, 3, 5),
                (1, 3, 0),
                (2, 2, 2, -100),
                (1, 0, 3),
            ],
        ),
    ]


def random_cases() -> list[Case]:
    rng = random.Random(0x47535337)
    result: list[Case] = []
    for case_index in range(48):
        n = rng.randint(1, 52)
        values = [rng.randint(-10**12, 10**12) for _ in range(n)]
        if case_index == 0:
            values = [-10**12] * n
        edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, n)]
        rng.shuffle(edges)
        operations: list[tuple[int, ...]] = []
        for operation_index in range(260):
            left = rng.randrange(n)
            right = rng.randrange(n)
            if operation_index % 17 == 0:
                right = left
            if rng.randrange(9) < 5:
                operations.append((1, left, right))
            else:
                assigned = rng.choice(
                    [-10**12, -1, 0, 1, 10**12]
                    if rng.randrange(5) == 0
                    else [rng.randint(-10**12, 10**12)]
                )
                operations.append((2, left, right, assigned))
        result.append(Case(values, edges, operations))
    return result


def performance_case() -> tuple[list[str], list[str]]:
    n = 100_000
    operation_count = 100_000
    input_lines = ["1", f"{n} {operation_count}"]
    input_lines.append(" ".join(["-1"] * n))
    input_lines.extend(f"{vertex - 1} {vertex}" for vertex in range(1, n))

    output_lines: list[str] = []
    uniform_value = -1
    point = 0
    for index in range(operation_count):
        phase = index % 4
        if phase == 0:
            uniform_value = ((index * 1_000_003) % 2_000_000_001) - 1_000_000_000
            left, right = (0, n - 1) if index % 8 == 0 else (n - 1, 0)
            input_lines.append(f"2 {left} {right} {uniform_value}")
        elif phase == 1:
            left = (index * 97) % n
            right = (index * 1009 + 17) % n
            input_lines.append(f"1 {left} {right}")
            length = abs(left - right) + 1
            answer = max(0, uniform_value * length)
            assert answer <= INT64_MAX
            output_lines.append(str(answer))
        elif phase == 2:
            point = (index * 65537 + 31) % n
            assigned = ((index * 999_983) % 2_000_001) - 1_000_000
            input_lines.append(f"2 {point} {point} {assigned}")
            uniform_value = assigned
        else:
            input_lines.append(f"1 {point} {point}")
            output_lines.append(str(max(0, uniform_value)))

    return input_lines, output_lines


def write_case(
    directory: Path,
    name: str,
    input_lines: list[str],
    output_lines: list[str],
) -> None:
    (directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)
    write_case(
        output_directory,
        "case_00",
        *encode_brute(manual_cases() + random_cases()),
    )
    write_case(output_directory, "case_01", *performance_case())


if __name__ == "__main__":
    main()
