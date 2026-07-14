#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Point = tuple[int, int, int]
Query = tuple[int, int]


def nearest(points: list[Point], query: Query) -> tuple[int, int]:
    if not points:
        return (-1, -1)
    best = min(
        (
            (x - query[0]) ** 2 + (y - query[1]) ** 2,
            point_id,
        )
        for x, y, point_id in points
    )
    return (best[1], best[0])


def write_case(
    out_dir: Path,
    index: int,
    points: list[Point],
    queries: list[Query],
    answers: list[tuple[int, int]] | None = None,
) -> None:
    if answers is None:
        answers = [nearest(points, query) for query in queries]
    with (out_dir / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{len(points)} {len(queries)}\n")
        input_file.writelines(f"{x} {y} {point_id}\n" for x, y, point_id in points)
        input_file.writelines(f"{x} {y}\n" for x, y in queries)
        output_file.writelines(
            f"{point_id} {distance}\n" for point_id, distance in answers
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basic_points = [
        (0, 0, 8),
        (2, 0, 3),
        (-2, 0, 1),
        (0, 2, -1),
        (0, 0, 5),
    ]
    write_case(
        args.out_dir,
        0,
        basic_points,
        [(0, 0), (1, 0), (0, 1), (100, 100), (-2, 0)],
    )
    write_case(args.out_dir, 1, [], [(0, 0), (10, -10)])

    source = random.Random(2026071503)
    points = [
        (
            source.randint(-500, 500),
            source.randint(-500, 500),
            point_id,
        )
        for point_id in source.sample(range(-2000, 2000), 700)
    ]
    queries = [
        (source.randint(-600, 600), source.randint(-600, 600))
        for _ in range(900)
    ]
    write_case(args.out_dir, 2, points, queries)

    size = 150_000
    max_points = [(index * 3, 0, index) for index in range(size)]
    max_queries = [(index * 3, 0) for index in range(size)]
    max_answers = [(index, 0) for index in range(size)]
    write_case(args.out_dir, 3, max_points, max_queries, max_answers)


if __name__ == "__main__":
    main()
