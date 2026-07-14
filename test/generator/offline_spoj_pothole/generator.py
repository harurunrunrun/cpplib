#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def solve(n: int, corridors: list[tuple[int, int]]) -> int:
    inf = 10**6
    capacity = [[0] * n for _ in range(n)]
    for left, right in corridors:
        capacity[left][right] += 1 if left == 0 or right == n - 1 else inf
    answer = 0
    while True:
        parent = [-1] * n
        parent[0] = 0
        queue = deque([0])
        while queue and parent[-1] == -1:
            vertex = queue.popleft()
            for to, value in enumerate(capacity[vertex]):
                if value and parent[to] == -1:
                    parent[to] = vertex
                    queue.append(to)
        if parent[-1] == -1:
            return answer
        pushed = inf
        vertex = n - 1
        while vertex:
            pushed = min(pushed, capacity[parent[vertex]][vertex])
            vertex = parent[vertex]
        vertex = n - 1
        while vertex:
            previous = parent[vertex]
            capacity[previous][vertex] -= pushed
            capacity[vertex][previous] += pushed
            vertex = previous
        answer += pushed


def write_case(out_dir: Path, name: str, cases: list[tuple[int, list[tuple[int, int]], int | None]]) -> None:
    lines = [str(len(cases))]
    answers = []
    for n, corridors, answer in cases:
        lines.append(str(n))
        by_from = [[] for _ in range(n - 1)]
        for left, right in corridors:
            assert 0 <= left < n - 1 and left < right < n
            by_from[left].append(right)
        for destinations in by_from:
            lines.append(" ".join([str(len(destinations)), *(str(to + 1) for to in destinations)]))
        answers.append(str(solve(n, corridors) if answer is None else answer))
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(answers) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "basic", [(2, [(0, 1)], 1), (5, [(0, 1), (0, 2), (1, 3), (2, 3), (3, 4)], 1)])
    source = random.Random(129)
    cases = []
    for _ in range(35):
        n = source.randint(2, 9)
        possible = [(a, b) for a in range(n) for b in range(a + 1, n)]
        corridors = source.sample(possible, source.randint(1, len(possible)))
        cases.append((n, corridors, None))
    write_case(out_dir, "random", cases)
    n = 200
    maximum = [(0, vertex) for vertex in range(1, n - 1)] + [(vertex, n - 1) for vertex in range(1, n - 1)]
    write_case(out_dir, "maximum", [(n, maximum, n - 2)])


if __name__ == "__main__":
    main()
