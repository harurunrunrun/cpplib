#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def decompose(n: int, edges: list[tuple[int, int]]) -> tuple[int, list[int], list[int]]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)

    parent = [-1] * n
    depth = [-1] * n
    subtree_size = [0] * n
    component_parent = [-1] * n
    removed = [False] * n
    stack: list[tuple[int, int, int]] = [(0, -1, 0)]
    root = -1

    while stack:
        start, centroid_parent, centroid_depth = stack.pop()
        if removed[start]:
            continue
        nodes: list[int] = []
        component_stack = [start]
        component_parent[start] = -2
        while component_stack:
            vertex = component_stack.pop()
            nodes.append(vertex)
            for to in graph[vertex]:
                if removed[to] or to == component_parent[vertex]:
                    continue
                if component_parent[to] != -1:
                    raise RuntimeError("input is not a tree")
                component_parent[to] = vertex
                component_stack.append(to)

        for vertex in reversed(nodes):
            subtree_size[vertex] = 1
            for to in graph[vertex]:
                if not removed[to] and component_parent[to] == vertex:
                    subtree_size[vertex] += subtree_size[to]

        centroid = nodes[0]
        for vertex in nodes:
            largest = len(nodes) - subtree_size[vertex]
            for to in graph[vertex]:
                if not removed[to] and component_parent[to] == vertex:
                    largest = max(largest, subtree_size[to])
            if largest * 2 <= len(nodes):
                centroid = vertex
                break

        removed[centroid] = True
        parent[centroid] = centroid_parent
        depth[centroid] = centroid_depth
        if centroid_parent == -1:
            root = centroid
        for to in graph[centroid]:
            if not removed[to]:
                stack.append((to, centroid, centroid_depth + 1))
        for vertex in nodes:
            component_parent[vertex] = -1

    return root, parent, depth


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (1, []),
        (7, [(i, i + 1) for i in range(6)]),
        (8, [(0, i) for i in range(1, 8)]),
        (7, [(0, 1), (0, 2), (1, 3), (1, 4), (2, 5), (2, 6)]),
    ]
    rng = random.Random(20260713)
    for n in list(range(2, 20)) + [31, 64, 127, 200]:
        edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, n)]
        rng.shuffle(edges)
        cases.append((n, edges))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for n, edges in cases:
        lines.append(str(n))
        lines.extend(f"{u} {v}" for u, v in edges)
        root, parent, depth = decompose(n, edges)
        outputs.append(str(root))
        outputs.append(" ".join(map(str, parent)))
        outputs.append(" ".join(map(str, depth)))

    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
