#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

INF = 1 << 60


def solve(n: int, edges: list[tuple[int, int, int]], source: int) -> list[str]:
    distance = [INF] * n
    distance[source] = 0
    for _ in range(n - 1):
        updated = False
        for u, v, cost in edges:
            if distance[u] == INF:
                continue
            candidate = distance[u] + cost
            if candidate < distance[v]:
                distance[v] = candidate
                updated = True
        if not updated:
            break

    negative = [False] * n
    for _ in range(n):
        for u, v, cost in edges:
            if distance[u] == INF:
                continue
            if distance[u] + cost < distance[v] or negative[u]:
                if distance[u] + cost < distance[v]:
                    distance[v] = distance[u] + cost
                negative[v] = True

    result: list[str] = []
    for vertex in range(n):
        if negative[vertex]:
            result.append("NEG")
        elif distance[vertex] == INF:
            result.append("INF")
        else:
            result.append(str(distance[vertex]))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], int]] = [
        (3, [(0, 1, 2), (1, 2, -5), (0, 2, 10)], 0),
        (4, [(0, 1, 1), (1, 2, -3), (2, 1, 1), (2, 3, 1)], 0),
        (5, [(0, 1, 2), (2, 3, -2), (3, 2, 1)], 0),
        (1, [], 0),
    ]
    rng = random.Random(20260713)
    for _ in range(40):
        n = rng.randrange(1, 19)
        edges: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if rng.randrange(7) == 0:
                    edges.append((u, v, rng.randrange(-20, 31)))
        cases.append((n, edges, rng.randrange(n)))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for n, edges, source in cases:
        lines.append(f"{n} {len(edges)} {source}")
        lines.extend(f"{u} {v} {cost}" for u, v, cost in edges)
        outputs.append(" ".join(solve(n, edges, source)))

    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
