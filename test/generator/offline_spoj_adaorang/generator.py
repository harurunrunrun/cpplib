#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def build_case(n, q, root, rng):
    colors = [rng.randrange(1, 6) for _ in range(n)]
    edges = [(rng.randrange(v), v) for v in range(1, n)]
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    parent = [-2] * n
    parent[root] = -1
    order = [root]
    for u in order:
        for v in graph[u]:
            if v != parent[u]:
                parent[v] = u
                order.append(v)
    children = [[] for _ in range(n)]
    for v in range(n):
        if parent[v] >= 0:
            children[parent[v]].append(v)

    def lca(u, v):
        ancestors = set()
        while u >= 0:
            ancestors.add(u)
            u = parent[u]
        while v not in ancestors:
            v = parent[v]
        return v

    def subtree_vertices(start):
        vertices = []
        stack = [start]
        while stack:
            u = stack.pop()
            vertices.append(u)
            stack.extend(children[u])
        return vertices

    queries = [(rng.randrange(n), rng.randrange(n)) for _ in range(q)]
    answers = []
    for u, v in queries:
        ancestor = lca(u, v)
        answers.append(len({colors[x] for x in subtree_vertices(ancestor)}))
    lines = [f"{n} {q} {root}", " ".join(map(str, colors))]
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.extend(f"{u} {v}" for u, v in queries)
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76001)
    cases = [build_case(1, 5, 0, rng), build_case(17, 80, 7, rng)]
    input_lines = [str(len(cases))]
    output_lines = []
    for lines, answers in cases:
        input_lines.extend(lines)
        output_lines.extend(map(str, answers))
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
