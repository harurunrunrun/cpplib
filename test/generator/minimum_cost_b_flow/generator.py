#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def brute(
    vertex_count: int,
    edges: list[tuple[int, int, int, int, int]],
    target_balance: list[int],
) -> int | None:
    answer = None
    ranges = [
        range(lower, upper + 1)
        for _, _, lower, upper, _ in edges
    ]
    for values in itertools.product(*ranges):
        balance = [0] * vertex_count
        cost = 0
        for (from_, to, _, _, edge_cost), value in zip(edges, values):
            balance[from_] -= value
            balance[to] += value
            cost += value * edge_cost
        if balance == target_balance:
            answer = cost if answer is None else min(answer, cost)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[
        tuple[int, int, list[int], list[tuple[int, int, int, int, int]]]
    ] = [
        (0, 2, [0, 0], [(0, 1, 0, 3, -2), (1, 0, 0, 3, 0)]),
        (1, 2, [-2, 2], [(0, 1, 0, 5, 7)]),
        (1, 2, [-3, 3], [(0, 1, 0, 2, 1)]),
        (0, 1, [0], [(0, 0, 1, 4, -3)]),
    ]
    for _ in range(360):
        vertex_count = rng.randint(1, 5)
        edge_count = rng.randint(0, 7)
        edges = []
        witness = []
        for _ in range(edge_count):
            from_ = rng.randrange(vertex_count)
            to = rng.randrange(vertex_count)
            lower = rng.randint(0, 2)
            upper = rng.randint(lower, 3)
            cost = rng.randint(-8, 9)
            edges.append((from_, to, lower, upper, cost))
            witness.append(rng.randint(lower, upper))
        kind = rng.randrange(2)
        if kind == 0:
            balance = [0] * vertex_count
        else:
            balance = [0] * vertex_count
            for (from_, to, _, _, _), value in zip(edges, witness):
                balance[from_] -= value
                balance[to] += value
            if rng.random() < 0.2 and vertex_count >= 2:
                first, second = rng.sample(range(vertex_count), 2)
                balance[first] += 1
                balance[second] -= 1
        cases.append((kind, vertex_count, balance, edges))
    input_lines = [str(len(cases))]
    output_lines = []
    for kind, vertex_count, balance, edges in cases:
        input_lines.append(f"{kind} {vertex_count} {len(edges)}")
        if kind == 1:
            input_lines.append(" ".join(map(str, balance)))
        input_lines.extend(
            f"{from_} {to} {lower} {upper} {cost}"
            for from_, to, lower, upper, cost in edges
        )
        answer = brute(vertex_count, edges, balance)
        output_lines.append("none" if answer is None else str(answer))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
