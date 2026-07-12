#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


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
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    parent = [-1] * n
    depth = [0] * n
    order = [root]
    for v in order:
        for to in graph[v]:
            if to == parent[v]:
                continue
            parent[to] = v
            depth[to] = depth[v] + 1
            order.append(to)

    def lca(u: int, v: int) -> int:
        while depth[u] > depth[v]:
            u = parent[u]
        while depth[v] > depth[u]:
            v = parent[v]
        while u != v:
            u, v = parent[u], parent[v]
        return u

    queries = [(rng.randrange(n), rng.randrange(n)) for _ in range(1800)]
    lines = [f"{n} {root} {len(queries)}"]
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.extend(f"{u} {v}" for u, v in queries)
    outputs = []
    for u, v in queries:
        ancestor = lca(u, v)
        distance = depth[u] + depth[v] - 2 * depth[ancestor]
        outputs.append(f"{ancestor} {distance} {ancestor}")
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
