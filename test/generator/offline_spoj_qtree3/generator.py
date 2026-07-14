#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]
Operation = tuple[int, int]


def random_tree(size: int, rng: random.Random) -> list[Edge]:
    edges = [
        (rng.randrange(vertex), vertex)
        for vertex in range(1, size)
    ]
    rng.shuffle(edges)
    return edges


def chain_tree(size: int) -> list[Edge]:
    return [(vertex - 1, vertex) for vertex in range(1, size)]


def rooted_parent(size: int, edges: list[Edge]) -> list[int]:
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    parent = [-1] * size
    parent[0] = -2
    stack = [0]
    while stack:
        vertex = stack.pop()
        for next_vertex in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                stack.append(next_vertex)
    parent[0] = -1
    return parent


def naive_answers(
    size: int,
    edges: list[Edge],
    operations: list[Operation],
) -> list[str]:
    parent = rooted_parent(size, edges)
    black = [False] * size
    answers: list[str] = []
    for operation_type, vertex in operations:
        if operation_type == 0:
            black[vertex] = not black[vertex]
            continue

        root_path = []
        current = vertex
        while current != -1:
            root_path.append(current)
            current = parent[current]
        answer = -1
        for candidate in reversed(root_path):
            if black[candidate]:
                answer = candidate + 1
                break
        answers.append(str(answer))
    return answers


def random_operations(
    size: int,
    count: int,
    rng: random.Random,
) -> list[Operation]:
    operations: list[Operation] = [(1, 0), (1, size - 1)]
    for _ in range(max(0, count - len(operations))):
        operation_type = 0 if rng.random() < 0.45 else 1
        operations.append((operation_type, rng.randrange(size)))
    return operations


def write_case(
    output_directory: Path,
    name: str,
    size: int,
    edges: list[Edge],
    operations: list[Operation],
) -> None:
    input_lines = [f"{size} {len(operations)}"]
    input_lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    input_lines.extend(
        f"{operation_type} {vertex + 1}"
        for operation_type, vertex in operations
    )
    output_lines = naive_answers(size, edges, operations)
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    output_directory = Path(parser.parse_args().out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071403)

    single_operations: list[Operation] = [
        (1, 0),
        (0, 0),
        (1, 0),
        (0, 0),
        (1, 0),
        (0, 0),
        (1, 0),
    ]
    write_case(
        output_directory,
        "case_00_single_vertex",
        1,
        [],
        single_operations,
    )

    chain_size = 17
    chain_operations: list[Operation] = [
        (1, 16),
        (0, 16),
        (1, 16),
        (1, 8),
        (0, 0),
        (1, 16),
        (0, 8),
        (1, 16),
        (0, 0),
        (1, 16),
        (1, 7),
        (0, 8),
        (1, 16),
        (0, 16),
        (1, 16),
    ]
    write_case(
        output_directory,
        "case_01_chain_boundaries",
        chain_size,
        chain_tree(chain_size),
        chain_operations,
    )

    star_size = 101
    star_edges = [(0, vertex) for vertex in range(1, star_size)]
    star_operations = random_operations(star_size, 1200, rng)
    star_operations.extend(
        [(0, 0), (1, star_size - 1), (0, 0), (1, star_size - 1)]
    )
    write_case(
        output_directory,
        "case_02_star",
        star_size,
        star_edges,
        star_operations,
    )

    random_size = 1200
    random_edges = random_tree(random_size, rng)
    write_case(
        output_directory,
        "case_03_random",
        random_size,
        random_edges,
        random_operations(random_size, 5000, rng),
    )

    maximum_size = 100_000
    maximum_operations: list[Operation] = [
        (1, maximum_size - 1),
        (0, maximum_size - 1),
        (1, maximum_size - 1),
        (0, 0),
        (1, maximum_size - 1),
        (0, maximum_size // 2),
        (1, maximum_size - 1),
        (1, maximum_size // 2 - 1),
        (0, 0),
        (1, maximum_size - 1),
        (0, maximum_size // 2),
        (1, maximum_size - 1),
        (0, maximum_size - 1),
        (1, maximum_size - 1),
    ]
    write_case(
        output_directory,
        "case_04_maximum_chain",
        maximum_size,
        chain_tree(maximum_size),
        maximum_operations,
    )


if __name__ == "__main__":
    main()
