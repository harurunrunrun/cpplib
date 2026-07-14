#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def has_cycle(vertex_count: int, edges: list[tuple[int, int]]) -> bool:
    parent = list(range(vertex_count))

    def root(vertex: int) -> int:
        while parent[vertex] != vertex:
            parent[vertex] = parent[parent[vertex]]
            vertex = parent[vertex]
        return vertex

    for left, right in edges:
        left_root = root(left)
        right_root = root(right)
        if left_root == right_root:
            return True
        parent[left_root] = right_root
    return False


def write_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int]],
) -> None:
    name = f"case_{index:03d}"
    lines = [f"{vertex_count} {len(edges)}"]
    lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        f"{int(has_cycle(vertex_count, edges))}\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        (1, []),
        (4, [(0, 1), (1, 2), (2, 3)]),
        (3, [(0, 1), (1, 2), (2, 0)]),
        (1, [(0, 0)]),
        (2, [(0, 1), (0, 1)]),
    ]
    for index, (vertex_count, edges) in enumerate(fixed):
        write_case(out_dir, index, vertex_count, edges)

    rng = random.Random(417091)
    for index in range(len(fixed), 65):
        vertex_count = rng.randint(1, 14)
        edge_count = rng.randint(0, 32)
        edges = [
            (rng.randrange(vertex_count), rng.randrange(vertex_count))
            for _ in range(edge_count)
        ]
        write_case(out_dir, index, vertex_count, edges)


if __name__ == "__main__":
    main()
