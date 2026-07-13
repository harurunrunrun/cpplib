#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


Edge = tuple[int, int, float]
Case = tuple[int, list[Edge], int, float, float, float, int, float]


def normalize_edges(vertex_count: int, input_edges: list[Edge]) -> list[Edge]:
    accumulated: dict[tuple[int, int], float] = {}
    for left, right, weight in input_edges:
        assert 0 <= left < vertex_count and 0 <= right < vertex_count
        assert math.isfinite(weight) and weight >= 0.0
        if left == right or weight == 0.0:
            continue
        endpoints = (min(left, right), max(left, right))
        accumulated[endpoints] = accumulated.get(endpoints, 0.0) + weight
    return [(left, right, weight) for (left, right), weight in sorted(accumulated.items())]


def multiply(left: list[list[float]], right: list[list[float]]) -> list[list[float]]:
    size = len(left)
    result = [[0.0] * size for _ in range(size)]
    for row in range(size):
        for middle in range(size):
            if left[row][middle] == 0.0:
                continue
            for column in range(size):
                result[row][column] += left[row][middle] * right[middle][column]
    return result


def normalize_columns(matrix: list[list[float]]) -> None:
    size = len(matrix)
    for column in range(size):
        total = sum(matrix[row][column] for row in range(size))
        if total == 0.0:
            matrix[column][column] = 1.0
            total = 1.0
        for row in range(size):
            matrix[row][column] /= total


def oracle(
    vertex_count: int,
    input_edges: list[Edge],
    expansion_power: int,
    inflation: float,
    self_loop_weight: float,
    pruning_threshold: float,
    maximum_iterations: int,
    tolerance: float,
) -> list[int]:
    if vertex_count == 0:
        return []
    matrix = [[0.0] * vertex_count for _ in range(vertex_count)]
    for left, right, weight in normalize_edges(vertex_count, input_edges):
        matrix[left][right] = weight
        matrix[right][left] = weight
    for vertex in range(vertex_count):
        matrix[vertex][vertex] += self_loop_weight
    normalize_columns(matrix)
    for _ in range(maximum_iterations):
        expanded = [row[:] for row in matrix]
        for _ in range(1, expansion_power):
            expanded = multiply(expanded, matrix)
        for column in range(vertex_count):
            maximum_row = 0
            maximum_value = -1.0
            for row in range(vertex_count):
                expanded[row][column] = expanded[row][column] ** inflation
                if expanded[row][column] > maximum_value:
                    maximum_value = expanded[row][column]
                    maximum_row = row
            for row in range(vertex_count):
                if row != maximum_row and expanded[row][column] < pruning_threshold:
                    expanded[row][column] = 0.0
        normalize_columns(expanded)
        change = max(
            abs(expanded[row][column] - matrix[row][column])
            for row in range(vertex_count)
            for column in range(vertex_count)
        )
        matrix = expanded
        if change <= tolerance:
            break

    parent = list(range(vertex_count))

    def leader(vertex: int) -> int:
        while parent[vertex] != vertex:
            parent[vertex] = parent[parent[vertex]]
            vertex = parent[vertex]
        return vertex

    def merge(left: int, right: int) -> None:
        left = leader(left)
        right = leader(right)
        if left > right:
            left, right = right, left
        if left != right:
            parent[right] = left

    for column in range(vertex_count):
        attractor = max(range(vertex_count), key=lambda row: (matrix[row][column], -row))
        merge(column, attractor)
    label_of_root: dict[int, int] = {}
    labels: list[int] = []
    for vertex in range(vertex_count):
        root = leader(vertex)
        if root not in label_of_root:
            label_of_root[root] = len(label_of_root)
        labels.append(label_of_root[root])
    return labels


def clique_groups(sizes: list[int], rng: random.Random) -> tuple[int, list[Edge]]:
    edges: list[Edge] = []
    first = 0
    for size in sizes:
        for left in range(first, first + size):
            for right in range(left + 1, first + size):
                weight = float(rng.randrange(3, 10))
                edges.append((left, right, weight))
                if rng.randrange(5) == 0:
                    split = float(rng.randrange(0, int(weight) + 1))
                    edges[-1] = (left, right, weight - split)
                    edges.append((right, left, split))
        if size != 0:
            edges.append((first, first, 1000.0))
        first += size
    return first, edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071308)

    cases: list[Case] = [
        (0, [], 2, 2.0, 1.0, 1.0e-12, 60, 1.0e-12),
        (1, [], 2, 2.0, 1.0, 1.0e-12, 60, 1.0e-12),
        (4, [], 2, 2.0, 1.0, 1.0e-12, 60, 1.0e-12),
        (2, [(0, 1, 3.0)], 2, 2.0, 1.0, 1.0e-12, 60, 1.0e-12),
        (4, [(0, 1, 5.0), (2, 3, 7.0)], 2, 2.0, 1.0, 1.0e-12, 60, 1.0e-12),
        (2, [(0, 1, 1.0), (1, 0, 2.0), (0, 0, 99.0), (0, 1, 0.0)], 2, 2.0, 1.0, 1.0e-12, 60, 1.0e-12),
        (3, [(0, 1, 2.0), (1, 2, 3.0)], 3, 1.5, 0.0, 0.0, 12, 0.0),
        (3, [(0, 1, 2.0), (1, 2, 3.0), (2, 0, 4.0)], 2, 3.0, 2.0, 1.0e-8, 40, 1.0e-11),
    ]
    for _ in range(16):
        sizes: list[int] = []
        remaining = rng.randrange(1, 9)
        while remaining:
            size = rng.randrange(1, min(4, remaining) + 1)
            sizes.append(size)
            remaining -= size
        vertex_count, edges = clique_groups(sizes, rng)
        expansion_power = rng.choice([2, 2, 3])
        inflation = rng.choice([1.5, 2.0, 2.5])
        cases.append((
            vertex_count, edges, expansion_power, inflation, 1.0,
            1.0e-11, 70, 1.0e-12,
        ))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for identifier, case in enumerate(cases):
        (
            vertex_count, edges, expansion_power, inflation, self_loop_weight,
            pruning_threshold, maximum_iterations, tolerance,
        ) = case
        input_lines.append(
            f"{identifier} {vertex_count} {len(edges)} {expansion_power} "
            f"{inflation:.17g} {self_loop_weight:.17g} {pruning_threshold:.17g} "
            f"{maximum_iterations} {tolerance:.17g}"
        )
        input_lines.extend(f"{left} {right} {weight:.17g}" for left, right, weight in edges)
        labels = oracle(*case)
        community_count = len(set(labels))
        output_lines.append(
            " ".join(["CASE", str(identifier), str(community_count), *map(str, labels)])
        )
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
