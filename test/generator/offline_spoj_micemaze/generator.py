#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_CELLS = 100

Edge = tuple[int, int, int]
Case = tuple[int, int, int, list[Edge]]


def escaped_count_oracle(case: Case) -> int:
    """Independent Floyd--Warshall oracle."""
    cell_count, exit_cell, time_limit, edges = case
    infinity = 10**30
    distance = [[infinity] * cell_count for _ in range(cell_count)]
    for cell in range(cell_count):
        distance[cell][cell] = 0
    for source, destination, travel_time in edges:
        distance[source][destination] = min(
            distance[source][destination],
            travel_time,
        )
    for middle in range(cell_count):
        for source in range(cell_count):
            for destination in range(cell_count):
                candidate = distance[source][middle] + distance[middle][destination]
                if candidate < distance[source][destination]:
                    distance[source][destination] = candidate
    return sum(
        distance[cell][exit_cell] <= time_limit
        for cell in range(cell_count)
    )


def validate_case(case: Case) -> None:
    cell_count, exit_cell, time_limit, edges = case
    assert 1 <= cell_count <= MAX_CELLS
    assert 0 <= exit_cell < cell_count
    assert time_limit >= 0
    assert all(
        0 <= source < cell_count
        and 0 <= destination < cell_count
        and travel_time > 0
        for source, destination, travel_time in edges
    )


def write_case(
    out_dir: Path,
    suite_name: str,
    case: Case,
    *,
    final_newline: bool = True,
) -> None:
    validate_case(case)
    cell_count, exit_cell, time_limit, edges = case
    lines = [
        str(cell_count),
        str(exit_cell + 1),
        str(time_limit),
        str(len(edges)),
    ]
    lines.extend(
        f"{source + 1} {destination + 1} {travel_time}"
        for source, destination, travel_time in edges
    )
    input_data = "\n".join(lines)
    if final_newline:
        input_data += "\n"
    (out_dir / f"{suite_name}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{suite_name}.out").write_text(
        f"{escaped_count_oracle(case)}\n",
        encoding="ascii",
    )


def random_case(rng: random.Random, cell_count: int) -> Case:
    exit_cell = rng.randrange(cell_count)
    time_limit = rng.randint(0, 100)
    edges: list[Edge] = []
    for source in range(cell_count):
        for destination in range(cell_count):
            if source != destination and rng.randrange(4) == 0:
                edges.append((source, destination, rng.randint(1, 40)))
    for _ in range(cell_count // 2):
        source = rng.randrange(cell_count)
        destination = rng.randrange(cell_count)
        edges.append((source, destination, rng.randint(1, 40)))
    return cell_count, exit_cell, time_limit, edges


def maximum_case() -> Case:
    cell_count = MAX_CELLS
    exit_cell = 73
    time_limit = 37
    edges = [
        (
            source,
            destination,
            (source * 97 + destination * 193 + 17) % 1000 + 1,
        )
        for source in range(cell_count)
        for destination in range(cell_count)
        if source != destination
    ]
    assert len(edges) == MAX_CELLS * (MAX_CELLS - 1)
    return cell_count, exit_cell, time_limit, edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official: Case = (
        4,
        1,
        1,
        [
            (0, 1, 1),
            (0, 2, 1),
            (1, 0, 1),
            (1, 3, 1),
            (2, 0, 1),
            (2, 3, 1),
            (3, 1, 1),
            (3, 2, 1),
        ],
    )
    assert escaped_count_oracle(official) == 3
    write_case(out_dir, "official_sample", official)

    edge_cases: list[tuple[str, Case]] = [
        ("single_exit_zero_time", (1, 0, 0, [])),
        (
            "directed_orientation",
            (3, 2, 5, [(0, 2, 5), (2, 1, 1)]),
        ),
        (
            "exact_time_limit",
            (4, 3, 7, [(0, 1, 3), (1, 3, 4), (2, 3, 8)]),
        ),
        (
            "parallel_connections",
            (3, 2, 4, [(0, 2, 9), (0, 2, 4), (1, 0, 1)]),
        ),
    ]
    for suite_name, case in edge_cases:
        write_case(out_dir, suite_name, case)

    rng = random.Random(1845)
    for index in range(24):
        write_case(
            out_dir,
            f"random_{index:02d}",
            random_case(rng, rng.randint(1, 12)),
        )

    write_case(
        out_dir,
        "without_final_newline",
        (3, 1, 6, [(0, 1, 6), (2, 0, 1)]),
        final_newline=False,
    )

    write_case(out_dir, "maximum_cells_and_connections", maximum_case())


if __name__ == "__main__":
    main()
