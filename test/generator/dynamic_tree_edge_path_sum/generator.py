#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def path_sum(n, edges, weights, source, target):
    graph = [[] for _ in range(n)]
    for edge, (u, v) in enumerate(edges):
        graph[u].append((v, edge))
        graph[v].append((u, edge))
    parent = [None] * n
    parent[source] = (source, -1)
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        for to, edge in graph[vertex]:
            if parent[to] is None:
                parent[to] = (vertex, edge)
                queue.append(to)
    result = 0
    vertex = target
    while vertex != source:
        vertex, edge = parent[vertex]
        result += weights[edge]
    return result


def write_case(out_dir, index, n, weighted_edges, root, operations):
    edges = [(u, v) for u, v, _ in weighted_edges]
    weights = [weight for _, _, weight in weighted_edges]
    answer = []
    for kind, first, second in operations:
        if kind == 0:
            weights[first] = second
        else:
            answer.append(path_sum(n, edges, weights, first, second))
    lines = [f"{n} {len(operations)} {root}"]
    lines.extend(f"{u} {v} {weight}" for u, v, weight in weighted_edges)
    lines.extend(f"{kind} {first} {second}" for kind, first, second in operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answer)) + ("\n" if answer else ""), encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (1, [], 0, [(1, 0, 0)]),
        (2, [(0, 1, -7)], 1, [(1, 0, 1), (0, 0, 9), (1, 1, 0)]),
    ]
    rng = random.Random(2940701)
    for n in range(2, 30):
        edges = [(vertex, rng.randrange(vertex), rng.randint(-10**5, 10**5))
                 for vertex in range(1, n)]
        operations = []
        for _ in range(120):
            if rng.randrange(3) == 0:
                operations.append((0, rng.randrange(n - 1), rng.randint(-10**5, 10**5)))
            else:
                operations.append((1, rng.randrange(n), rng.randrange(n)))
        cases.append((n, edges, rng.randrange(n), operations))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
