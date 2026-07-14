#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


Operation = tuple[int, ...]


def component(graph: list[set[int]], start: int) -> list[int]:
    result = [start]
    seen = {start}
    for vertex in result:
        for to in graph[vertex]:
            if to not in seen:
                seen.add(to)
                result.append(to)
    return result


def path_sum(graph: list[set[int]], values: list[int], start: int, goal: int) -> int:
    parent = [-1] * len(graph)
    parent[start] = start
    queue = collections.deque([start])
    while queue:
        vertex = queue.popleft()
        if vertex == goal:
            break
        for to in graph[vertex]:
            if parent[to] == -1:
                parent[to] = vertex
                queue.append(to)
    result = values[goal]
    while goal != start:
        goal = parent[goal]
        result += values[goal]
    return result


def evaluate(
    values: list[int],
    initial_edges: list[tuple[int, int]],
    operations: list[Operation],
) -> list[int]:
    graph = [set() for _ in values]
    for left, right in initial_edges:
        graph[left].add(right)
        graph[right].add(left)
    current = values.copy()
    output: list[int] = []
    for operation in operations:
        if operation[0] == 0:
            _, old_left, old_right, new_left, new_right = operation
            graph[old_left].remove(old_right)
            graph[old_right].remove(old_left)
            graph[new_left].add(new_right)
            graph[new_right].add(new_left)
        elif operation[0] == 1:
            _, vertex, delta = operation
            current[vertex] += delta
        else:
            _, left, right = operation
            output.append(path_sum(graph, current, left, right))
    return output


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    edges: list[tuple[int, int]],
    operations: list[Operation],
) -> None:
    lines = [f"{len(values)} {len(operations)}", " ".join(map(str, values))]
    lines.extend(f"{left} {right}" for left, right in edges)
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    output = evaluate(values, edges, operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, output)) + ("\n" if output else ""),
        encoding="utf-8",
    )


def random_case(rng: random.Random, n: int, query_count: int) -> tuple[
    list[int], list[tuple[int, int]], list[Operation]
]:
    values = [rng.randrange(-10**9, 10**9 + 1) for _ in range(n)]
    initial_edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
    graph = [set() for _ in range(n)]
    edge_set: set[tuple[int, int]] = set()
    for left, right in initial_edges:
        graph[left].add(right)
        graph[right].add(left)
        edge_set.add(tuple(sorted((left, right))))

    operations: list[Operation] = []
    for _ in range(query_count):
        kind = rng.randrange(5)
        if kind == 0 and n > 1:
            old_left, old_right = rng.choice(sorted(edge_set))
            graph[old_left].remove(old_right)
            graph[old_right].remove(old_left)
            edge_set.remove((old_left, old_right))
            first_component = component(graph, old_left)
            first_set = set(first_component)
            second_component = [vertex for vertex in range(n) if vertex not in first_set]
            new_left = rng.choice(first_component)
            new_right = rng.choice(second_component)
            graph[new_left].add(new_right)
            graph[new_right].add(new_left)
            edge_set.add(tuple(sorted((new_left, new_right))))
            operations.append((0, old_left, old_right, new_left, new_right))
        elif kind <= 1:
            operations.append(
                (1, rng.randrange(n), rng.randrange(-10**9, 10**9 + 1))
            )
        else:
            operations.append((2, rng.randrange(n), rng.randrange(n)))
    return values, initial_edges, operations


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        0,
        [7],
        [],
        [(2, 0, 0), (1, 0, -10), (2, 0, 0), (1, 0, 10**9), (2, 0, 0)],
    )
    write_case(
        out_dir,
        1,
        [3, -2, 8, 5, -7],
        [(0, 1), (1, 2), (1, 3), (3, 4)],
        [
            (2, 2, 4),
            (1, 1, 11),
            (2, 0, 4),
            (0, 1, 3, 2, 4),
            (2, 0, 3),
            (0, 0, 1, 0, 4),
            (2, 2, 3),
        ],
    )

    rng = random.Random(20260714)
    for index, (n, query_count) in enumerate(((2, 80), (17, 500), (47, 900)), 2):
        values, edges, operations = random_case(rng, n, query_count)
        write_case(out_dir, index, values, edges, operations)


if __name__ == "__main__":
    main()
