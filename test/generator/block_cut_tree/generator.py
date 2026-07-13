#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def component_count(n: int, edges: list[tuple[int, int]], removed: int = -1) -> int:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        if u == removed or v == removed:
            continue
        graph[u].append(v)
        graph[v].append(u)
    visited = [False] * n
    if removed >= 0:
        visited[removed] = True
    count = 0
    for start in range(n):
        if visited[start]:
            continue
        count += 1
        visited[start] = True
        stack = [start]
        while stack:
            v = stack.pop()
            for to in graph[v]:
                if not visited[to]:
                    visited[to] = True
                    stack.append(to)
    return count


def connected_without(
    n: int,
    edges: list[tuple[int, int]],
    source: int,
    target: int,
    removed: int,
) -> bool:
    if source == removed or target == removed:
        return False
    graph = [[] for _ in range(n)]
    for u, v in edges:
        if u == removed or v == removed:
            continue
        graph[u].append(v)
        graph[v].append(u)
    stack = [source]
    visited = {source}
    while stack:
        v = stack.pop()
        if v == target:
            return True
        for to in graph[v]:
            if to not in visited:
                visited.add(to)
                stack.append(to)
    return False


def share_block(n: int, edges: list[tuple[int, int]], u: int, v: int) -> bool:
    if u == v:
        return True
    if not connected_without(n, edges, u, v, -1):
        return False
    return all(
        connected_without(n, edges, u, v, removed)
        for removed in range(n)
        if removed != u and removed != v
    )


def write_case(out_dir: Path, case_id: int, n: int, edges: list[tuple[int, int]]) -> None:
    base_components = component_count(n, edges)
    queries: list[str] = []
    answers: list[str] = []
    for v in range(n):
        queries.append(f"ART {v}")
        answers.append(str(int(component_count(n, edges, v) > base_components)))
    for u in range(n):
        for v in range(n):
            queries.append(f"SAME {u} {v}")
            answers.append(str(int(share_block(n, edges, u, v))))
    lines = [f"{n} {len(edges)} {len(queries)}"]
    lines += [f"{u} {v}" for u, v in edges]
    lines += queries
    (out_dir / f"case_{case_id:02d}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    expected = "\n".join(answers)
    if expected:
        expected += "\n"
    (out_dir / f"case_{case_id:02d}.out").write_text(expected, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (0, []),
        (1, []),
        (5, [(0, 1), (1, 2), (2, 0), (1, 3), (3, 4)]),
        (4, [(0, 1), (0, 1), (1, 2)]),
    ]
    rng = random.Random(20260713)
    for n in (6, 8, 10, 12):
        edges = [
            (u, v)
            for u in range(n)
            for v in range(u + 1, n)
            if rng.randrange(4) == 0
        ]
        cases.append((n, edges))
    for case_id, (n, edges) in enumerate(cases):
        write_case(out_dir, case_id, n, edges)


if __name__ == "__main__":
    main()
