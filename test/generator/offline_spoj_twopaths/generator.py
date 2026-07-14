#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def component(n, edges, skip, root):
    graph = [[] for _ in range(n)]
    for index, (u, v) in enumerate(edges):
        if index != skip:
            graph[u].append(v)
            graph[v].append(u)
    seen = {root}
    queue = collections.deque([root])
    while queue:
        v = queue.popleft()
        for to in graph[v]:
            if to not in seen:
                seen.add(to)
                queue.append(to)
    return graph, seen


def diameter(graph, vertices):
    return max(
        shortest(graph, source, target)
        for source in vertices for target in vertices
    )


def shortest(graph, source, target):
    queue = collections.deque([(source, 0)])
    seen = {source}
    while queue:
        v, distance = queue.popleft()
        if v == target:
            return distance
        for to in graph[v]:
            if to not in seen:
                seen.add(to)
                queue.append((to, distance + 1))
    raise AssertionError


def solve(n, edges):
    answer = 0
    all_vertices = set(range(n))
    for skip, (u, _) in enumerate(edges):
        graph, first = component(n, edges, skip, u)
        second = all_vertices - first
        answer = max(answer, diameter(graph, first) * diameter(graph, second))
    return answer


def write_case(out_dir, index, n, edges):
    lines = [str(n)] + [f"{u + 1} {v + 1}" for u, v in edges]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{solve(n, edges)}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(2, [(0, 1)]), (8, [(i, i + 1) for i in range(7)]),
             (8, [(0, i) for i in range(1, 8)])]
    rng = random.Random(730020)
    for n in range(3, 17):
        cases.append((n, [(v, rng.randrange(v)) for v in range(1, n)]))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
