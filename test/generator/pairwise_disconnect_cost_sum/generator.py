#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


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


def solve_brute(n: int, edges: list[Edge], modulus: int) -> int:
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
    return answer % modulus


def write_case(
    out_dir: Path,
    case_index: int,
    n: int,
    edges: list[Edge],
    modulus: int,
) -> None:
    answer = solve_brute(n, edges, modulus)
    name = f"case_{case_index:03d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([
            f"{n} {len(edges)} {modulus}",
            *(f"{left} {right} {weight}" for left, right, weight in edges),
        ]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        f"{answer}\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[Edge], int]] = [
        (1, [], 1_000_000_000),
        (2, [(0, 1, 5)], 1_000_000_000),
        (3, [(0, 1, 1), (1, 2, 2)], 1_000_000_000),
        (3, [(0, 1, 1), (1, 2, 2), (0, 2, 3)], 1_000_000_000),
        (4, [(0, 1, 2), (2, 3, 5)], 1_000_000_000),
        (2, [(0, 0, 1), (0, 1, 2)], 1_000_000_000),
        (3, [(0, 1, 0), (0, 1, 9), (1, 2, 4)], 17),
    ]

    source = random.Random(101061)
    for _ in range(100):
        n = source.randint(1, 8)
        edge_count = source.randint(0, 15)
        weights = source.sample(range(0, 2_000), edge_count)
        edges = [
            (source.randrange(n), source.randrange(n), weight)
            for weight in weights
        ]
        cases.append((n, edges, source.randint(1, 1_000_000_000)))

    for case_index, case in enumerate(cases):
        write_case(out_dir, case_index, *case)


if __name__ == "__main__":
    main()
