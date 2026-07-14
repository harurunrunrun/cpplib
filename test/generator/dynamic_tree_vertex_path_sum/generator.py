#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def rooted_tree(
    n: int, edges: list[tuple[int, int]], root: int
) -> tuple[list[int], list[int]]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    parent = [-1] * n
    depth = [0] * n
    order = [root]
    parent[root] = root
    for vertex in order:
        for to in graph[vertex]:
            if parent[to] == -1:
                parent[to] = vertex
                depth[to] = depth[vertex] + 1
                order.append(to)
    return parent, depth


def path_sum(
    values: list[int], parent: list[int], depth: list[int], u: int, v: int
) -> int:
    result = 0
    while depth[u] > depth[v]:
        result += values[u]
        u = parent[u]
    while depth[v] > depth[u]:
        result += values[v]
        v = parent[v]
    while u != v:
        result += values[u] + values[v]
        u = parent[u]
        v = parent[v]
    return result + values[u]


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    edges: list[tuple[int, int]],
    root: int,
    operations: list[tuple[int, int, int]],
) -> None:
    current = values.copy()
    parent, depth = rooted_tree(len(values), edges, root)
    output: list[str] = []
    for kind, first, second in operations:
        if kind == 0:
            current[first] += second
        else:
            output.append(str(path_sum(current, parent, depth, first, second)))

    lines = [f"{len(values)} {len(operations)} {root}"]
    lines.append(" ".join(map(str, values)))
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.extend(f"{kind} {first} {second}" for kind, first, second in operations)
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

    write_case(out_dir, 0, [7], [], 0, [(1, 0, 0), (0, 0, -10), (1, 0, 0)])

    chain_values = [index - 6 for index in range(13)]
    chain_edges = [(index - 1, index) for index in range(1, 13)]
    chain_operations = [
        (1, 0, 12),
        (1, 4, 9),
        (0, 6, 21),
        (1, 12, 3),
        (0, 0, -8),
        (1, 0, 0),
    ]
    write_case(out_dir, 1, chain_values, chain_edges, 7, chain_operations)

    rng = random.Random(20260714)
    n = 47
    edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
    values = [rng.randrange(-10**6, 10**6 + 1) for _ in range(n)]
    operations: list[tuple[int, int, int]] = []
    for _ in range(700):
        if rng.randrange(5) < 2:
            operations.append((0, rng.randrange(n), rng.randrange(-10**6, 10**6 + 1)))
        else:
            operations.append((1, rng.randrange(n), rng.randrange(n)))
    write_case(out_dir, 2, values, edges, rng.randrange(n), operations)


if __name__ == "__main__":
    main()
