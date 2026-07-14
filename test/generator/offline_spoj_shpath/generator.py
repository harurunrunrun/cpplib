#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 10
MAX_CITIES = 10_000
MAX_NAME_LENGTH = 10
MAX_QUERIES = 100
MAX_PATH_COST = 200_000

Graph = list[list[tuple[int, int]]]
Query = tuple[int, int]
Case = tuple[list[str], Graph, list[Query]]


def floyd_warshall_oracle(graph: Graph) -> list[list[int]]:
    """Independent oracle for small cases."""
    city_count = len(graph)
    infinity = 10**30
    distance = [[infinity] * city_count for _ in range(city_count)]
    for city in range(city_count):
        distance[city][city] = 0
    for source, edges in enumerate(graph):
        for destination, cost in edges:
            distance[source][destination] = min(
                distance[source][destination],
                cost,
            )
    for middle in range(city_count):
        for source in range(city_count):
            for destination in range(city_count):
                candidate = distance[source][middle] + distance[middle][destination]
                if candidate < distance[source][destination]:
                    distance[source][destination] = candidate
    return distance


def solve_small(case: Case) -> list[int]:
    _, graph, queries = case
    distance = floyd_warshall_oracle(graph)
    return [distance[source][destination] for source, destination in queries]


def validate_case(case: Case) -> None:
    names, graph, queries = case
    city_count = len(names)
    assert 1 <= city_count <= MAX_CITIES
    assert len(set(names)) == city_count
    assert len(graph) == city_count
    assert all(
        1 <= len(name) <= MAX_NAME_LENGTH
        and name.isascii()
        and name.isalpha()
        and name.islower()
        for name in names
    )
    for edges in graph:
        for destination, cost in edges:
            assert 0 <= destination < city_count
            assert cost > 0
    assert 1 <= len(queries) <= MAX_QUERIES
    assert all(
        0 <= source < city_count and 0 <= destination < city_count
        for source, destination in queries
    )


def write_suite(
    out_dir: Path,
    suite_name: str,
    cases: list[Case],
    expected: list[list[int]] | None = None,
    *,
    blank_between_cases: bool = False,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= MAX_TEST_COUNT
    if expected is None:
        expected = [solve_small(case) for case in cases]
    assert len(expected) == len(cases)

    case_blocks: list[str] = []
    output_lines: list[str] = []
    for case, answers in zip(cases, expected):
        validate_case(case)
        names, graph, queries = case
        assert len(answers) == len(queries)
        assert all(0 <= answer <= MAX_PATH_COST for answer in answers)
        lines = [str(len(names))]
        for city, city_name in enumerate(names):
            lines.append(city_name)
            lines.append(str(len(graph[city])))
            lines.extend(
                f"{destination + 1} {cost}"
                for destination, cost in graph[city]
            )
        lines.append(str(len(queries)))
        lines.extend(
            f"{names[source]} {names[destination]}"
            for source, destination in queries
        )
        case_blocks.append("\n".join(lines))
        output_lines.extend(map(str, answers))

    separator = "\n\n" if blank_between_cases else "\n"
    input_data = str(len(cases)) + separator + separator.join(case_blocks)
    if final_newline:
        input_data += "\n"
    (out_dir / f"{suite_name}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{suite_name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def encoded_name(index: int, prefix: str) -> str:
    suffix = []
    for _ in range(4):
        suffix.append(chr(ord("a") + index % 26))
        index //= 26
    return prefix + "".join(reversed(suffix))


def strongly_connected_case(
    rng: random.Random,
    city_count: int,
    query_count: int,
    prefix: str,
) -> Case:
    names = [encoded_name(city, prefix) for city in range(city_count)]
    graph: Graph = [[] for _ in range(city_count)]
    if city_count > 1:
        for city in range(city_count):
            graph[city].append(((city + 1) % city_count, rng.randint(1, 30)))
    for _ in range(3 * city_count + 2):
        source = rng.randrange(city_count)
        destination = rng.randrange(city_count)
        graph[source].append((destination, rng.randint(1, 60)))
    queries = [(0, 0)]
    queries.extend(
        (rng.randrange(city_count), rng.randrange(city_count))
        for _ in range(query_count - 1)
    )
    return names, graph, queries


def maximum_case() -> tuple[Case, list[int]]:
    city_count = MAX_CITIES
    names = [encoded_name(city, "m") for city in range(city_count)]
    graph: Graph = [[] for _ in range(city_count)]
    for city in range(city_count):
        if city + 1 < city_count:
            graph[city].append((city + 1, 2))
        if city > 0:
            graph[city].append((city - 1, 3))
        if city + 50 < city_count:
            graph[city].append((city + 50, 101))
        if city >= 50:
            graph[city].append((city - 50, 151))

    queries: list[Query] = [
        (0, city_count - 1),
        (city_count - 1, 0),
        (city_count // 2, city_count // 2),
    ]
    for query in range(MAX_QUERIES - len(queries)):
        source = (97 * query + 13) % city_count
        destination = (503 * query + 71) % city_count
        queries.append((source, destination))

    answers = [
        2 * (destination - source)
        if source <= destination
        else 3 * (source - destination)
        for source, destination in queries
    ]
    return (names, graph, queries), answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official: Case = (
        ["gdansk", "bydgoszcz", "torun", "warszawa"],
        [
            [(1, 1), (2, 3)],
            [(0, 1), (2, 1), (3, 4)],
            [(0, 3), (1, 1), (3, 1)],
            [(1, 4), (2, 1)],
        ],
        [(0, 3), (1, 3)],
    )
    assert solve_small(official) == [3, 2]
    write_suite(out_dir, "official_sample", [official])

    edge_cases: list[Case] = [
        (
            ["alone"],
            [[(0, 7)]],
            [(0, 0)],
        ),
        (
            ["alpha", "beta"],
            [[(1, 9), (1, 2)], [(0, 3)]],
            [(0, 0), (0, 1), (1, 0), (0, 1)],
        ),
    ]
    write_suite(
        out_dir,
        "blank_lines_between_cases",
        edge_cases,
        blank_between_cases=True,
    )

    rng = random.Random(341)
    random_cases = [
        strongly_connected_case(
            rng,
            city_count=index + 1,
            query_count=min(MAX_QUERIES, 12 + index),
            prefix=chr(ord("a") + index),
        )
        for index in range(MAX_TEST_COUNT)
    ]
    write_suite(out_dir, "maximum_test_count_random_oracle", random_cases)

    eof_case = strongly_connected_case(rng, 7, 20, "z")
    write_suite(
        out_dir,
        "without_final_newline",
        [eof_case],
        final_newline=False,
    )

    stress_case, stress_answers = maximum_case()
    write_suite(
        out_dir,
        "maximum_cities_and_queries",
        [stress_case],
        [stress_answers],
    )


if __name__ == "__main__":
    main()
