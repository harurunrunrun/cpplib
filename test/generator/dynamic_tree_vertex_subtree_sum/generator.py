#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def subtree_vertices(
    n: int, edges: list[tuple[int, int]], root: int
) -> list[list[int]]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    parent = [-1] * n
    parent[root] = root
    order = [root]
    for vertex in order:
        for to in graph[vertex]:
            if parent[to] == -1:
                parent[to] = vertex
                order.append(to)
    result = [[vertex] for vertex in range(n)]
    for vertex in reversed(order[1:]):
        result[parent[vertex]].extend(result[vertex])
    return result


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    edges: list[tuple[int, int]],
    root: int,
    operations: list[tuple[int, int, int]],
) -> None:
    current = values.copy()
    subtrees = subtree_vertices(len(values), edges, root)
    output: list[str] = []
    for kind, vertex, delta in operations:
        if kind == 0:
            current[vertex] += delta
        else:
            output.append(str(sum(current[node] for node in subtrees[vertex])))

    lines = [f"{len(values)} {len(operations)} {root}"]
    lines.append(" ".join(map(str, values)))
    lines.extend(f"{u} {v}" for u, v in edges)
    for kind, vertex, delta in operations:
        lines.append(f"0 {vertex} {delta}" if kind == 0 else f"1 {vertex}")
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + ("\n" if output else ""), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [-4], [], 0, [(1, 0, 0), (0, 0, 9), (1, 0, 0)])

    edges = [(0, 1), (0, 2), (2, 3), (2, 4), (4, 5), (4, 6)]
    values = [3, -2, 8, 1, 5, -7, 4]
    operations = [
        (1, 4, 0),
        (1, 2, 0),
        (0, 5, 11),
        (1, 4, 0),
        (0, 0, -6),
        (1, 0, 0),
        (1, 1, 0),
    ]
    write_case(out_dir, 1, values, edges, 2, operations)

    rng = random.Random(20260715)
    n = 53
    random_edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
    random_values = [rng.randrange(-10**6, 10**6 + 1) for _ in range(n)]
    random_operations: list[tuple[int, int, int]] = []
    for _ in range(700):
        if rng.randrange(5) < 2:
            random_operations.append(
                (0, rng.randrange(n), rng.randrange(-10**6, 10**6 + 1))
            )
        else:
            random_operations.append((1, rng.randrange(n), 0))
    write_case(
        out_dir,
        2,
        random_values,
        random_edges,
        rng.randrange(n),
        random_operations,
    )


if __name__ == "__main__":
    main()
