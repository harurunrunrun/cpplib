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
    n = 180
    parents = [-1] + [rng.randrange(v) for v in range(1, n)]
    graph = [[] for _ in range(n)]
    for v in range(1, n):
        graph[v].append(parents[v])
        graph[parents[v]].append(v)

    parent = [-2] * n
    depth = [0] * n
    tin = [-1] * n
    tout = [-1] * n
    timer = 0
    stack = [(0, -1, 0)]
    while stack:
        v, p, phase = stack.pop()
        if phase == 0:
            parent[v] = p
            tin[v] = timer
            timer += 1
            stack.append((v, p, 1))
            for to in reversed(graph[v]):
                if to == p:
                    continue
                depth[to] = depth[v] + 1
                stack.append((to, v, 0))
        else:
            tout[v] = timer

    log = n.bit_length()
    up = [[0] * n for _ in range(log)]
    for v in range(n):
        up[0][v] = v if parent[v] == -1 else parent[v]
    for level in range(1, log):
        for v in range(n):
            up[level][v] = up[level - 1][up[level - 1][v]]

    def ancestor(u: int, v: int) -> bool:
        return tin[u] <= tin[v] < tout[u]

    def lca(u: int, v: int) -> int:
        if ancestor(u, v):
            return u
        if ancestor(v, u):
            return v
        for level in reversed(range(log)):
            if not ancestor(up[level][u], v):
                u = up[level][u]
        return up[0][u]

    queries: list[list[int]] = [[], [0], [n - 1, n - 1], list(range(n))]
    for _ in range(160):
        queries.append([rng.randrange(n) for _ in range(rng.randrange(0, 80))])

    output: list[str] = []
    for query in queries:
        selected = sorted(set(query), key=tin.__getitem__)
        if not selected:
            output.append("0")
            continue
        vertices = selected + [lca(u, v) for u, v in zip(selected, selected[1:])]
        vertices = sorted(set(vertices), key=tin.__getitem__)
        selected_set = set(selected)
        output.append(str(len(vertices)))
        active: list[int] = []
        for v in vertices:
            while active and not ancestor(active[-1], v):
                active.pop()
            p = active[-1] if active else -1
            length = 0 if p == -1 else depth[v] - depth[p]
            output.append(f"{v} {p} {length} {int(v in selected_set)}")
            active.append(v)

    input_lines = [f"{n} {len(queries)}"]
    input_lines += [f"{parents[v]} {v}" for v in range(1, n)]
    input_lines += [f"{len(query)}" + (" " + " ".join(map(str, query)) if query else "") for query in queries]
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
