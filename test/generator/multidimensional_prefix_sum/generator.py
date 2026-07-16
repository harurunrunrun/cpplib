#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Coordinate = tuple[int, int, int]
Update = tuple[str, Coordinate, int]
Query = tuple[str, Coordinate, Coordinate | None]


def flat_index(shape: Coordinate, coordinate: Coordinate) -> int:
    return (coordinate[0] * shape[1] + coordinate[1]) * shape[2] + coordinate[2]


def range_sum(
    shape: Coordinate,
    values: list[int],
    begin: Coordinate,
    end: Coordinate,
) -> int:
    result = 0
    for x in range(begin[0], end[0]):
        for y in range(begin[1], end[1]):
            for z in range(begin[2], end[2]):
                result += values[flat_index(shape, (x, y, z))]
    return result


def write_case(
    out_dir: Path,
    index: int,
    shape: Coordinate,
    updates: list[Update],
    queries: list[Query],
) -> None:
    values = [0] * (shape[0] * shape[1] * shape[2])
    input_lines = [f"{shape[0]} {shape[1]} {shape[2]}", str(len(updates))]
    for kind, coordinate, value in updates:
        input_lines.append(
            f"{kind} {coordinate[0]} {coordinate[1]} {coordinate[2]} {value}"
        )
        position = flat_index(shape, coordinate)
        if kind == "S":
            values[position] = value
        else:
            values[position] += value

    input_lines.append(str(len(queries)))
    answers: list[int] = []
    for kind, first, second in queries:
        if kind == "G":
            input_lines.append(f"G {first[0]} {first[1]} {first[2]}")
            answers.append(values[flat_index(shape, first)])
        elif kind == "P":
            input_lines.append(f"P {first[0]} {first[1]} {first[2]}")
            answers.append(range_sum(shape, values, (0, 0, 0), first))
        else:
            assert second is not None
            input_lines.append(
                f"R {first[0]} {first[1]} {first[2]} "
                f"{second[0]} {second[1]} {second[2]}"
            )
            answers.append(range_sum(shape, values, first, second))

    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "".join(f"{answer}\n" for answer in answers), encoding="utf-8"
    )


def random_coordinate(generator: random.Random, shape: Coordinate) -> Coordinate:
    return tuple(generator.randrange(extent) for extent in shape)  # type: ignore[return-value]


def random_corner(generator: random.Random, shape: Coordinate) -> Coordinate:
    return tuple(generator.randrange(extent + 1) for extent in shape)  # type: ignore[return-value]


def make_random_case(
    generator: random.Random,
    shape: Coordinate,
    update_count: int,
    query_count: int,
) -> tuple[Coordinate, list[Update], list[Query]]:
    updates: list[Update] = []
    if all(shape):
        for _ in range(update_count):
            updates.append(
                (
                    "S" if generator.randrange(3) == 0 else "A",
                    random_coordinate(generator, shape),
                    generator.randrange(-10**9, 10**9 + 1),
                )
            )

    queries: list[Query] = []
    for _ in range(query_count):
        kind = generator.randrange(3) if all(shape) else 1 + generator.randrange(2)
        if kind == 0:
            queries.append(("G", random_coordinate(generator, shape), None))
        elif kind == 1:
            queries.append(("P", random_corner(generator, shape), None))
        else:
            left = random_corner(generator, shape)
            right = tuple(
                generator.randrange(left[axis], shape[axis] + 1)
                for axis in range(3)
            )
            queries.append(("R", left, right))  # type: ignore[arg-type]
    return shape, updates, queries


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[Coordinate, list[Update], list[Query]]] = [
        ((0, 0, 0), [], [("P", (0, 0, 0), None), ("R", (0, 0, 0), (0, 0, 0))]),
        ((0, 5, 2), [], [("P", (0, 5, 2), None), ("R", (0, 1, 1), (0, 5, 2))]),
        (
            (1, 1, 1),
            [("S", (0, 0, 0), 7), ("A", (0, 0, 0), -2)],
            [
                ("G", (0, 0, 0), None),
                ("P", (1, 1, 1), None),
                ("R", (0, 0, 0), (1, 1, 1)),
                ("R", (1, 0, 0), (1, 1, 1)),
            ],
        ),
    ]

    generator = random.Random(2026071604)
    for _ in range(30):
        shape = (
            generator.randrange(0, 11),
            generator.randrange(0, 11),
            generator.randrange(0, 11),
        )
        cases.append(make_random_case(generator, shape, 150, 150))
    cases.append(make_random_case(generator, (15, 15, 15), 5000, 1000))

    for index, case in enumerate(cases):
        write_case(args.out_dir, index, *case)


if __name__ == "__main__":
    main()
