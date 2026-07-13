#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def has_trail(n: int, edges: list[tuple[int, int]], directed: bool, forced_start: int) -> bool:
    m = len(edges)
    if m == 0:
        return forced_start == -1 or 0 <= forced_start < n
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    if directed:
        indegree = [0] * n
        outdegree = [0] * n
        for edge_id, (u, v) in enumerate(edges):
            graph[u].append((v, edge_id))
            outdegree[u] += 1
            indegree[v] += 1
        starts = [v for v in range(n) if outdegree[v] - indegree[v] == 1]
        ends = [v for v in range(n) if indegree[v] - outdegree[v] == 1]
        if any(abs(outdegree[v] - indegree[v]) > 1 for v in range(n)):
            return False
        if len(starts) != len(ends) or len(starts) > 1:
            return False
        start = forced_start
        if start == -1:
            start = starts[0] if starts else next((v for v in range(n) if outdegree[v]), -1)
        elif starts and start != starts[0]:
            return False
    else:
        degree = [0] * n
        for edge_id, (u, v) in enumerate(edges):
            graph[u].append((v, edge_id))
            graph[v].append((u, edge_id))
            degree[u] += 1
            degree[v] += 1
        odds = [v for v in range(n) if degree[v] % 2]
        if len(odds) not in (0, 2):
            return False
        start = forced_start
        if start == -1:
            start = odds[0] if odds else next((v for v in range(n) if degree[v]), -1)
        elif odds and start not in odds:
            return False
    if start == -1:
        return False

    cursor = [0] * n
    used = [False] * m
    stack = [start]
    count = 0
    while stack:
        v = stack[-1]
        while cursor[v] < len(graph[v]) and used[graph[v][cursor[v]][1]]:
            cursor[v] += 1
        if cursor[v] == len(graph[v]):
            stack.pop()
            continue
        to, edge_id = graph[v][cursor[v]]
        cursor[v] += 1
        if used[edge_id]:
            continue
        used[edge_id] = True
        count += 1
        stack.append(to)
    return count == m


def write_case(
    out_dir: Path,
    index: int,
    directed: bool,
    n: int,
    edges: list[tuple[int, int]],
    start: int,
) -> None:
    name = f"case_{index:02d}"
    lines = [f"{int(directed)} {n} {len(edges)} {start}"]
    lines.extend(f"{u} {v}" for u, v in edges)
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    expected = int(has_trail(n, edges, directed, start))
    (out_dir / f"{name}.out").write_text(f"{expected}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[bool, int, list[tuple[int, int]], int]] = [
        (True, 0, [], -1),
        (False, 1, [], -1),
        (True, 2, [(0, 0), (0, 1), (1, 0), (0, 0)], -1),
        (True, 2, [(0, 1)], 1),
        (False, 2, [(0, 1), (0, 1), (0, 0), (1, 1)], -1),
        (False, 3, [(0, 1), (1, 2)], 1),
        (True, 4, [(0, 1), (1, 0), (2, 3), (3, 2)], -1),
        (False, 4, [(0, 1), (1, 0), (2, 3), (3, 2)], -1),
    ]
    rng = random.Random(20260901)
    for directed in (False, True):
        for n, m in [(2, 4), (6, 12), (20, 50), (50, 140)]:
            edges = [(rng.randrange(n), rng.randrange(n)) for _ in range(m)]
            start = rng.randrange(n) if rng.randrange(2) else -1
            cases.append((directed, n, edges, start))

        n = 30
        walk = [rng.randrange(n) for _ in range(101)]
        edges = list(zip(walk, walk[1:]))
        if not directed:
            rng.shuffle(edges)
        cases.append((directed, n, edges, walk[0]))

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
