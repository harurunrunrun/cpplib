#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


class DSU:
    def __init__(self, weights: list[int]) -> None:
        self.parent = [-1] * len(weights)
        self.weights = [value % MOD for value in weights]
        self.answer = 0

    def leader(self, vertex: int) -> int:
        while self.parent[vertex] >= 0:
            vertex = self.parent[vertex]
        return vertex

    def merge(self, left: int, right: int) -> None:
        left = self.leader(left)
        right = self.leader(right)
        if left == right:
            return
        if self.parent[left] > self.parent[right]:
            left, right = right, left
        self.answer = (self.answer + self.weights[left] * self.weights[right]) % MOD
        self.weights[left] = (self.weights[left] + self.weights[right]) % MOD
        self.parent[left] += self.parent[right]
        self.parent[right] = left


def write_case(
    out_dir: Path,
    index: int,
    weights: list[int],
    queries: list[tuple[int, int, int]],
) -> None:
    dsu = DSU(weights)
    answers: list[int] = []
    for length, first, second in queries:
        for offset in range(length):
            dsu.merge(first + offset, second + offset)
        answers.append(dsu.answer)
    lines = [f"{len(weights)} {len(queries)}", " ".join(map(str, weights))]
    lines.extend(" ".join(map(str, query)) for query in queries)
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + ("\n" if answers else ""),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [7], [(0, 0, 1), (1, 0, 0)])
    rng = random.Random(2026071404)
    for index in range(1, 31):
        size = rng.randrange(1, 90)
        weights = [rng.randrange(MOD) for _ in range(size)]
        queries: list[tuple[int, int, int]] = []
        for _ in range(rng.randrange(30, 250)):
            length = rng.randrange(size + 1)
            queries.append((
                length,
                rng.randrange(size - length + 1),
                rng.randrange(size - length + 1),
            ))
        write_case(out_dir, index, weights, queries)


if __name__ == "__main__":
    main()
