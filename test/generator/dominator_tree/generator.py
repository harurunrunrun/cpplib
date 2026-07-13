#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def naive_dominators(n: int, edges: list[tuple[int, int]], source: int) -> list[int]:
    graph = [[] for _ in range(n)]
    predecessor = [[] for _ in range(n)]
    for left, right in edges:
        graph[left].append(right)
        predecessor[right].append(left)
    reachable = [False] * n
    reachable[source] = True
    queue = deque([source])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if not reachable[to]:
                reachable[to] = True
                queue.append(to)
    all_reachable = {vertex for vertex in range(n) if reachable[vertex]}
    dom = [set() for _ in range(n)]
    for vertex in all_reachable:
        dom[vertex] = set(all_reachable)
    dom[source] = {source}
    changed = True
    while changed:
        changed = False
        for vertex in all_reachable:
            if vertex == source:
                continue
            incoming = [dom[parent] for parent in predecessor[vertex] if reachable[parent]]
            intersection = set.intersection(*incoming) if incoming else set()
            nxt = intersection | {vertex}
            if nxt != dom[vertex]:
                dom[vertex] = nxt
                changed = True
    answer = [-1] * n
    answer[source] = source
    for vertex in all_reachable:
        if vertex == source:
            continue
        answer[vertex] = max(dom[vertex] - {vertex}, key=lambda candidate: len(dom[candidate]))
    return answer


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    source: int,
    answer: list[int] | None = None,
) -> None:
    if answer is None:
        answer = naive_dominators(n, edges, source)
    lines = [f"{n} {len(edges)} {source}"]
    lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 7, [(0, 1), (0, 2), (1, 3), (2, 3), (3, 4), (5, 6)], 0)
    for index, seed in enumerate([2026071481, 2026071482, 2026071483], start=1):
        rng = random.Random(seed)
        n = 28
        source = rng.randrange(n)
        edges = [
            (left, right)
            for left in range(n)
            for right in range(n)
            if rng.randrange(100) < 13
        ]
        write_case(out_dir, index, n, edges, source)

    n = 100000
    edges = [(vertex, vertex + 1) for vertex in range(n - 1)]
    answer = [0] + [vertex - 1 for vertex in range(1, n)]
    write_case(out_dir, 4, n, edges, 0, answer)


if __name__ == "__main__":
    main()
