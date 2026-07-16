#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Query = tuple[int, int, int]


def write_case(
    output_directory: Path,
    name: str,
    values: list[int],
    queries: list[Query],
) -> None:
    input_lines = [
        str(len(values)),
        " ".join(map(str, values)),
        str(len(queries)),
    ]
    input_lines.extend(
        f"{left} {right} {threshold}"
        for left, right, threshold in queries
    )
    answers = [
        str(sum(value > threshold for value in values[left:right]))
        for left, right, threshold in queries
    ]
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(answers) + ("\n" if answers else ""),
        encoding="utf-8",
    )


def random_queries(
    size: int,
    count: int,
    rng: random.Random,
) -> list[Query]:
    queries: list[Query] = []
    for _ in range(count):
        left = rng.randrange(size + 1)
        right = rng.randrange(left, size + 1)
        threshold = rng.randrange(-1_000_000_000, 1_000_000_001)
        queries.append((left, right, threshold))
    return queries


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    output_directory = Path(parser.parse_args().out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    write_case(
        output_directory,
        "case_00_empty",
        [],
        [(0, 0, -2_147_483_648), (0, 0, 0), (0, 0, 2_147_483_647)],
    )
    write_case(
        output_directory,
        "case_01_boundaries",
        [-2_147_483_648, -4, -4, 0, 7, 2_147_483_647],
        [
            (0, 6, -2_147_483_648),
            (0, 6, 2_147_483_647),
            (1, 5, -4),
            (2, 2, 0),
            (5, 6, 2_147_483_646),
        ],
    )

    rng = random.Random(2026071601)
    for case, size in enumerate((1, 2, 17, 128, 1024), start=2):
        values = [
            rng.randrange(-1_000_000_000, 1_000_000_001)
            for _ in range(size)
        ]
        queries = random_queries(size, 300, rng)
        queries.extend(
            [
                (0, size, -2_147_483_648),
                (0, size, 2_147_483_647),
                (0, 0, 0),
                (size, size, 0),
            ]
        )
        write_case(
            output_directory,
            f"case_{case:02d}_random",
            values,
            queries,
        )

    maximum_size = 30_000
    maximum_values = list(range(maximum_size))
    rng.shuffle(maximum_values)
    write_case(
        output_directory,
        "case_07_maximum_size",
        maximum_values,
        random_queries(maximum_size, 80, rng)
        + [(0, maximum_size, -1), (0, maximum_size, maximum_size - 1)],
    )


if __name__ == "__main__":
    main()
