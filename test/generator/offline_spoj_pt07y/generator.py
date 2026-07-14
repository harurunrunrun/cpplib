#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(vertex_count: int, edges: list[tuple[int, int]]) -> bool:
    if vertex_count == 0 or len(edges) != vertex_count - 1:
        return False
    graph = [[] for _ in range(vertex_count)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    seen = [False] * vertex_count
    seen[0] = True
    stack = [0]
    while stack:
        vertex = stack.pop()
        for next_vertex in graph[vertex]:
            if not seen[next_vertex]:
                seen[next_vertex] = True
                stack.append(next_vertex)
    return all(seen)


def write_case(out_dir: Path, name: str, vertex_count: int, edges: list[tuple[int, int]]) -> None:
    input_lines = [f"{vertex_count} {len(edges)}"]
    input_lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    answer = "YES" if solve(vertex_count, edges) else "NO"
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(answer + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "single", 1, [])
    write_case(out_dir, "path", 5, [(0, 1), (1, 2), (2, 3), (3, 4)])
    write_case(out_dir, "cycle", 4, [(0, 1), (1, 2), (2, 0), (2, 3)])
    write_case(out_dir, "disconnected", 4, [(0, 1), (2, 3)])
    write_case(out_dir, "parallel", 3, [(0, 1), (0, 1)])

    source = random.Random(139)
    for case_index in range(40):
        vertex_count = source.randint(1, 100)
        edge_count = source.randint(0, 180)
        edges = [
            (source.randrange(vertex_count), source.randrange(vertex_count))
            for _ in range(edge_count)
        ]
        write_case(out_dir, f"random_{case_index:02d}", vertex_count, edges)

    write_case(
        out_dir,
        "maximum",
        10000,
        [(index, index + 1) for index in range(9999)],
    )


if __name__ == "__main__":
    main()
