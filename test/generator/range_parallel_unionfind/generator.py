#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


class DSU:
    def __init__(self, values: list[int]) -> None:
        self.parent = [-1] * len(values)
        self.total = values[:]
        self.groups = len(values)
        self.pair_sum = 0

    def leader(self, vertex: int) -> int:
        root = vertex
        while self.parent[root] >= 0:
            root = self.parent[root]
        while vertex != root:
            nxt = self.parent[vertex]
            self.parent[vertex] = root
            vertex = nxt
        return root

    def merge(self, left: int, right: int) -> bool:
        left = self.leader(left)
        right = self.leader(right)
        if left == right:
            return False
        if -self.parent[left] < -self.parent[right]:
            left, right = right, left
        self.pair_sum += self.total[left] * self.total[right]
        self.total[left] += self.total[right]
        self.parent[left] += self.parent[right]
        self.parent[right] = left
        self.groups -= 1
        return True


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    queries: list[tuple[int, int, int]],
) -> None:
    dsu = DSU(values)
    output: list[str] = []
    for length, first, second in queries:
        merged = 0
        for offset in range(length):
            merged += dsu.merge(first + offset, second + offset)
        output.append(f"{merged} {dsu.groups} {dsu.pair_sum}")
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(f"{length} {first} {second}" for length, first, second in queries)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_queries(seed: int, n: int, count: int, max_length: int) -> list[tuple[int, int, int]]:
    rng = random.Random(seed)
    result: list[tuple[int, int, int]] = []
    for _ in range(count):
        length = rng.randrange(min(n, max_length) + 1)
        first = rng.randrange(n - length + 1)
        second = rng.randrange(n - length + 1)
        result.append((length, first, second))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        0,
        [1, -2, 3, 4, -5, 6, 7, -8],
        [(0, 0, 8), (4, 0, 4), (3, 1, 3), (8, 0, 0), (1, 0, 7)],
    )
    for index, seed in enumerate([2026071421, 2026071422, 2026071423], start=1):
        rng = random.Random(seed)
        n = 90
        values = [rng.randrange(-1000, 1001) for _ in range(n)]
        write_case(out_dir, index, values, random_queries(seed + 10, n, 1800, n))

    n = 50000
    values = [(index * 1000003) % 2001 - 1000 for index in range(n)]
    queries = [(n, 0, 0), (n // 2, 0, n // 2)]
    queries.extend(random_queries(2026071424, n, 7000, 20))
    write_case(out_dir, 4, values, queries)


if __name__ == "__main__":
    main()
