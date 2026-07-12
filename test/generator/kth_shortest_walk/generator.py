#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def solve(n: int, edges: list[tuple[int, int, int]], s: int, t: int, k: int) -> list[int]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for u, v, w in edges:
        graph[u].append((v, w))
    reachable = [False] * n
    stack = [s]
    reachable[s] = True
    while stack:
        v = stack.pop()
        for to, _ in graph[v]:
            if not reachable[to]:
                reachable[to] = True
                stack.append(to)
    if not reachable[t]:
        return []
    result: list[int] = []
    que: list[tuple[int, int]] = [(0, s)]
    guard = 0
    while que and len(result) < k:
        d, v = heapq.heappop(que)
        if v == t:
            result.append(d)
        for to, w in graph[v]:
            heapq.heappush(que, (d + w, to))
        guard += 1
        if guard > 200000:
            raise RuntimeError("too many generated walks")
    return result


def write_case(out_dir: Path, idx: int, n: int, edges: list[tuple[int, int, int]], s: int, t: int, k: int) -> None:
    costs = solve(n, edges, s, t, k)
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{n} {len(edges)} {s} {t} {k}", *[f"{u} {v} {w}" for u, v, w in edges]]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(f"{len(costs)}\n" + " ".join(map(str, costs)) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], int, int, int]] = [
        (1, [], 0, 0, 3),
        (3, [(0, 1, 1), (0, 2, 2), (1, 2, 1), (1, 1, 2)], 0, 2, 4),
        (4, [(0, 1, 1), (2, 3, 1)], 0, 3, 5),
    ]
    rng = random.Random(20260730)
    for n in [2, 4, 6, 8]:
        for _ in range(2):
            edges: list[tuple[int, int, int]] = []
            for u in range(n):
                for v in range(n):
                    if u < v and rng.randrange(4) == 0:
                        edges.append((u, v, 1 + rng.randrange(5)))
            cases.append((n, edges, rng.randrange(n), rng.randrange(n), 6))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
