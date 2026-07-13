#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int, int]],
    expected_size: int,
    expected_weight: int,
) -> None:
    name = f"case_{index:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)}", *[f"{u} {v} {weight}" for u, v, weight in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        f"{expected_size} {expected_weight}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], int, int]] = [
        (0, [], 0, 0),
        (4, [(0, 1, 100), (0, 2, 60), (1, 3, 1)], 1, 100),
        (4, [(0, 1, 3), (1, 0, 7), (2, 3, 5), (0, 0, 100)], 2, 12),
        (3, [(0, 1, -1), (1, 2, -2), (2, 0, -3)], 0, 0),
        (5, [(0, 1, 5), (1, 2, 5), (2, 0, 5), (3, 4, 2)], 2, 7),
    ]

    n = 120
    preferred = {(vertex, vertex + 1): 10**12 + vertex for vertex in range(0, n, 2)}
    complete_edges = [
        (u, v, preferred.get((u, v), 1))
        for u in range(n)
        for v in range(u + 1, n)
    ]
    cases.append((n, complete_edges, n // 2, sum(preferred.values())))

    for index, (n, edges, expected_size, expected_weight) in enumerate(cases):
        write_case(out_dir, index, n, edges, expected_size, expected_weight)


if __name__ == "__main__":
    main()
