#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path

Edge = tuple[int, int, int]


def solve(n: int, edges: list[Edge]) -> int:
    capacity = [[0] * n for _ in range(n)]
    for left, right, value in edges:
        if left == right:
            continue
        capacity[left][right] += value
        capacity[right][left] += value
    answer = 0
    while True:
        parent = [-1] * n
        parent[0] = 0
        queue = deque([0])
        while queue and parent[n - 1] == -1:
            vertex = queue.popleft()
            for to, value in enumerate(capacity[vertex]):
                if value and parent[to] == -1:
                    parent[to] = vertex
                    queue.append(to)
        if parent[n - 1] == -1:
            return answer
        pushed = 10**30
        vertex = n - 1
        while vertex != 0:
            pushed = min(pushed, capacity[parent[vertex]][vertex])
            vertex = parent[vertex]
        vertex = n - 1
        while vertex != 0:
            previous = parent[vertex]
            capacity[previous][vertex] -= pushed
            capacity[vertex][previous] += pushed
            vertex = previous
        answer += pushed


def write_case(out_dir: Path, name: str, n: int, edges: list[Edge], answer: int | None = None) -> None:
    assert 2 <= n <= 5000
    assert 1 <= len(edges) <= 30000
    assert all(0 <= left < n and 0 <= right < n and 1 <= cap <= 10**9 for left, right, cap in edges)
    if answer is None:
        answer = solve(n, edges)
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *(f"{a + 1} {b + 1} {c}" for a, b, c in edges)]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "official", 4, [(0, 1, 3), (1, 2, 4), (2, 0, 2), (1, 1, 5), (2, 3, 3), (3, 2, 3)], 5)
    write_case(out_dir, "parallel", 2, [(0, 1, 7), (0, 1, 11), (0, 0, 10)], 18)
    source = random.Random(126)
    for case_index in range(35):
        n = source.randint(2, 8)
        edges = [
            (source.randrange(n), source.randrange(n), source.randint(1, 30))
            for _ in range(source.randint(1, 20))
        ]
        write_case(out_dir, f"random_{case_index:02d}", n, edges)
    maximum = [(0, 4999, 10**9) for _ in range(30000)]
    write_case(out_dir, "maximum", 5000, maximum, 30_000_000_000_000)


if __name__ == "__main__":
    main()
