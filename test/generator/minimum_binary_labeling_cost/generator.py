#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

UnaryCost = tuple[int, int]
PairwiseCost = tuple[int, int, int]
Case = tuple[list[UnaryCost], list[PairwiseCost]]


def minimum_cost(
    unary_costs: list[UnaryCost],
    pairwise_costs: list[PairwiseCost],
) -> int:
    answer: int | None = None
    for labels in range(1 << len(unary_costs)):
        cost = sum(
            unary[(labels >> vertex) & 1]
            for vertex, unary in enumerate(unary_costs)
        )
        cost += sum(
            different_cost
            for left, right, different_cost in pairwise_costs
            if ((labels >> left) & 1) != ((labels >> right) & 1)
        )
        answer = cost if answer is None else min(answer, cost)
    return 0 if answer is None else answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        ([], []),
        ([(7, 3)], []),
        ([(0, 10), (10, 0)], [(0, 1, 4)]),
        (
            [(3, 8), (6, 1), (5, 5)],
            [(0, 1, 4), (0, 1, 7), (2, 2, 100)],
        ),
        (
            [(3_000_000_000_000_000_000, 4_000_000_000_000_000_000)] * 2,
            [(0, 1, 4_000_000_000_000_000_000)],
        ),
    ]

    source = random.Random(41003)
    for _ in range(160):
        variable_count = source.randint(0, 9)
        unary_costs = [
            (source.randint(0, 100), source.randint(0, 100))
            for _ in range(variable_count)
        ]
        pairwise_costs: list[PairwiseCost] = []
        if variable_count > 0:
            for _ in range(source.randint(0, 24)):
                pairwise_costs.append((
                    source.randrange(variable_count),
                    source.randrange(variable_count),
                    source.randint(0, 100),
                ))
        cases.append((unary_costs, pairwise_costs))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for unary_costs, pairwise_costs in cases:
        input_lines.append(f"{len(unary_costs)} {len(pairwise_costs)}")
        input_lines.extend(f"{zero} {one}" for zero, one in unary_costs)
        input_lines.extend(
            f"{left} {right} {cost}"
            for left, right, cost in pairwise_costs
        )
        output_lines.append(str(minimum_cost(
            unary_costs,
            pairwise_costs,
        )))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
