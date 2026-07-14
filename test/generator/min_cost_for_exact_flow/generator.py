#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int, int]


def exact_cost(
    n: int,
    source: int,
    sink: int,
    required_flow: int,
    edges: list[Edge],
) -> int | None:
    selected = [0] * len(edges)
    best: int | None = None

    def dfs(index: int) -> None:
        nonlocal best
        if index != len(edges):
            for flow in range(edges[index][2] + 1):
                selected[index] = flow
                dfs(index + 1)
            return

        balance = [0] * n
        cost = 0
        for flow, (u, v, _capacity, edge_cost) in zip(selected, edges):
            balance[u] -= flow
            balance[v] += flow
            cost += flow * edge_cost
        if balance[source] != -required_flow or balance[sink] != required_flow:
            return
        if any(balance[v] != 0 for v in range(n) if v not in (source, sink)):
            return
        if best is None or cost < best:
            best = cost

    dfs(0)
    return best


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    source: int,
    sink: int,
    required_flow: int,
    edges: list[Edge],
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{n} {len(edges)} {source} {sink} {required_flow}"]
    lines.extend(f"{u} {v} {capacity} {cost}" for u, v, capacity, cost in edges)
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    answer = exact_cost(n, source, sink, required_flow, edges)
    expected = "impossible" if answer is None else str(answer)
    (out_dir / f"{stem}.out").write_text(expected + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, int, int, int, list[Edge]]] = [
        (2, 0, 1, 0, []),
        (2, 0, 1, 1, []),
        (2, 0, 1, 1, [(0, 1, 1, -1)]),
        (4, 0, 3, 2, [
            (0, 1, 2, 1),
            (0, 2, 1, 5),
            (1, 2, 1, -2),
            (1, 3, 1, 2),
            (2, 3, 2, 1),
        ]),
    ]
    rng = random.Random(20260714)
    for _ in range(32):
        n = rng.randrange(2, 7)
        candidates = [(u, v) for u in range(n) for v in range(u + 1, n)]
        rng.shuffle(candidates)
        edges = [
            (u, v, rng.randrange(3), rng.randrange(-7, 9))
            for u, v in candidates[: rng.randrange(min(8, len(candidates)) + 1)]
        ]
        cases.append((n, 0, n - 1, rng.randrange(6), edges))

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
