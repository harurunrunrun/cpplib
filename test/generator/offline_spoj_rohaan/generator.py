#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 50
MAX_STATIONS = 50
MAX_ORDERS = 50
MAX_DISTANCE = 1_000_000
MAX_TOTAL = 10_000_000

Case = tuple[list[list[int]], list[tuple[int, int]]]


def dijkstra(matrix: list[list[int]], source: int) -> list[int]:
    """Independent positive-weight oracle; it does not use Floyd--Warshall."""
    station_count = len(matrix)
    infinity = 10**30
    distance = [infinity] * station_count
    used = [False] * station_count
    distance[source] = 0
    for _ in range(station_count):
        vertex = min(
            (index for index in range(station_count) if not used[index]),
            key=distance.__getitem__,
        )
        used[vertex] = True
        for destination, weight in enumerate(matrix[vertex]):
            candidate = distance[vertex] + weight
            if candidate < distance[destination]:
                distance[destination] = candidate
    return distance


def solve_case(matrix: list[list[int]], orders: list[tuple[int, int]]) -> int:
    shortest: dict[int, list[int]] = {}
    result = 0
    for source, destination in orders:
        zero_based_source = source - 1
        if zero_based_source not in shortest:
            shortest[zero_based_source] = dijkstra(matrix, zero_based_source)
        result += shortest[zero_based_source][destination - 1]
    return result


def validate_case(matrix: list[list[int]], orders: list[tuple[int, int]]) -> None:
    station_count = len(matrix)
    assert 1 <= station_count <= MAX_STATIONS
    assert len(matrix) == station_count
    for source, row in enumerate(matrix):
        assert len(row) == station_count
        for destination, value in enumerate(row):
            assert 0 <= value <= MAX_DISTANCE
            if source != destination:
                assert value >= 1
    assert 1 <= len(orders) <= MAX_ORDERS
    assert all(
        1 <= source <= station_count and 1 <= destination <= station_count
        for source, destination in orders
    )


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[Case],
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= MAX_TEST_COUNT
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for test_case, (matrix, orders) in enumerate(cases, start=1):
        validate_case(matrix, orders)
        answer = solve_case(matrix, orders)
        assert 0 <= answer <= MAX_TOTAL
        input_lines.append(str(len(matrix)))
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        input_lines.append(str(len(orders)))
        input_lines.extend(f"{source} {destination}" for source, destination in orders)
        output_lines.append(f"Case #{test_case}: {answer}")

    input_data = "\n".join(input_lines)
    if final_newline:
        input_data += "\n"
    (out_dir / f"{name}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def random_matrix(rng: random.Random, station_count: int, upper: int) -> list[list[int]]:
    return [
        [
            0 if source == destination else rng.randint(1, upper)
            for destination in range(station_count)
        ]
        for source in range(station_count)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official: Case = (
        [[0, 1, 1], [1, 0, 1], [1, 9, 0]],
        [(2, 1), (3, 2)],
    )
    assert solve_case(*official) == 3
    write_suite(out_dir, "official_sample", [official])

    edge_cases: list[Case] = [
        ([[0]], [(1, 1)]),
        (
            [[0, 100, 1], [1, 0, 100], [100, 1, 0]],
            [(1, 2), (2, 1), (3, 2)],
        ),
        (
            [[0, 8, 30, 100], [9, 0, 7, 40], [20, 6, 0, 5], [4, 50, 3, 0]],
            [(1, 4), (1, 4), (4, 2), (2, 3), (3, 1)],
        ),
    ]
    write_suite(out_dir, "edge_and_directed", edge_cases)

    rng = random.Random(15285)
    random_cases: list[Case] = []
    for _ in range(24):
        station_count = rng.randint(1, 8)
        matrix = random_matrix(rng, station_count, 80)
        orders = [
            (rng.randint(1, station_count), rng.randint(1, station_count))
            for _ in range(rng.randint(1, MAX_ORDERS))
        ]
        random_cases.append((matrix, orders))
    write_suite(out_dir, "random_small_independent_oracle", random_cases)

    write_suite(
        out_dir,
        "without_final_newline",
        [([[0, 5], [7, 0]], [(1, 2), (2, 1)])],
        final_newline=False,
    )

    maximum_cases: list[Case] = []
    maximum_matrix = [
        [0 if source == destination else MAX_DISTANCE for destination in range(MAX_STATIONS)]
        for source in range(MAX_STATIONS)
    ]
    maximum_cases.append(
        (
            maximum_matrix,
            [(1, 2)] + [(index, index) for index in range(1, MAX_ORDERS)],
        )
    )
    for test_case in range(1, MAX_TEST_COUNT):
        matrix = random_matrix(rng, MAX_STATIONS, 50_000)
        orders = [
            (
                (7 * order + 11 * test_case) % MAX_STATIONS + 1,
                (19 * order + 3 * test_case + 1) % MAX_STATIONS + 1,
            )
            for order in range(MAX_ORDERS)
        ]
        maximum_cases.append((matrix, orders))
    write_suite(out_dir, "maximum_constraints", maximum_cases)


if __name__ == "__main__":
    main()
