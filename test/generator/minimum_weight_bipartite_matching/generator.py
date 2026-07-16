#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(l: int, r: int, edges: list[tuple[int, int, int]]) -> tuple[int, int]:
    graph = [[] for _ in range(l)]
    for u, v, c in edges:
        graph[u].append((v, c))
    used = [False] * r
    best_size = 0
    best_cost = 0

    def dfs(v: int, size: int, cost: int) -> None:
        nonlocal best_size, best_cost
        if v == l:
            if best_size < size or (best_size == size and cost < best_cost):
                best_size = size
                best_cost = cost
            return
        dfs(v + 1, size, cost)
        for to, c in graph[v]:
            if not used[to]:
                used[to] = True
                dfs(v + 1, size + 1, cost + c)
                used[to] = False

    dfs(0, 0, 0)
    return best_size, best_cost


def write_case(out_dir: Path, idx: int, l: int, r: int, edges: list[tuple[int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{l} {r} {len(edges)}", *[f"{u} {v} {c}" for u, v, c in edges]]) + "\n",
        encoding="utf-8",
    )
    size, cost = solve(l, r, edges)
    (out_dir / f"{name}.out").write_text(f"{size} {cost}\n", encoding="utf-8")


def write_self_test_case(out_dir: Path, idx: int) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text("", encoding="utf-8")
    (out_dir / f"{name}.out").write_text("", encoding="utf-8")


def write_long_augmenting_path_case(
    out_dir: Path,
    idx: int,
    size: int,
) -> None:
    name = f"case_{idx:02d}"
    edge_count = 2 * size - 1
    with (out_dir / f"{name}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file:
        input_file.write(f"{size} {size} {edge_count}\n")
        for vertex in range(size):
            input_file.write(f"{vertex} {vertex} 1\n")
            if vertex != 0:
                input_file.write(f"{vertex} {vertex - 1} 0\n")
    (out_dir / f"{name}.out").write_text(
        f"{size} {size}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, list[tuple[int, int, int]]]] = [
        (0, 0, []),
        (2, 2, [(0, 0, 5), (0, 1, 1), (1, 0, 2), (1, 1, 10)]),
        (1, 1, [(0, 0, 3_000_000_000_000_000_000)]),
        (2, 2, [(0, 0, 3_000_000_000_000_000_000),
                (0, 1, 3_000_000_000_000_000_005),
                (1, 0, -2_000_000_000_000_000_000),
                (1, 1, 7)]),
    ]
    rng = random.Random(20260824)
    for l, r in [(1, 5), (5, 1), (5, 5), (8, 8)]:
        edges = [(i, j, rng.randrange(21) - 10) for i in range(l) for j in range(r) if rng.randrange(3) == 0]
        cases.append((l, r, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)
    write_self_test_case(out_dir, len(cases))
    write_long_augmenting_path_case(out_dir, len(cases) + 1, 1_500)


if __name__ == "__main__":
    main()
