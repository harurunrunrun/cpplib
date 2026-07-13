#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from fractions import Fraction
from pathlib import Path


Edge = tuple[int, int]
Case = tuple[int, list[Edge], int, int]


def normalize_edges(vertex_count: int, input_edges: list[Edge]) -> list[Edge]:
    result = set()
    for left, right in input_edges:
        assert 0 <= left < vertex_count and 0 <= right < vertex_count
        if left != right:
            result.add((min(left, right), max(left, right)))
    return sorted(result)


def components(vertex_count: int, edges: list[Edge], active: list[bool]) -> tuple[list[int], int]:
    adjacency: list[list[int]] = [[] for _ in range(vertex_count)]
    for enabled, (left, right) in zip(active, edges):
        if enabled:
            adjacency[left].append(right)
            adjacency[right].append(left)
    labels = [-1] * vertex_count
    count = 0
    for start in range(vertex_count):
        if labels[start] != -1:
            continue
        labels[start] = count
        queue = deque([start])
        while queue:
            vertex = queue.popleft()
            for neighbor in adjacency[vertex]:
                if labels[neighbor] == -1:
                    labels[neighbor] = count
                    queue.append(neighbor)
        count += 1
    return labels, count


def edge_betweenness(vertex_count: int, edges: list[Edge], active: list[bool]) -> list[Fraction]:
    adjacency: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
    for edge_index, (left, right) in enumerate(edges):
        if active[edge_index]:
            adjacency[left].append((right, edge_index))
            adjacency[right].append((left, edge_index))
    scores = [Fraction(0) for _ in edges]
    for source in range(vertex_count):
        distance = [-1] * vertex_count
        path_count = [0] * vertex_count
        predecessor: list[list[tuple[int, int]]] = [[] for _ in range(vertex_count)]
        order: list[int] = []
        distance[source] = 0
        path_count[source] = 1
        queue = deque([source])
        while queue:
            vertex = queue.popleft()
            order.append(vertex)
            for neighbor, edge_index in adjacency[vertex]:
                if distance[neighbor] == -1:
                    distance[neighbor] = distance[vertex] + 1
                    queue.append(neighbor)
                if distance[neighbor] == distance[vertex] + 1:
                    path_count[neighbor] += path_count[vertex]
                    predecessor[neighbor].append((vertex, edge_index))
        dependency = [Fraction(0) for _ in range(vertex_count)]
        for vertex in reversed(order):
            for previous, edge_index in predecessor[vertex]:
                contribution = Fraction(path_count[previous], path_count[vertex]) * (
                    1 + dependency[vertex]
                )
                scores[edge_index] += contribution
                dependency[previous] += contribution
    return scores


def oracle(vertex_count: int, input_edges: list[Edge], target: int, maximum_rounds: int):
    edges = normalize_edges(vertex_count, input_edges)
    active = [True] * len(edges)
    labels, count = components(vertex_count, edges, active)
    removed: list[Edge] = []
    while count < target and len(removed) < maximum_rounds:
        scores = edge_betweenness(vertex_count, edges, active)
        best = None
        for edge_index in range(len(edges)):
            if active[edge_index] and (best is None or scores[edge_index] > scores[best]):
                best = edge_index
        if best is None:
            break
        active[best] = False
        removed.append(edges[best])
        labels, count = components(vertex_count, edges, active)
    return labels, count, removed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071307)

    cases: list[Case] = [
        (0, [], 0, 0),
        (1, [(0, 0)], 1, 4),
        (2, [(0, 1)], 2, 4),
        (4, [(0, 1), (2, 3)], 4, 4),
        (6, [(0, 1), (1, 2), (0, 2), (2, 3), (3, 4), (4, 5), (3, 5)], 2, 7),
        (4, [(0, 1), (1, 2), (2, 3)], 3, 3),
        (4, [(0, 1), (1, 2), (2, 3), (3, 0)], 2, 4),
        (3, [(0, 1), (1, 0), (1, 1), (1, 2), (2, 1)], 3, 3),
        (5, [(0, 1), (1, 2), (2, 3), (3, 4)], 5, 0),
    ]
    for _ in range(18):
        vertex_count = rng.randrange(1, 10)
        input_edges: list[Edge] = []
        for _ in range(rng.randrange(0, vertex_count * 3 + 1)):
            left = rng.randrange(vertex_count)
            right = rng.randrange(vertex_count)
            input_edges.append((left, right))
            if rng.randrange(5) == 0:
                input_edges.append((right, left))
        normalized = normalize_edges(vertex_count, input_edges)
        _, initial_count = components(vertex_count, normalized, [True] * len(normalized))
        target = rng.randrange(initial_count, vertex_count + 1)
        maximum_rounds = rng.randrange(0, len(normalized) + 2)
        cases.append((vertex_count, input_edges, target, maximum_rounds))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for identifier, (vertex_count, edges, target, maximum_rounds) in enumerate(cases):
        input_lines.append(
            f"{identifier} {vertex_count} {len(edges)} {target} {maximum_rounds}"
        )
        input_lines.extend(f"{left} {right}" for left, right in edges)
        labels, count, removed = oracle(vertex_count, edges, target, maximum_rounds)
        tokens = ["CASE", str(identifier), str(count), str(len(removed))]
        tokens.extend(map(str, labels))
        for left, right in removed:
            tokens.extend((str(left), str(right)))
        output_lines.append(" ".join(tokens))
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
