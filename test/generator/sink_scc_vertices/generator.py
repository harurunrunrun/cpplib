#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]


def solve(vertex_count: int, edges: list[Edge]) -> list[int]:
    reachable = [
        [left == right for right in range(vertex_count)]
        for left in range(vertex_count)
    ]
    for left, right in edges:
        reachable[left][right] = True
    for middle in range(vertex_count):
        for left in range(vertex_count):
            if not reachable[left][middle]:
                continue
            for right in range(vertex_count):
                reachable[left][right] |= reachable[middle][right]
    return [
        vertex
        for vertex in range(vertex_count)
        if all(
            not reachable[vertex][other] or reachable[other][vertex]
            for other in range(vertex_count)
        )
    ]


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
        + " ".join(map(str, answer))
        + "\n",
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
    write_case(out_dir, "two_sinks", 7, [
        (0, 1), (1, 0), (1, 2), (2, 3), (3, 2),
        (0, 4), (4, 5), (5, 4), (6, 0),
    ])
    write_case(out_dir, "loops_and_duplicates", 4, [
        (0, 0), (0, 1), (0, 1), (1, 0), (1, 2),
        (2, 3), (3, 2), (3, 3),
    ])

    source = random.Random(0x51CC)
    for case_index in range(50):
        vertex_count = source.randrange(11)
        edges = [
            (left, right)
            for left in range(vertex_count)
            for right in range(vertex_count)
            if source.randrange(5) == 0
        ]
        write_case(
            out_dir,
            f"random_{case_index:02d}",
            vertex_count,
            edges,
        )


if __name__ == "__main__":
    main()
