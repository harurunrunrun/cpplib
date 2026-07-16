#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import functools
import random
from pathlib import Path


def solve(vertex_count: int, edges: list[tuple[int, int, int]]) -> int | None:
    infinity = 10**30
    distance = [[infinity] * vertex_count for _ in range(vertex_count)]
    degree = [0] * vertex_count
    total = 0
    for vertex in range(vertex_count):
        distance[vertex][vertex] = 0
    for left, right, cost in edges:
        distance[left][right] = min(distance[left][right], cost)
        distance[right][left] = min(distance[right][left], cost)
        degree[left] += 1
        degree[right] += 1
        total += cost
    graph = [[] for _ in range(vertex_count)]
    for left, right, _ in edges:
        graph[left].append(right)
        graph[right].append(left)
    nonisolated = [vertex for vertex in range(vertex_count) if degree[vertex]]
    if nonisolated:
        reached = {nonisolated[0]}
        queue = collections.deque([nonisolated[0]])
        while queue:
            vertex = queue.popleft()
            for next_vertex in graph[vertex]:
                if next_vertex not in reached:
                    reached.add(next_vertex)
                    queue.append(next_vertex)
        if any(vertex not in reached for vertex in nonisolated):
            return None
    for middle in range(vertex_count):
        for left in range(vertex_count):
            for right in range(vertex_count):
                distance[left][right] = min(
                    distance[left][right],
                    distance[left][middle] + distance[middle][right],
                )
    odd = [vertex for vertex in range(vertex_count) if degree[vertex] % 2]

    @functools.cache
    def pairing(mask: int) -> int:
        if mask == (1 << len(odd)) - 1:
            return 0
        first = next(index for index in range(len(odd)) if not mask >> index & 1)
        return min(
            distance[odd[first]][odd[second]]
            + pairing(mask | (1 << first) | (1 << second))
            for second in range(first + 1, len(odd))
            if not mask >> second & 1
        )

    return total + pairing(0)


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
) -> None:
    lines = [f"{vertex_count} {len(edges)}"]
    lines.extend(f"{left} {right} {cost}" for left, right, cost in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    answer = solve(vertex_count, edges)
    expected = "NONE" if answer is None else str(answer)
    (out_dir / f"case_{index:02d}.out").write_text(f"{expected}\n", encoding="utf-8")


def write_sparse_path_case(out_dir: Path, index: int, vertex_count: int) -> None:
    cost = 10**9
    input_path = out_dir / f"case_{index:02d}.in"
    with input_path.open("w", encoding="utf-8") as stream:
        stream.write(f"{vertex_count} {vertex_count - 1}\n")
        for vertex in range(1, vertex_count):
            stream.write(f"{vertex - 1} {vertex} {cost}\n")
    expected = 2 * (vertex_count - 1) * cost
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{expected}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (1, []),
        (4, [(0, 1, 1), (0, 2, 2), (1, 3, 3), (2, 3, 4)]),
        (4, [(0, 1, 1), (0, 2, 2), (1, 3, 3), (2, 3, 4), (1, 2, 5)]),
        (2, [(0, 1, 1), (0, 1, 2), (0, 1, 3)]),
        (6, [(0, 1, 2), (1, 2, 3), (3, 4, 5), (4, 5, 7)]),
    ]
    rng = random.Random(2026071313)
    for _ in range(30):
        vertex_count = rng.randrange(2, 11)
        edges: list[tuple[int, int, int]] = []
        for vertex in range(1, vertex_count):
            edges.append((vertex, rng.randrange(vertex), rng.randrange(0, 101)))
        for left in range(vertex_count):
            for right in range(left + 1, vertex_count):
                if rng.randrange(5) == 0:
                    edges.append((left, right, rng.randrange(0, 101)))
        cases.append((vertex_count, edges))

    for index, (vertex_count, edges) in enumerate(cases):
        write_case(args.out_dir, index, vertex_count, edges)
    write_sparse_path_case(args.out_dir, len(cases), 100000)


if __name__ == "__main__":
    main()
