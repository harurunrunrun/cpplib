#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def adjacency(n: int, edges: list[tuple[int, int]]) -> list[list[int]]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    return graph


def centers(graph: list[list[int]]) -> list[int]:
    n = len(graph)
    if n <= 2:
        return list(range(n))
    degree = [len(edges) for edges in graph]
    leaves = [vertex for vertex, value in enumerate(degree) if value <= 1]
    remaining = n
    while remaining > 2:
        remaining -= len(leaves)
        next_leaves: list[int] = []
        for vertex in leaves:
            for to in graph[vertex]:
                degree[to] -= 1
                if degree[to] == 1:
                    next_leaves.append(to)
        leaves = next_leaves
    return sorted(leaves)


def rooted_code(graph: list[list[int]], root: int, parent: int = -1) -> str:
    children = sorted(rooted_code(graph, to, root) for to in graph[root] if to != parent)
    return "(" + "".join(children) + ")"


def unrooted_code(graph: list[list[int]]) -> str:
    return min(rooted_code(graph, center) for center in centers(graph))


def random_tree(rng: random.Random, n: int) -> list[tuple[int, int]]:
    edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, n)]
    rng.shuffle(edges)
    return edges


def permuted_edges(edges: list[tuple[int, int]], permutation: list[int]) -> list[tuple[int, int]]:
    return [(permutation[u], permutation[v]) for u, v in edges]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[int, list[tuple[int, int]], int, list[tuple[int, int]], int]] = []
    for n in range(1, 15):
        edges = [(vertex - 1, vertex) for vertex in range(1, n)]
        permutation = list(range(n))
        rng.shuffle(permutation)
        root = rng.randrange(n)
        cases.append((n, edges, root, permuted_edges(edges, permutation), permutation[root]))
    for _ in range(110):
        n = rng.randrange(1, 15)
        edges_a = random_tree(rng, n)
        root_a = rng.randrange(n)
        if rng.randrange(2) == 0:
            permutation = list(range(n))
            rng.shuffle(permutation)
            edges_b = permuted_edges(edges_a, permutation)
            root_b = permutation[root_a] if rng.randrange(2) == 0 else rng.randrange(n)
        else:
            m = rng.randrange(1, 15)
            edges_b = random_tree(rng, m)
            root_b = rng.randrange(m)
        cases.append((n, edges_a, root_a, edges_b, root_b))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, edges_a, root_a, edges_b, root_b in cases:
        m = len(edges_b) + 1
        input_lines.append(f"{n} {m} {root_a} {root_b}")
        input_lines.extend(f"{u} {v}" for u, v in edges_a)
        input_lines.extend(f"{u} {v}" for u, v in edges_b)
        graph_a = adjacency(n, edges_a)
        graph_b = adjacency(m, edges_b)
        center_a = centers(graph_a)
        center_b = centers(graph_b)
        unrooted = n == m and unrooted_code(graph_a) == unrooted_code(graph_b)
        rooted = n == m and rooted_code(graph_a, root_a) == rooted_code(graph_b, root_b)
        output_lines.append(
            " ".join(
                map(
                    str,
                    [int(unrooted), int(rooted), len(center_a), *center_a, len(center_b), *center_b],
                )
            )
        )

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
