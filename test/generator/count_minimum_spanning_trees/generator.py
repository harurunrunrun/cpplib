#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


MODULUS = 1_000_000_007


def brute_force(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
) -> tuple[int, int, int]:
    if vertex_count <= 1:
        return (1, 0, 1)
    best_weight: int | None = None
    best_count = 0
    for selected in itertools.combinations(range(len(edges)), vertex_count - 1):
        parent = list(range(vertex_count))

        def leader(vertex: int) -> int:
            while parent[vertex] != vertex:
                parent[vertex] = parent[parent[vertex]]
                vertex = parent[vertex]
            return vertex

        weight = 0
        is_tree = True
        for edge_index in selected:
            first, second, edge_weight = edges[edge_index]
            first_root = leader(first)
            second_root = leader(second)
            if first_root == second_root:
                is_tree = False
                break
            parent[second_root] = first_root
            weight += edge_weight
        if not is_tree:
            continue
        root = leader(0)
        if any(leader(vertex) != root for vertex in range(vertex_count)):
            continue
        if best_weight is None or weight < best_weight:
            best_weight = weight
            best_count = 1
        elif weight == best_weight:
            best_count += 1
    if best_weight is None:
        return (0, 0, 0)
    return (1, best_weight, best_count % MODULUS)


def write_cases(
    directory: Path,
    index: int,
    cases: list[tuple[int, list[tuple[int, int, int]], tuple[int, int, int]]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, answer in cases:
        input_lines.append(f"{vertex_count} {len(edges)}")
        input_lines.extend(f"{u} {v} {weight}" for u, v, weight in edges)
        output_lines.append(" ".join(map(str, answer)))
    (directory / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def make_small_cases(rng: random.Random):
    cases: list[tuple[int, list[tuple[int, int, int]], tuple[int, int, int]]] = []
    explicit_graphs = [
        (0, []),
        (1, [(0, 0, -10**12), (0, 0, 10**12)]),
        (2, [(0, 1, -10**12), (0, 1, -10**12), (0, 0, -10**15)]),
        (3, [(0, 1, 2), (0, 1, 2), (1, 2, -1), (0, 2, 5)]),
        (4, [(0, 1, 0), (2, 3, 0)]),
        (
            5,
            [
                (first, second, 7)
                for first in range(5)
                for second in range(first + 1, 5)
            ],
        ),
    ]
    for vertex_count, edges in explicit_graphs:
        cases.append((vertex_count, edges, brute_force(vertex_count, edges)))

    weight_pool = [-10**12, -4, -1, 0, 0, 0, 1, 2, 5, 10**12]
    for _ in range(320):
        vertex_count = rng.randrange(0, 8)
        if vertex_count == 0:
            edges: list[tuple[int, int, int]] = []
        else:
            edge_count = rng.randrange(0, 13)
            edges = []
            for _ in range(edge_count):
                first = rng.randrange(vertex_count)
                second = first if rng.randrange(6) == 0 else rng.randrange(vertex_count)
                edges.append((first, second, rng.choice(weight_pool)))
                if rng.randrange(12) == 0 and len(edges) < 12:
                    edges.append(edges[-1])
        cases.append((vertex_count, edges, brute_force(vertex_count, edges)))
    return cases


def make_large_cases():
    chain_vertex_count = 100_000
    chain_edges = [
        (vertex, vertex + 1, vertex - 50_000)
        for vertex in range(chain_vertex_count - 1)
    ]
    chain_edges.extend(
        (index * 499 % chain_vertex_count, index * 499 % chain_vertex_count, -10**15 + index)
        for index in range(200)
    )
    chain_weight = sum(vertex - 50_000 for vertex in range(chain_vertex_count - 1))

    complete_vertex_count = 50
    complete_edges = [
        (first, second, 3)
        for first in range(complete_vertex_count)
        for second in range(first + 1, complete_vertex_count)
    ]
    complete_count = pow(
        complete_vertex_count,
        complete_vertex_count - 2,
        MODULUS,
    )
    return [
        (
            chain_vertex_count,
            chain_edges,
            (1, chain_weight, 1),
        ),
        (
            complete_vertex_count,
            complete_edges,
            (1, 3 * (complete_vertex_count - 1), complete_count),
        ),
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    write_cases(
        output_directory,
        0,
        make_small_cases(random.Random(2026071603)),
    )
    write_cases(output_directory, 1, make_large_cases())


if __name__ == "__main__":
    main()

