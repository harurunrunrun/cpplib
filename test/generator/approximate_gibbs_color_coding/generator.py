#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def has_simple_path(graph: list[list[int]], length: int) -> bool:
    if length == 0:
        return True

    def visit(vertex: int, used: int, count: int) -> bool:
        if count == length:
            return True
        for to in graph[vertex]:
            if used >> to & 1 == 0 and visit(to, used | 1 << to, count + 1):
                return True
        return False

    return any(visit(vertex, 1 << vertex, 1) for vertex in range(len(graph)))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[list[list[int]], int]] = []
    chain = [[1], [2], [3], [4], [5], []]
    cases.append((chain, 6))
    cases.append(([[1, 2], [3], [3], [4], []], 4))
    cases.append(([[1], [], [3], [], []], 3))
    complete = [[to for to in range(6) if to != vertex] for vertex in range(6)]
    cases.append((complete, 5))
    cases.append(([[], [], []], 1))
    cases.append(([[]], 0))

    lines = [str(len(cases))]
    found_count = 0
    for graph, length in cases:
        expected = has_simple_path(graph, length)
        found_count += int(expected)
        edges = [(u, v) for u, adjacency in enumerate(graph) for v in adjacency]
        lines.append(f"{len(graph)} {len(edges)} {length} {int(expected)}")
        lines.extend(f"{u} {v}" for u, v in edges)
    (args.out_dir / "cases.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        f"OK {len(cases)} {found_count}\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
