#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def shortest_paths(matrix: list[list[int]], source: int) -> list[int]:
    vertex_count = len(matrix)
    distance = [10**30] * vertex_count
    used = [False] * vertex_count
    distance[source] = 0
    for _ in range(vertex_count):
        vertex = min(
            (index for index in range(vertex_count) if not used[index]),
            key=distance.__getitem__,
        )
        used[vertex] = True
        for target, cost in enumerate(matrix[vertex]):
            distance[target] = min(
                distance[target], distance[vertex] + cost
            )
    return distance


def write_case(
    out_dir: Path,
    index: int,
    matrix: list[list[int]],
    queries: list[tuple[int, int]],
) -> None:
    cache: dict[int, list[int]] = {}
    answer = 0
    for source, target in queries:
        if source not in cache:
            cache[source] = shortest_paths(matrix, source)
        answer += cache[source][target]

    name = f"case_{index:03d}"
    input_lines = [
        f"{len(matrix)} {len(queries)}",
        *(" ".join(map(str, row)) for row in matrix),
        *(f"{source} {target}" for source, target in queries),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [[0]], [(0, 0), (0, 0)])
    write_case(
        out_dir,
        1,
        [[0, 1, 1], [1, 0, 1], [1, 9, 0]],
        [(1, 0), (2, 1)],
    )

    rng = random.Random(11451419)
    for index in range(2, 42):
        vertex_count = rng.randint(1, 12)
        matrix = [
            [
                0 if source == target else rng.randint(1, 200)
                for target in range(vertex_count)
            ]
            for source in range(vertex_count)
        ]
        queries = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(rng.randint(1, 80))
        ]
        write_case(out_dir, index, matrix, queries)


if __name__ == "__main__":
    main()
