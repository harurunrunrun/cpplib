#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def path_edges(n: int, edges: list[tuple[int, int, int]], source: int, target: int) -> list[int]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for edge_id, (left, right, _) in enumerate(edges):
        graph[left].append((right, edge_id))
        graph[right].append((left, edge_id))
    parent = [(-1, -1)] * n
    parent[source] = (source, -1)
    queue = deque([source])
    while queue:
        vertex = queue.popleft()
        for next_vertex, edge_id in graph[vertex]:
            if parent[next_vertex][0] == -1:
                parent[next_vertex] = (vertex, edge_id)
                queue.append(next_vertex)
    result: list[int] = []
    current = target
    while current != source:
        previous, edge_id = parent[current]
        result.append(edge_id)
        current = previous
    return result


def write_case(
    directory: Path,
    name: str,
    n: int,
    edges: list[tuple[int, int, int]],
    operations: list[tuple[str, int, int]],
) -> None:
    current = [weight for _, _, weight in edges]
    output: list[str] = []
    lines = ["1", str(n)]
    lines.extend(f"{left + 1} {right + 1} {weight}" for left, right, weight in edges)
    for operation, first, second in operations:
        lines.append(f"{operation} {first + 1} {second if operation == 'CHANGE' else second + 1}")
        if operation == "CHANGE":
            current[first] = second
        else:
            ids = path_edges(n, edges, first, second)
            output.append(str(max(current[edge] for edge in ids)))
    lines.append("DONE")
    (directory / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / f"{name}.out").write_text("\n".join(output) + ("\n" if output else ""), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    directory = Path(parser.parse_args().out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071501)

    write_case(directory, "case_00_two", 2, [(0, 1, 1)], [
        ("QUERY", 0, 1), ("CHANGE", 0, 2_147_483_000), ("QUERY", 1, 0),
    ])
    write_case(directory, "case_01_chain", 8, [(i - 1, i, i) for i in range(1, 8)], [
        ("QUERY", 0, 7), ("CHANGE", 3, 100), ("QUERY", 2, 6),
        ("CHANGE", 3, 4), ("QUERY", 6, 2),
    ])

    for case in range(2, 10):
        n = 5 + case * 7
        edges = [(rng.randrange(vertex), vertex, rng.randrange(1, 10_000)) for vertex in range(1, n)]
        operations: list[tuple[str, int, int]] = []
        for _ in range(250):
            if rng.random() < 0.35:
                operations.append(("CHANGE", rng.randrange(n - 1), rng.randrange(1, 10_000)))
            else:
                left = rng.randrange(n)
                right = rng.randrange(n - 1)
                if right >= left:
                    right += 1
                operations.append(("QUERY", left, right))
        write_case(directory, f"case_{case:02d}_random", n, edges, operations)


if __name__ == "__main__":
    main()
