#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def brute(n: int, edges: list[tuple[int, int]]) -> list[int]:
    answer = [0] * len(edges)
    for chosen in itertools.combinations(range(len(edges)), 4):
        degree = [0] * n
        graph = [[] for _ in range(n)]
        for edge in chosen:
            left, right = edges[edge]
            degree[left] += 1
            degree[right] += 1
            graph[left].append(right)
            graph[right].append(left)
        vertices = [v for v in range(n) if degree[v]]
        if len(vertices) != 4 or any(degree[v] != 2 for v in vertices):
            continue
        reached = {vertices[0]}
        stack = [vertices[0]]
        while stack:
            vertex = stack.pop()
            for to in graph[vertex]:
                if to not in reached:
                    reached.add(to)
                    stack.append(to)
        if len(reached) != 4:
            continue
        for edge in chosen:
            answer[edge] += 1
    return answer


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    known: list[int] | None = None,
) -> None:
    input_lines = [f"{n} {len(edges)}"]
    input_lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    answer = known if known is not None else brute(n, edges)
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 0, [])
    write_case(out_dir, 1, 4, [(0, 1), (1, 2), (2, 3), (3, 0)])
    parallel_square = (
        [(0, 1)] * 2
        + [(1, 2)] * 3
        + [(2, 3)] * 4
        + [(3, 0)] * 5
    )
    write_case(out_dir, 2, 4, parallel_square)

    rng = random.Random(2026071613)
    for index in range(3, 10):
        n = 8
        edges: list[tuple[int, int]] = []
        for _ in range(15):
            left, right = rng.sample(range(n), 2)
            edges.append((left, right))
        write_case(out_dir, index, n, edges)

    right_count = 3000
    bipartite = [
        edge
        for vertex in range(right_count)
        for edge in ((0, vertex + 2), (1, vertex + 2))
    ]
    write_case(
        out_dir,
        10,
        right_count + 2,
        bipartite,
        [right_count - 1] * len(bipartite),
    )

    multiplicity = 75000
    large_square = (
        [(0, 1)] * multiplicity
        + [(1, 2)] * multiplicity
        + [(2, 3)] * multiplicity
        + [(3, 0)] * multiplicity
    )
    write_case(
        out_dir,
        11,
        4,
        large_square,
        [multiplicity**3] * len(large_square),
    )


if __name__ == "__main__":
    main()
