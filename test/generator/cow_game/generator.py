#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

INF = 1 << 60


def feasible(n: int, constraints: list[tuple[int, int, int]]) -> bool:
    dist = [0] * n
    for it in range(n):
        updated = False
        for u, v, c in constraints:
            if dist[u] + c < dist[v]:
                dist[v] = dist[u] + c
                updated = True
        if not updated:
            return True
        if it == n - 1:
            return False
    return True


def shortest(n: int, source: int, constraints: list[tuple[int, int, int]]) -> list[int]:
    dist = [INF] * n
    dist[source] = 0
    for _ in range(n - 1):
        updated = False
        for u, v, c in constraints:
            if dist[u] == INF:
                continue
            if dist[u] + c < dist[v]:
                dist[v] = dist[u] + c
                updated = True
        if not updated:
            break
    return dist


def solve_max(n: int, source: int, target: int, constraints: list[tuple[int, int, int]]) -> str:
    if not feasible(n, constraints):
        return "infeasible"
    dist = shortest(n, source, constraints)
    return "inf" if dist[target] == INF else str(dist[target])


def solve_range(n: int, source: int, target: int, constraints: list[tuple[int, int, int]]) -> str:
    if not feasible(n, constraints):
        return "infeasible"
    upper = shortest(n, source, constraints)[target]
    lower_dual = shortest(n, target, constraints)[source]
    left = "-inf" if lower_dual == INF else str(-lower_dual)
    right = "inf" if upper == INF else str(upper)
    return f"{left} {right}"


def write_case(out_dir: Path, idx: int, n: int, constraints: list[tuple[int, int, int]], queries: list[tuple[str, int, int]]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join(
            [
                f"{n} {len(constraints)} {len(queries)}",
                *[f"{u} {v} {c}" for u, v, c in constraints],
                *[f"{kind} {s} {t}" for kind, s, t in queries],
            ]
        ) + "\n",
        encoding="utf-8",
    )
    outputs = [
        solve_max(n, s, t, constraints) if kind == "MAX" else solve_range(n, s, t, constraints)
        for kind, s, t in queries
    ]
    (out_dir / f"{name}.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], list[tuple[str, int, int]]]] = [
        (3, [(0, 1, 3), (1, 2, 4), (0, 2, 10)], [("MAX", 0, 2), ("RANGE", 0, 2)]),
        (2, [(0, 1, 5), (1, 0, -10)], [("MAX", 0, 1), ("RANGE", 0, 1)]),
        (2, [], [("MAX", 0, 1), ("RANGE", 0, 1)]),
        (
            3,
            [(0, 1, 5), (1, 0, -5), (2, 1, -2), (1, 2, 4)],
            [("RANGE", 0, 2), ("MAX", 0, 2), ("MAX", 2, 0)],
        ),
    ]
    rng = random.Random(20260821)
    for n in [1, 5, 20, 40]:
        constraints: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if u != v and rng.randrange(7) == 0:
                    constraints.append((u, v, rng.randrange(21) - 10))
        queries = []
        for _ in range(8):
            kind = "MAX" if rng.randrange(2) == 0 else "RANGE"
            queries.append((kind, rng.randrange(n), rng.randrange(n)))
        cases.append((n, constraints, queries))

    for i, case in enumerate(cases):
        write_case(out_dir, i, *case)


if __name__ == "__main__":
    main()
