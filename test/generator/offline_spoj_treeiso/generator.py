#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def centers(graph):
    n = len(graph)
    if n <= 2:
        return list(range(n))
    degree = [len(edges) for edges in graph]
    leaves = collections.deque(v for v in range(n) if degree[v] <= 1)
    remaining = n
    while remaining > 2:
        size = len(leaves)
        remaining -= size
        for _ in range(size):
            v = leaves.popleft()
            for to in graph[v]:
                degree[to] -= 1
                if degree[to] == 1:
                    leaves.append(to)
    return list(leaves)


def rooted_code(graph, root, parent=-1):
    return "(" + "".join(sorted(rooted_code(graph, to, root)
                                 for to in graph[root] if to != parent)) + ")"


def canonical(n, edges):
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    return min(rooted_code(graph, root) for root in centers(graph))


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for n, first, second in groups:
        lines.append(str(n))
        lines += [f"{u + 1} {v + 1}" for u, v in first]
        lines += [f"{u + 1} {v + 1}" for u, v in second]
        output.append("YES" if canonical(n, first) == canonical(n, second) else "NO")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[(1, [], [])],
             [(4, [(0, 1), (1, 2), (2, 3)], [(0, 1), (0, 2), (0, 3)])]]
    rng = random.Random(730017)
    for _ in range(12):
        groups = []
        for n in range(2, 14):
            first = [(v, rng.randrange(v)) for v in range(1, n)]
            if rng.randrange(2) == 0:
                permutation = list(range(n))
                rng.shuffle(permutation)
                second = [(permutation[u], permutation[v]) for u, v in first]
            else:
                second = [(v, rng.randrange(v)) for v in range(1, n)]
            groups.append((n, first, second))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
