#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def path_sum(n, edges, weights, source, target):
    graph = [[] for _ in range(n)]
    for index, (u, v) in enumerate(edges):
        graph[u].append((v, index))
        graph[v].append((u, index))
    parent = [(-1, -1)] * n
    parent[source] = (source, -1)
    queue = collections.deque([source])
    while queue:
        v = queue.popleft()
        if v == target:
            break
        for to, edge in graph[v]:
            if parent[to][0] == -1:
                parent[to] = (v, edge)
                queue.append(to)
    answer = 0
    v = target
    while v != source:
        v, edge = parent[v]
        answer += weights[edge]
    return answer


def write_case(out_dir, index, n, weighted_edges, queries):
    edges = [(u, v) for u, v, _ in weighted_edges]
    weights = [weight for _, _, weight in weighted_edges]
    expected = []
    for query in queries:
        if query[0] == 1:
            weights[query[1]] = query[2]
        else:
            expected.append(path_sum(n, edges, weights, query[1], query[2]))
    lines = [str(n)] + [f"{u + 1} {v + 1} {weight}" for u, v, weight in weighted_edges]
    lines += [str(len(queries))]
    for query in queries:
        if query[0] == 1:
            lines.append(f"1 {query[1] + 1} {query[2]}")
        else:
            lines.append(f"2 {query[1] + 1} {query[2] + 1}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, expected)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(2, [(0, 1, 1)], [(2, 0, 1), (1, 0, 20), (2, 0, 1)]),
             (4, [(0, 1, 3), (1, 2, 5), (2, 3, 7)],
              [(2, 0, 3), (1, 1, 4), (2, 0, 3), (2, 2, 2)])]
    rng = random.Random(29407)
    for n in range(2, 14):
        edges = [(v, rng.randrange(v), rng.randint(1, 40)) for v in range(1, n)]
        queries = []
        for _ in range(25):
            if rng.randrange(3) == 0:
                queries.append((1, rng.randrange(n - 1), rng.randint(1, 40)))
            else:
                queries.append((2, rng.randrange(n), rng.randrange(n)))
        cases.append((n, edges, queries))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
