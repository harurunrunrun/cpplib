#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def distances(n, edges, source):
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    dist = [-1] * n
    dist[source] = 0
    queue = collections.deque([source])
    while queue:
        v = queue.popleft()
        for to in graph[v]:
            if dist[to] < 0:
                dist[to] = dist[v] + 1
                queue.append(to)
    return dist


def write_case(out_dir, index, n, edges, queries):
    lines = [str(n)] + [f"{u + 1} {v + 1}" for u, v in edges]
    lines += [str(len(queries))] + [f"{u + 1} {v + 1}" for u, v in queries]
    answer = [distances(n, edges, u)[v] + 1 for u, v in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(1404)
    cases = [(2, [(0, 1)], [(0, 0), (0, 1), (1, 1)]),
             (8, [(i, i + 1) for i in range(7)], [(0, 7), (3, 3), (1, 6)]),
             (8, [(0, i) for i in range(1, 8)], [(1, 2), (0, 7), (4, 4)])]
    for n in range(2, 11):
        edges = [(v, rng.randrange(v)) for v in range(1, n)]
        queries = [(rng.randrange(n), rng.randrange(n)) for _ in range(15)]
        cases.append((n, edges, queries))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
