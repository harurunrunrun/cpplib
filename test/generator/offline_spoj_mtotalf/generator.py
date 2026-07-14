#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path

Pipe = tuple[str, str, int]
NAMES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"


def solve(pipes: list[Pipe]) -> int:
    capacity = [[0] * 52 for _ in range(52)]
    for left_name, right_name, value in pipes:
        left = NAMES.index(left_name)
        right = NAMES.index(right_name)
        if left == right:
            continue
        capacity[left][right] += value
        capacity[right][left] += value
    answer = 0
    while True:
        parent = [-1] * 52
        parent[0] = 0
        queue = deque([0])
        while queue and parent[25] == -1:
            vertex = queue.popleft()
            for to, value in enumerate(capacity[vertex]):
                if value and parent[to] == -1:
                    parent[to] = vertex
                    queue.append(to)
        if parent[25] == -1:
            return answer
        pushed = 10**30
        vertex = 25
        while vertex:
            pushed = min(pushed, capacity[parent[vertex]][vertex])
            vertex = parent[vertex]
        vertex = 25
        while vertex:
            previous = parent[vertex]
            capacity[previous][vertex] -= pushed
            capacity[vertex][previous] += pushed
            vertex = previous
        answer += pushed


def write_case(out_dir: Path, name: str, pipes: list[Pipe], answer: int | None = None) -> None:
    assert 1 <= len(pipes) <= 700
    assert all(left in NAMES and right in NAMES and left != right and 1 <= cap <= 1000 for left, right, cap in pipes)
    if answer is None:
        answer = solve(pipes)
    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(pipes)), *(f"{a} {b} {c}" for a, b, c in pipes)]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "official", [("A", "B", 3), ("B", "C", 3), ("C", "D", 5), ("D", "Z", 4), ("B", "Z", 6)], 3)
    source = random.Random(130)
    for case_index in range(35):
        used = NAMES[:source.randint(26, 35)]
        pipes = []
        for _ in range(source.randint(1, 80)):
            left, right = source.sample(used, 2)
            pipes.append((left, right, source.randint(1, 50)))
        write_case(out_dir, f"random_{case_index:02d}", pipes)
    write_case(out_dir, "maximum", [("A", "Z", 1000) for _ in range(700)], 700000)


if __name__ == "__main__":
    main()
