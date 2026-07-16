#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[int, int, list[int], list[tuple[int, int]]]


def normalized(case: Case) -> tuple[list[int], list[tuple[int, int]]]:
    n, _, linear, quadratic = case
    linear_mask = [0] * n
    edge_mask: dict[tuple[int, int], int] = {}
    for index in linear:
        linear_mask[index] ^= 1
    for first, second in quadratic:
        if first == second:
            linear_mask[first] ^= 1
        else:
            edge = (min(first, second), max(first, second))
            edge_mask[edge] = edge_mask.get(edge, 0) ^ 1
    return linear_mask, [edge for edge, value in edge_mask.items() if value]


def rank_gf2(n: int, edges: list[tuple[int, int]]) -> int:
    rows = [0] * n
    for first, second in edges:
        rows[first] ^= 1 << second
        rows[second] ^= 1 << first
    rank = 0
    for column in range(n):
        pivot = next((r for r in range(rank, n) if rows[r] >> column & 1), None)
        if pivot is None:
            continue
        rows[rank], rows[pivot] = rows[pivot], rows[rank]
        for row in range(n):
            if row != rank and (rows[row] >> column & 1):
                rows[row] ^= rows[rank]
        rank += 1
    return rank


def brute_answer(case: Case) -> str:
    n, constant, _, _ = case
    linear, edges = normalized(case)
    counts = [0, 0]
    for mask in range(1 << n):
        value = constant
        for index, coefficient in enumerate(linear):
            value ^= coefficient & (mask >> index & 1)
        for first, second in edges:
            value ^= (mask >> first & 1) & (mask >> second & 1)
        counts[value] += 1
    rank = rank_gf2(n, edges)
    if counts[0] == counts[1]:
        balanced, arf = 1, -1
    else:
        balanced = 0
        arf = int((counts[0] < counts[1]) ^ bool(constant))
    return f"{rank} {balanced} {arf} {counts[0]} {counts[1]}"


def serialize(case: Case) -> str:
    n, constant, linear, quadratic = case
    lines = [f"{n} {constant} {len(linear)} {len(quadratic)}"]
    if linear:
        lines.append(" ".join(map(str, linear)))
    for first, second in quadratic:
        lines.append(f"{first} {second}")
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260716)
    cases: list[Case] = [
        (0, 0, [], []),
        (0, 1, [], []),
        (1, 0, [], []),
        (1, 0, [0], []),
        (2, 0, [], [(0, 1)]),
        (2, 0, [0, 1], [(0, 1)]),
        (3, 1, [2], [(0, 1)]),
        (4, 0, [0, 0], [(0, 1), (1, 0), (2, 2)]),
    ]
    for _ in range(350):
        n = rng.randrange(0, 11)
        linear = [rng.randrange(n) for _ in range(rng.randrange(0, 2 * n + 1))] if n else []
        quadratic = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(rng.randrange(0, n * n + 1))
        ] if n else []
        cases.append((n, rng.randrange(2), linear, quadratic))

    (out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n" + "\n".join(map(serialize, cases)) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(map(brute_answer, cases)) + "\n",
        encoding="utf-8",
    )

    n = 1024
    edges = [(index, n - 1 - index) for index in range(n // 2)]
    linear = [vertex for index in range(101) for vertex in (index, n - 1 - index)]
    large: Case = (n, 0, linear, edges)
    zero_count = (1 << (n - 1)) - (1 << (n - n // 2 - 1))
    one_count = (1 << n) - zero_count
    (out_dir / "case_01.in").write_text("1\n" + serialize(large) + "\n", encoding="utf-8")
    (out_dir / "case_01.out").write_text(
        f"1024 0 1 {zero_count} {one_count}\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
