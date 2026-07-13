#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def complement_components(n: int, edges: list[tuple[int, int]]) -> list[list[int]]:
    adjacent = [set() for _ in range(n)]
    for left, right in edges:
        adjacent[left].add(right)
        adjacent[right].add(left)
    remaining = set(range(n))
    groups: list[list[int]] = []
    while remaining:
        start = next(iter(remaining))
        remaining.remove(start)
        queue = deque([start])
        group: list[int] = []
        while queue:
            vertex = queue.popleft()
            group.append(vertex)
            reached = [candidate for candidate in remaining if candidate not in adjacent[vertex]]
            for candidate in reached:
                remaining.remove(candidate)
                queue.append(candidate)
        groups.append(sorted(group))
    return sorted(groups)


def write_case(out_dir: Path, index: int, n: int, edges: list[tuple[int, int]]) -> None:
    groups = complement_components(n, edges)
    lines = [f"{n} {len(edges)}"]
    lines.extend(f"{left} {right}" for left, right in edges)
    output = [str(len(groups))]
    output.extend(f"{len(group)} " + " ".join(map(str, group)) for group in groups)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_edges(seed: int, n: int, probability: int) -> list[tuple[int, int]]:
    rng = random.Random(seed)
    return [
        (left, right)
        for left in range(n)
        for right in range(left + 1, n)
        if rng.randrange(100) < probability
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 8, [])
    write_case(out_dir, 1, 12, [(left, right) for left in range(12) for right in range(left + 1, 12)])
    write_case(out_dir, 2, 70, random_edges(2026071431, 70, 72))
    write_case(out_dir, 3, 180, random_edges(2026071432, 180, 35))
    write_case(out_dir, 4, 900, random_edges(2026071433, 900, 97))


if __name__ == "__main__":
    main()
