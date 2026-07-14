#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def subtree_vertices(n, edges):
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    parent = [-1] * n
    parent[0] = 0
    order = [0]
    for v in order:
        for to in graph[v]:
            if parent[to] == -1:
                parent[to] = v
                order.append(to)
    children = [[] for _ in range(n)]
    for v in range(1, n):
        children[parent[v]].append(v)
    result = []
    for root in range(n):
        vertices = []
        stack = [root]
        while stack:
            v = stack.pop()
            vertices.append(v)
            stack.extend(children[v])
        result.append(vertices)
    return result


def write_case(out_dir, index, labels, edges, queries):
    subtrees = subtree_vertices(len(labels), edges)
    answer = []
    for root, order in queries:
        ranked = sorted(subtrees[root], key=lambda v: (labels[v], v))
        answer.append(ranked[order - 1] + 1)
    lines = [str(len(labels)), " ".join(map(str, labels))]
    lines += [f"{u + 1} {v + 1}" for u, v in edges]
    lines += [str(len(queries))] + [f"{root + 1} {order}" for root, order in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([7], [], [(0, 1)]),
             ([4, 1, 1, 9], [(0, 1), (0, 2), (2, 3)],
              [(0, 1), (0, 4), (2, 1), (2, 2), (3, 1)])]
    rng = random.Random(730003)
    for n in range(2, 16):
        edges = [(v, rng.randrange(v)) for v in range(1, n)]
        labels = [rng.randint(-5, 8) for _ in range(n)]
        subtrees = subtree_vertices(n, edges)
        queries = []
        for _ in range(20):
            root = rng.randrange(n)
            queries.append((root, rng.randint(1, len(subtrees[root]))))
        cases.append((labels, edges, queries))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
