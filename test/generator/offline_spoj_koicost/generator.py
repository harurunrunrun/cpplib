#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


MAX_N = 100_000
MAX_M = 100_000
MAX_WEIGHT = 100_000
MODULUS = 1_000_000_000
Edge = tuple[int, int, int]


def connected(
    n: int,
    edges: list[Edge],
    active: list[bool],
    source: int,
    target: int,
) -> bool:
    graph = [[] for _ in range(n)]
    for enabled, (left, right, _) in zip(active, edges):
        if not enabled:
            continue
        graph[left].append(right)
        graph[right].append(left)
    seen = [False] * n
    seen[source] = True
    queue = deque([source])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if seen[to]:
                continue
            seen[to] = True
            queue.append(to)
    return seen[target]


def solve_brute(n: int, edges: list[Edge]) -> int:
    order = sorted(range(len(edges)), key=lambda index: edges[index][2])
    assert len({weight for _, _, weight in edges}) == len(edges)
    answer = 0
    for source in range(n):
        for target in range(source + 1, n):
            active = [True] * len(edges)
            if not connected(n, edges, active, source, target):
                continue
            deleted_sum = 0
            for edge_index in order:
                deleted_sum += edges[edge_index][2]
                active[edge_index] = False
                if not connected(n, edges, active, source, target):
                    answer += deleted_sum
                    break
    return answer % MODULUS


def validate(n: int, edges: list[Edge]) -> None:
    assert 1 <= n <= MAX_N
    assert 0 <= len(edges) <= MAX_M
    assert len({weight for _, _, weight in edges}) == len(edges)
    assert all(
        0 <= left < n and 0 <= right < n
        and 1 <= weight <= MAX_WEIGHT
        for left, right, weight in edges
    )


def write_case(
    out_dir: Path,
    name: str,
    n: int,
    edges: list[Edge],
    answer: int | None = None,
) -> None:
    validate(n, edges)
    if answer is None:
        answer = solve_brute(n, edges)
    (out_dir / f"{name}.in").write_text(
        "\n".join([
            f"{n} {len(edges)}",
            *(f"{left + 1} {right + 1} {weight}" for left, right, weight in edges),
        ]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        f"{answer}\n",
        encoding="ascii",
    )


def maximum_case_answer() -> int:
    answer = 0
    for weight in range(2, MAX_WEIGHT + 1):
        left_size = 1
        right_size = MAX_N - weight + 1
        prefix_sum = weight * (weight + 1) // 2
        answer += left_size * right_size * prefix_sum
    return answer % MODULUS


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    sample = [
        (0, 1, 10), (1, 2, 2), (3, 2, 5), (5, 2, 15),
        (2, 4, 4), (3, 4, 3), (1, 5, 6),
    ]
    write_case(out_dir, "official", 6, sample, 256)
    write_case(out_dir, "single_vertex", 1, [], 0)
    write_case(out_dir, "disconnected", 6, [(0, 1, 2), (2, 3, 5)])

    source = random.Random(101061)
    for case_index in range(40):
        n = source.randint(2, 8)
        possible_edges = [
            (left, right)
            for left in range(n)
            for right in range(left + 1, n)
        ]
        edge_count = source.randint(0, min(15, len(possible_edges)))
        endpoints = source.sample(possible_edges, edge_count)
        weights = source.sample(range(1, MAX_WEIGHT + 1), edge_count)
        edges = [
            (left, right, weight)
            for (left, right), weight in zip(endpoints, weights)
        ]
        write_case(out_dir, f"random_{case_index:02d}", n, edges)

    maximum_edges = [
        (vertex, vertex + 1, vertex + 1)
        for vertex in range(MAX_N - 1)
    ]
    maximum_edges.append((0, MAX_N - 1, MAX_WEIGHT))
    write_case(
        out_dir,
        "max_n_m_weight",
        MAX_N,
        maximum_edges,
        maximum_case_answer(),
    )


if __name__ == "__main__":
    main()
