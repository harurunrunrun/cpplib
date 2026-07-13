#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def connected_without(
    n: int,
    edges: list[tuple[int, int]],
    source: int,
    sink: int,
    removed: frozenset[int],
) -> bool:
    graph = [[] for _ in range(n)]
    for index, (left, right) in enumerate(edges):
        if index in removed:
            continue
        graph[left].append(right)
        graph[right].append(left)
    reached = {source}
    stack = [source]
    while stack:
        vertex = stack.pop()
        for to in graph[vertex]:
            if to not in reached:
                reached.add(to)
                stack.append(to)
    return sink in reached


def groups_naive(
    n: int, edges: list[tuple[int, int]]
) -> list[list[int]]:
    parent = list(range(n))

    def leader(vertex: int) -> int:
        while parent[vertex] != vertex:
            parent[vertex] = parent[parent[vertex]]
            vertex = parent[vertex]
        return vertex

    def merge(left: int, right: int) -> None:
        left = leader(left)
        right = leader(right)
        if left != right:
            parent[right] = left

    removals = [frozenset()]
    removals.extend(frozenset([edge]) for edge in range(len(edges)))
    removals.extend(
        frozenset(pair) for pair in itertools.combinations(range(len(edges)), 2)
    )
    for left in range(n):
        for right in range(left + 1, n):
            if all(
                connected_without(n, edges, left, right, removed)
                for removed in removals
            ):
                merge(left, right)
    result: dict[int, list[int]] = {}
    for vertex in range(n):
        result.setdefault(leader(vertex), []).append(vertex)
    return sorted(sorted(group) for group in result.values())


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    known: list[list[int]] | None = None,
) -> None:
    input_lines = [f"{n} {len(edges)}"]
    input_lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    groups = known if known is not None else groups_naive(n, edges)
    output_lines = [str(len(groups))]
    output_lines.extend(
        f"{len(group)} " + " ".join(map(str, group)) for group in groups
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [(0, 0)])
    write_case(out_dir, 1, 2, [(0, 1), (0, 1)])
    write_case(out_dir, 2, 2, [(0, 1), (0, 1), (0, 1)])
    write_case(
        out_dir,
        3,
        4,
        [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)],
    )
    rng = random.Random(2026071712)
    for index in range(4, 12):
        n = rng.randrange(2, 9)
        edges = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(rng.randrange(1, 13))
        ]
        write_case(out_dir, index, n, edges)

    cycle_size = 200000
    cycle = [(vertex, vertex + 1) for vertex in range(cycle_size - 1)]
    cycle.append((cycle_size - 1, 0))
    write_case(
        out_dir,
        12,
        cycle_size,
        cycle,
        [[vertex] for vertex in range(cycle_size)],
    )

    path_size = 66667
    triple_path = [
        (vertex, vertex + 1)
        for vertex in range(path_size - 1)
        for _ in range(3)
    ]
    write_case(
        out_dir,
        13,
        path_size,
        triple_path,
        [list(range(path_size))],
    )


if __name__ == "__main__":
    main()
