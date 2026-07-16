#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    name: str,
    n: int,
    root: int,
    edges: list[tuple[int, int]],
    queries: list[tuple[int, int]],
) -> None:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)

    parent = [-2] * n
    depth = [0] * n
    parent[root] = -1
    stack = [root]
    while stack:
        v = stack.pop()
        for to in graph[v]:
            if to == parent[v]:
                continue
            if parent[to] != -2:
                raise AssertionError("generator produced a non-tree")
            parent[to] = v
            depth[to] = depth[v] + 1
            stack.append(to)

    def naive_lca(u: int, v: int) -> int:
        while depth[u] > depth[v]:
            u = parent[u]
        while depth[v] > depth[u]:
            v = parent[v]
        while u != v:
            u, v = parent[u], parent[v]
        return u

    lines = [f"{n} {root} {len(queries)}"]
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.extend(f"{u} {v}" for u, v in queries)
    outputs = []
    for u, v in queries:
        ancestor = naive_lca(u, v)
        distance = depth[u] + depth[v] - 2 * depth[ancestor]
        outputs.append(f"{ancestor} {distance} {ancestor} {distance + 1}")
    (out_dir / f"{name}.in").write_text(
        "\n".join(lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(outputs) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n, root = 257, 193
    edges = [(v, rng.randrange(v)) for v in range(1, n)]
    rng.shuffle(edges)
    queries = [(rng.randrange(n), rng.randrange(n)) for _ in range(1800)]
    write_case(out_dir, "case_00", n, root, edges, queries)

    for case_id in range(1, 7):
        n = rng.randrange(1, 801)
        root = rng.randrange(n)
        edges = [(v, rng.randrange(v)) for v in range(1, n)]
        rng.shuffle(edges)
        queries = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(1200)
        ]
        write_case(
            out_dir, f"case_{case_id:02d}", n, root, edges, queries
        )

    n = 200_000
    query_count = 300_000
    edges = [(v - 1, v) for v in range(1, n)]
    queries = [
        (
            (index * 1_000_003 + 97) % n,
            (index * 1_000_033 + 193) % n,
        )
        for index in range(query_count)
    ]
    lines = [f"{n} 0 {query_count}"]
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.extend(f"{u} {v}" for u, v in queries)
    outputs = []
    for u, v in queries:
        ancestor = min(u, v)
        distance = abs(u - v)
        outputs.append(
            f"{ancestor} {distance} {ancestor} {distance + 1}"
        )
    (out_dir / "case_07_large_path.in").write_text(
        "\n".join(lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_07_large_path.out").write_text(
        "\n".join(outputs) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
