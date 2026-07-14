#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def components(n, edges, skip=-1):
    graph = [[] for _ in range(n)]
    for index, (u, v) in enumerate(edges):
        if index != skip:
            graph[u].append(v)
            graph[v].append(u)
    seen = [False] * n
    count = 0
    for root in range(n):
        if seen[root]:
            continue
        count += 1
        seen[root] = True
        stack = [root]
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not seen[to]:
                    seen[to] = True
                    stack.append(to)
    return count


def write_case(out_dir, index, n, edges):
    base = components(n, edges)
    answer = sum(components(n, edges, i) > base for i in range(len(edges)))
    lines = [f"{n} {len(edges)}"] + [f"{u + 1} {v + 1}" for u, v in edges]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{answer}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(2, [(0, 1)]), (3, [(0, 1), (1, 2), (2, 0)]),
             (7, [(i, i + 1) for i in range(6)])]
    rng = random.Random(7503)
    for n in range(3, 11):
        edges = {(rng.randrange(v), v) for v in range(1, n)}
        for u in range(n):
            for v in range(u + 1, n):
                if rng.randrange(4) == 0:
                    edges.add((u, v))
        cases.append((n, sorted(edges)))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
