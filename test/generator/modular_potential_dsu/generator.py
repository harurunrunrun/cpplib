#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


MOD = 998244353


def difference(graph: list[list[tuple[int, int]]], u: int, v: int) -> int | None:
    relative = [None] * len(graph)
    relative[u] = 0
    queue = collections.deque([u])
    while queue:
        current = queue.popleft()
        if current == v:
            return (-relative[v]) % MOD
        for to, delta in graph[current]:
            if relative[to] is None:
                relative[to] = (relative[current] + delta) % MOD
                queue.append(to)
    return None


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2147483647)
    n = 45
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    operations: list[str] = []
    answers: list[str] = []
    for _ in range(1800):
        u = rng.randrange(n)
        v = rng.randrange(n)
        if rng.randrange(3) != 0:
            raw = rng.choice([rng.randrange(-2 * MOD, 2 * MOD), MOD - 1, MOD - 2])
            expected = difference(graph, u, v)
            normalized = raw % MOD
            accepted = expected is None or expected == normalized
            answers.append("1" if accepted else "0")
            operations.append(f"M {u} {v} {raw}")
            if expected is None:
                graph[u].append((v, -normalized % MOD))
                graph[v].append((u, normalized))
        else:
            expected = difference(graph, u, v)
            answers.append("NA" if expected is None else str(expected))
            operations.append(f"D {u} {v}")

    lines = [f"0 {n} {len(operations)}", *operations]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(answers) + "\n", encoding="utf-8")

    large_mod = 2147483647
    large_operations = [
        f"M 0 1 {large_mod - 1}",
        f"M 1 2 {large_mod - 1}",
        "D 0 2",
        "D 2 0",
        "M 0 2 -2",
        "M 0 2 -1",
        "D 0 3",
        f"M 2 3 {large_mod - 1}",
        "D 0 3",
    ]
    large_answers = [
        "1",
        "1",
        str(large_mod - 2),
        "2",
        "1",
        "0",
        "NA",
        "1",
        str(large_mod - 3),
    ]
    large_lines = [f"1 4 {len(large_operations)}", *large_operations]
    (out_dir / "case_01.in").write_text("\n".join(large_lines) + "\n", encoding="utf-8")
    (out_dir / "case_01.out").write_text("\n".join(large_answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
