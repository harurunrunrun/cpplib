#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def cheapest_position(
    distance: list[list[int]],
    tour: list[int],
    vertex: int,
) -> int:
    return min(
        (
            distance[tour[position - 1]][vertex]
            + distance[vertex][tour[position]]
            - distance[tour[position - 1]][tour[position]],
            position,
        )
        for position in range(len(tour))
    )[1]


def arbitrary_insertion(
    distance: list[list[int]],
    insertion_order: list[int],
) -> list[int]:
    if not insertion_order:
        return []
    tour = [insertion_order[0]]
    for vertex in insertion_order[1:]:
        tour.insert(cheapest_position(distance, tour, vertex), vertex)
    return tour


def tour_cost(distance: list[list[int]], tour: list[int]) -> int:
    return sum(
        distance[tour[index]][tour[(index + 1) % len(tour)]]
        for index in range(len(tour))
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026072901)
    cases: list[tuple[list[list[int]], list[int]]] = [
        ([], []),
        ([[7]], [0]),
        (
            [
                [0, 1, 1, 1],
                [1, 0, 1, 1],
                [1, 1, 0, 1],
                [1, 1, 1, 0],
            ],
            [3, 1, 2, 0],
        ),
    ]
    for _ in range(90):
        size = rng.randrange(1, 42)
        if rng.randrange(2) == 0:
            points = [
                (rng.randrange(-500, 501), rng.randrange(-500, 501))
                for _ in range(size)
            ]
            distance = [
                [
                    abs(first_x - second_x) + abs(first_y - second_y)
                    for second_x, second_y in points
                ]
                for first_x, first_y in points
            ]
        else:
            distance = [
                [
                    0 if first == second else rng.randrange(-50, 501)
                    for second in range(size)
                ]
                for first in range(size)
            ]
        insertion_order = list(range(size))
        rng.shuffle(insertion_order)
        cases.append((distance, insertion_order))

    size = 180
    distance = [
        [
            0 if first == second
            else abs(first - second) + (first * 17 + second * 31) % 23
            for second in range(size)
        ]
        for first in range(size)
    ]
    insertion_order = list(range(size))
    rng.shuffle(insertion_order)
    cases.append((distance, insertion_order))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for distance, insertion_order in cases:
        size = len(distance)
        input_lines.append(str(size))
        input_lines.append(" ".join(map(str, insertion_order)))
        input_lines.extend(" ".join(map(str, row)) for row in distance)
        tour = arbitrary_insertion(distance, insertion_order)
        output_lines.append(
            str(len(tour))
            + "".join(f" {vertex}" for vertex in tour)
            + f" | {tour_cost(distance, tour)}"
        )

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
