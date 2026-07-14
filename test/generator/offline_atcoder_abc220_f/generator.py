#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def solve(n, edges):
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    answer = []
    for source in range(n):
        dist = [-1] * n
        dist[source] = 0
        queue = collections.deque([source])
        while queue:
            v = queue.popleft()
            for to in graph[v]:
                if dist[to] < 0:
                    dist[to] = dist[v] + 1
                    queue.append(to)
        answer.append(sum(dist))
    return answer


def write_case(out_dir, index, n, edges):
    lines = [str(n)] + [f"{u + 1} {v + 1}" for u, v in edges]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, solve(n, edges))) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(22006)
    cases = [(2, [(0, 1)]), (9, [(i, i + 1) for i in range(8)]),
             (9, [(0, i) for i in range(1, 9)])]
    for n in range(2, 15):
        cases.append((n, [(v, rng.randrange(v)) for v in range(1, n)]))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
