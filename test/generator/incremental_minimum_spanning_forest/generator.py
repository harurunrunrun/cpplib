#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]


class Dsu:
    def __init__(self, size: int) -> None:
        self.parent = list(range(size))

    def root(self, vertex: int) -> int:
        while self.parent[vertex] != vertex:
            self.parent[vertex] = self.parent[self.parent[vertex]]
            vertex = self.parent[vertex]
        return vertex

    def unite(self, first: int, second: int) -> bool:
        first = self.root(first)
        second = self.root(second)
        if first == second:
            return False
        self.parent[second] = first
        return True


def answers(vertex_count: int, edges: list[Edge]) -> list[int]:
    result: list[int] = []
    for last in range(len(edges)):
        chosen: set[int] = set()
        dsu = Dsu(vertex_count)
        for edge in sorted(range(last + 1), key=lambda index: edges[index][2]):
            first, second, _ = edges[edge]
            if dsu.unite(first, second):
                chosen.add(edge)
        previous = set()
        if last:
            dsu = Dsu(vertex_count)
            for edge in sorted(range(last), key=lambda index: edges[index][2]):
                first, second, _ = edges[edge]
                if dsu.unite(first, second):
                    previous.add(edge)
        removed = previous - chosen
        result.append(next(iter(removed)) if removed else (
            last if last not in chosen else -1
        ))
    return result


def write_case(
    out_dir: Path,
    name: str,
    cases: list[tuple[int, list[Edge]]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges in cases:
        input_lines.append(f"{vertex_count} {len(edges)}")
        input_lines.extend(f"{u} {v} {w}" for u, v, w in edges)
        output_lines.append(" ".join(map(str, answers(vertex_count, edges))))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        args.out_dir,
        "fixed",
        [
            (1, [(0, 0, 5)]),
            (4, [
                (0, 1, 10), (1, 2, 20), (0, 2, 5),
                (2, 3, 30), (0, 3, 40), (1, 3, 7),
            ]),
        ],
    )

    random_engine = random.Random(args.seed)
    cases: list[tuple[int, list[Edge]]] = []
    for _ in range(40):
        vertex_count = random_engine.randint(1, 40)
        edge_count = random_engine.randint(1, 160)
        weights = random_engine.sample(range(1, 1_000_000), edge_count)
        edges = [
            (
                random_engine.randrange(vertex_count),
                random_engine.randrange(vertex_count),
                weights[index],
            )
            for index in range(edge_count)
        ]
        cases.append((vertex_count, edges))
    write_case(args.out_dir, "random", cases)


if __name__ == "__main__":
    main()
