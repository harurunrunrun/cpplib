#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def exists(
    n: int, edges: list[tuple[int, int]], source: int, sink: int
) -> bool:
    if n == 1:
        return source == sink == 0
    adjacent = [[] for _ in range(n)]
    for left, right in edges:
        adjacent[left].append(right)
        adjacent[right].append(left)
    middle = [v for v in range(n) if v not in (source, sink)]
    for permutation in itertools.permutations(middle):
        order = (source, *permutation, sink)
        rank = [0] * n
        for index, vertex in enumerate(order):
            rank[vertex] = index
        valid = True
        for vertex in range(n):
            if vertex != source and not any(
                rank[to] < rank[vertex] for to in adjacent[vertex]
            ):
                valid = False
            if vertex != sink and not any(
                rank[vertex] < rank[to] for to in adjacent[vertex]
            ):
                valid = False
        if valid:
            return True
    return False


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]], int, int, bool | None]] = [
        (1, [], 0, 0, True),
        (4, [(0, 1), (1, 2), (2, 3)], 0, 3, True),
        (5, [(0, 2), (1, 2), (2, 3), (2, 4)], 0, 1, None),
        (4, [(0, 1), (2, 3)], 0, 1, False),
        (
            5,
            [(0, 1), (0, 1), (1, 2), (2, 3), (3, 4), (4, 0)],
            0,
            3,
            None,
        ),
    ]
    rng = random.Random(2026071611)
    for _ in range(18):
        n = rng.randrange(2, 9)
        source, sink = rng.sample(range(n), 2)
        edges: list[tuple[int, int]] = []
        for left in range(n):
            for right in range(left + 1, n):
                edges.extend([(left, right)] * rng.randrange(3))
        cases.append((n, edges, source, sink, None))

    large_n = 200000
    cases.append(
        (large_n, [(v, v + 1) for v in range(large_n - 1)], 0, large_n - 1, True)
    )

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, edges, source, sink, known in cases:
        input_lines.append(f"{n} {len(edges)} {source} {sink}")
        input_lines.extend(f"{left} {right}" for left, right in edges)
        answer = known if known is not None else exists(n, edges, source, sink)
        output_lines.append("YES" if answer else "NO")
    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
