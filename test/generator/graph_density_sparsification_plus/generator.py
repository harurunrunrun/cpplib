#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import fractions
import itertools
import random
from pathlib import Path


def maximum_density(
    n: int, edges: list[tuple[int, int]]
) -> fractions.Fraction | None:
    if n == 0:
        return None
    simple_edges = {
        (min(u, v), max(u, v)) for u, v in edges if u != v
    }
    answer = fractions.Fraction(0)
    for mask in range(1, 1 << n):
        vertices = mask.bit_count()
        internal = sum(
            bool(mask >> u & 1) and bool(mask >> v & 1)
            for u, v in simple_edges
        )
        answer = max(answer, fractions.Fraction(internal, vertices))
    return answer


class Dsu:
    def __init__(self, n: int) -> None:
        self.parent = list(range(n))
        self.size = [1] * n

    def leader(self, vertex: int) -> int:
        while self.parent[vertex] != vertex:
            self.parent[vertex] = self.parent[self.parent[vertex]]
            vertex = self.parent[vertex]
        return vertex

    def unite(self, left: int, right: int) -> bool:
        left = self.leader(left)
        right = self.leader(right)
        if left == right:
            return False
        if self.size[left] < self.size[right]:
            left, right = right, left
        self.parent[right] = left
        self.size[left] += self.size[right]
        return True


def sparse_certificate(
    n: int, edges: list[tuple[int, int]], connectivity: int
) -> list[int]:
    remaining = [
        edge_id for edge_id, (u, v) in enumerate(edges) if u != v
    ]
    selected: list[int] = []
    for _ in range(connectivity):
        if not remaining:
            break
        dsu = Dsu(n)
        next_remaining: list[int] = []
        for edge_id in remaining:
            if dsu.unite(*edges[edge_id]):
                selected.append(edge_id)
            else:
                next_remaining.append(edge_id)
        remaining = next_remaining
    return sorted(selected)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, int, int, list[tuple[int, int]]]] = [
        (0, 0, 0, []),
        (0, 4, 0, [(0, 1), (1, 2), (0, 2), (2, 3)]),
        (1, 4, 0, [(0, 1), (1, 2), (2, 3), (3, 0)]),
        (1, 3, 2, [(0, 1), (0, 1), (1, 2), (0, 2)]),
    ]
    for _ in range(130):
        n = rng.randint(0, 11)
        edges: list[tuple[int, int]] = []
        for u, v in itertools.combinations(range(n), 2):
            if rng.random() < 0.35:
                edges.append((u, v))
                if rng.random() < 0.1:
                    edges.append((u, v))
        if n and rng.random() < 0.1:
            vertex = rng.randrange(n)
            edges.append((vertex, vertex))
        cases.append((0, n, 0, edges))
    for _ in range(140):
        n = rng.randint(0, 9)
        edges = []
        for u, v in itertools.combinations(range(n), 2):
            for _ in range(rng.randrange(4)):
                if rng.random() < 0.45:
                    edges.append((u, v))
        if n and rng.random() < 0.15:
            vertex = rng.randrange(n)
            edges.append((vertex, vertex))
        rng.shuffle(edges)
        cases.append((1, n, rng.randint(0, 7), edges))
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for kind, n, connectivity, edges in cases:
        if kind == 0:
            input_lines.append(f"0 {n} {len(edges)}")
            answer = maximum_density(n, edges)
            output_lines.append(
                "none" if answer is None
                else f"{answer.numerator} {answer.denominator}"
            )
        else:
            input_lines.append(
                f"1 {n} {len(edges)} {connectivity}"
            )
            selected = sparse_certificate(n, edges, connectivity)
            output_lines.append(" ".join(map(
                str, [len(selected), *selected]
            )))
        input_lines.extend(f"{u} {v}" for u, v in edges)
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
