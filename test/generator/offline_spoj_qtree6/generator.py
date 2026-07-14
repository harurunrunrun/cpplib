#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def graph_of(n, edges):
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    return graph


def component(graph, colors, root):
    wanted = colors[root]
    seen = {root}
    stack = [root]
    while stack:
        v = stack.pop()
        for to in graph[v]:
            if to not in seen and colors[to] == wanted:
                seen.add(to)
                stack.append(to)
    return seen


def write_case(out_dir, index, n, edges, operations):
    graph = graph_of(n, edges)
    colors = [False] * n
    output = []
    for operation, vertex in operations:
        if operation == 0:
            output.append(len(component(graph, colors, vertex)))
        else:
            colors[vertex] = not colors[vertex]
    lines = [str(n)] + [f"{u + 1} {v + 1}" for u, v in edges]
    lines += [str(len(operations))]
    lines += [f"{operation} {vertex + 1}" for operation, vertex in operations]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, output)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(1, [], [(0, 0), (1, 0), (0, 0)])]
    rng = random.Random(730006)
    for n in range(2, 17):
        edges = [(v, rng.randrange(v)) for v in range(1, n)]
        operations = []
        for _ in range(60):
            operation = 0 if rng.randrange(3) == 0 else 1
            operations.append((operation, rng.randrange(n)))
        cases.append((n, edges, operations))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
