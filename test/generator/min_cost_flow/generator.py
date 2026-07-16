#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(n: int, source: int, sink: int, edges: list[tuple[int, int, int, int]]) -> tuple[int, int]:
    flow = [0] * len(edges)
    best_flow = -1
    best_cost = 0

    def dfs(i: int) -> None:
        nonlocal best_flow, best_cost
        if i == len(edges):
            balance = [0] * n
            cost = 0
            for f, (u, v, cap, c) in zip(flow, edges):
                balance[u] -= f
                balance[v] += f
                cost += f * c
            for v in range(n):
                if v not in (source, sink) and balance[v] != 0:
                    return
            sent = -balance[source]
            if balance[sink] != sent:
                return
            if best_flow < sent or (best_flow == sent and cost < best_cost):
                best_flow = sent
                best_cost = cost
            return
        for f in range(edges[i][2] + 1):
            flow[i] = f
            dfs(i + 1)

    dfs(0)
    return best_flow, best_cost


def write_case(out_dir: Path, idx: int, n: int, s: int, t: int, edges: list[tuple[int, int, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {s} {t}", *[f"{u} {v} {cap} {cost}" for u, v, cap, cost in edges]]) + "\n",
        encoding="utf-8",
    )
    flow, cost = solve(n, s, t, edges)
    (out_dir / f"{name}.out").write_text(f"{flow} {cost}\n", encoding="utf-8")


def write_self_test_case(out_dir: Path, idx: int) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text("", encoding="utf-8")
    (out_dir / f"{name}.out").write_text("", encoding="utf-8")


def write_deep_paths_case(
    out_dir: Path,
    idx: int,
    width: int,
    depth: int,
) -> None:
    sink = 0
    source = width * depth + 1
    vertex_count = source + 1
    edge_count = width * (depth + 1)
    name = f"case_{idx:02d}"
    with (out_dir / f"{name}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file:
        input_file.write(
            f"{vertex_count} {edge_count} {source} {sink}\n"
        )
        for path in range(width):
            previous = source
            base = 1 + path * depth
            for offset in range(depth):
                vertex = base + depth - 1 - offset
                input_file.write(f"{previous} {vertex} 1 0\n")
                previous = vertex
            input_file.write(f"{previous} {sink} 1 0\n")
    (out_dir / f"{name}.out").write_text(
        f"{width} 0\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, int, list[tuple[int, int, int, int]]]] = [
        (2, 0, 1, []),
        (2, 0, 1, [(0, 1, 1, 3000000000000000000)]),
        (4, 0, 3, [(0, 1, 2, 1), (0, 2, 1, 5), (1, 2, 1, -2), (1, 3, 1, 2), (2, 3, 2, 1)]),
    ]
    rng = random.Random(20260827)
    for n in [2, 4, 5]:
        edges: list[tuple[int, int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u < v and rng.randrange(4) == 0 and len(edges) < 8:
                    edges.append((u, v, rng.randrange(4), rng.randrange(11) - 5))
        cases.append((n, 0, n - 1, edges))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)
    write_self_test_case(out_dir, len(cases))
    write_deep_paths_case(out_dir, len(cases) + 1, 180, 180)


if __name__ == "__main__":
    main()
