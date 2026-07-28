#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def brute_cut(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    source: int,
    sink: int,
) -> int:
    return min(
        sum(
            capacity
            for from_, to, capacity in edges
            if mask >> from_ & 1 and not (mask >> to & 1)
        )
        for mask in range(1 << vertex_count)
        if mask >> source & 1 and not (mask >> sink & 1)
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases = [
        (2, 0, 1, []),
        (4, 0, 3, [(0, 1, 5), (0, 2, 7), (1, 3, 3), (2, 3, 9)]),
        (5, 0, 4, [(0, 1, 9), (1, 2, 9), (0, 3, 4), (3, 4, 4)]),
        (3, 0, 2, [(0, 1, 10), (1, 0, 4), (1, 2, 8), (0, 2, 1)]),
    ]
    for _ in range(280):
        vertex_count = rng.randint(2, 8)
        source, sink = rng.sample(range(vertex_count), 2)
        edges = [
            (from_, to, rng.randint(0, 100))
            for from_ in range(vertex_count)
            for to in range(vertex_count)
            if from_ != to and rng.random() < 0.32
        ]
        cases.append((vertex_count, source, sink, edges))
    input_lines = [str(len(cases))]
    output_lines = []
    for vertex_count, source, sink, edges in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} {source} {sink}"
        )
        input_lines.extend(
            f"{from_} {to} {capacity}"
            for from_, to, capacity in edges
        )
        output_lines.append(str(brute_cut(
            vertex_count, edges, source, sink
        )))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
