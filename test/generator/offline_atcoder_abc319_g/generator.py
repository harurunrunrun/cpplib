#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


MOD = 998244353


def solve(n, missing):
    blocked = [set() for _ in range(n)]
    for u, v in missing:
        blocked[u].add(v)
        blocked[v].add(u)
    graph = [[to for to in range(n) if to != v and to not in blocked[v]] for v in range(n)]
    dist = [-1] * n
    ways = [0] * n
    dist[0] = 0
    ways[0] = 1
    queue = collections.deque([0])
    while queue:
        v = queue.popleft()
        for to in graph[v]:
            if dist[to] == -1:
                dist[to] = dist[v] + 1
                queue.append(to)
            if dist[to] == dist[v] + 1:
                ways[to] = (ways[to] + ways[v]) % MOD
    return -1 if dist[n - 1] == -1 else ways[n - 1]


def write_case(out_dir, index, n, missing):
    lines = [f"{n} {len(missing)}"] + [f"{u + 1} {v + 1}" for u, v in missing]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{solve(n, missing)}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(2, []), (2, [(0, 1)]),
             (5, [(0, 4)]),
             (5, [(u, v) for u in range(5) for v in range(u + 1, 5) if (u, v) != (0, 4)])]
    rng = random.Random(31907)
    for n in range(3, 13):
        missing = [(u, v) for u in range(n) for v in range(u + 1, n)
                   if rng.randrange(3) == 0]
        cases.append((n, missing))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
