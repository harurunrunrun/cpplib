#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]


def solve(vertex_count: int, edges: list[Edge]) -> list[Edge]:
    result: list[Edge] = []
    for removed, (source, target) in enumerate(edges):
        if source == target:
            continue
        graph: list[list[int]] = [[] for _ in range(vertex_count)]
        for index, (left, right) in enumerate(edges):
            if index == removed:
                continue
            graph[left].append(right)
            graph[right].append(left)
        seen = [False] * vertex_count
        seen[source] = True
        stack = [source]
        while stack:
            vertex = stack.pop()
            for to in graph[vertex]:
                if not seen[to]:
                    seen[to] = True
                    stack.append(to)
        if not seen[target]:
            result.append((min(source, target), max(source, target)))
    result.sort()
    return result


def write_case(
    out_dir: Path,
    name: str,
    vertex_count: int,
    edges: list[Edge],
) -> None:
    answer = solve(vertex_count, edges)
    (out_dir / f"{name}.in").write_text(
        "\n".join([
            f"{vertex_count} {len(edges)}",
            *(f"{left} {right}" for left, right in edges),
        ])
        + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        f"{len(answer)}\n"
        + "".join(f"{left} {right}\n" for left, right in answer),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "empty", 0, [])
    write_case(out_dir, "single", 1, [])
    write_case(out_dir, "chain", 5, [
        (0, 1), (1, 2), (2, 3), (3, 4),
    ])
    write_case(out_dir, "cycle_and_tail", 6, [
        (0, 1), (1, 2), (2, 0), (2, 3), (3, 4), (4, 5), (5, 3),
    ])
    write_case(out_dir, "parallel_and_loops", 5, [
        (0, 0), (0, 1), (0, 1), (1, 2), (2, 2), (3, 4),
    ])

    source = random.Random(0xB21D63)
    for case_index in range(60):
        vertex_count = source.randrange(10)
        edge_count = source.randrange(25) if vertex_count else 0
        edges = [
            (
                source.randrange(vertex_count),
                source.randrange(vertex_count),
            )
            for _ in range(edge_count)
        ]
        write_case(
            out_dir,
            f"random_{case_index:02d}",
            vertex_count,
            edges,
        )


if __name__ == "__main__":
    main()
