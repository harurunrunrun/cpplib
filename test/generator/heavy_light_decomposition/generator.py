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
    n = 200
    root = 137
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
    subtree = [1] * n
    for v in reversed(order[1:]):
        subtree[parent[v]] += subtree[v]

    def lca(u: int, v: int) -> int:
        while depth[u] > depth[v]:
            u = parent[u]
        while depth[v] > depth[u]:
            v = parent[v]
        while u != v:
            u, v = parent[u], parent[v]
        return u

    def path_vertices(u: int, v: int, include_vertices: bool) -> list[int]:
        ancestor = lca(u, v)
        result: list[int] = []
        x = u
        while x != ancestor:
            result.append(x)
            x = parent[x]
        y_path: list[int] = []
        x = v
        while x != ancestor:
            y_path.append(x)
            x = parent[x]
        if include_vertices:
            result.append(ancestor)
        result.extend(reversed(y_path))
        return sorted(result)

    commands: list[str] = []
    outputs: list[str] = []
    kinds = ["LCA", "DIST", "JUMP", "ANCESTOR", "PARENT", "DEPTH", "SUBSIZE", "PATH", "SUBTREE"]
    for _ in range(1500):
        kind = rng.choice(kinds)
        if kind in {"LCA", "DIST", "ANCESTOR"}:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"{kind} {u} {v}")
            ancestor = lca(u, v)
            answer = ancestor if kind == "LCA" else depth[u] + depth[v] - 2 * depth[ancestor] if kind == "DIST" else int(lca(u, v) == u)
            outputs.append(str(answer))
        elif kind == "JUMP":
            v = rng.randrange(n)
            k = rng.randrange(depth[v] + 1)
            commands.append(f"JUMP {v} {k}")
            x = v
            for _ in range(k):
                x = parent[x]
            outputs.append(str(x))
        elif kind in {"PARENT", "DEPTH", "SUBSIZE"}:
            v = rng.randrange(n)
            commands.append(f"{kind} {v}")
            outputs.append(str(parent[v] if kind == "PARENT" else depth[v] if kind == "DEPTH" else subtree[v]))
        elif kind == "PATH":
            u, v = rng.randrange(n), rng.randrange(n)
            include = rng.randrange(2)
            vertices = path_vertices(u, v, bool(include))
            commands.append(f"PATH {u} {v} {include}")
            outputs.append(" ".join(map(str, [len(vertices), *vertices])))
        else:
            v = rng.randrange(n)
            include = rng.randrange(2)
            vertices = [x for x in range(n) if lca(v, x) == v and (include or x != v)]
            vertices.sort()
            commands.append(f"SUBTREE {v} {include}")
            outputs.append(" ".join(map(str, [len(vertices), *vertices])))

    lines = [f"{n} {root} {len(commands)}"]
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
