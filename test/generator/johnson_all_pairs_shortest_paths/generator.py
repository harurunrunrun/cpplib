#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


INF_SENTINEL = 1 << 60
LLONG_MAX = (1 << 63) - 1
LLONG_MIN = -(1 << 63)


def floyd_warshall(
    vertex_count: int,
    edges: list[tuple[int, int, int]],
) -> list[list[int | None]]:
    distance: list[list[int | None]] = [
        [None] * vertex_count for _ in range(vertex_count)
    ]
    for vertex in range(vertex_count):
        distance[vertex][vertex] = 0
    for source, target, cost in edges:
        current = distance[source][target]
        if current is None or cost < current:
            distance[source][target] = cost
    for middle in range(vertex_count):
        for source in range(vertex_count):
            left = distance[source][middle]
            if left is None:
                continue
            for target in range(vertex_count):
                right = distance[middle][target]
                if right is None:
                    continue
                candidate = left + right
                current = distance[source][target]
                if current is None or candidate < current:
                    distance[source][target] = candidate
    return distance


def write_distance_case(
    out_dir: Path,
    index: int,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    queries: list[tuple[int, int]],
    answers: list[int | None] | None = None,
) -> None:
    if answers is None:
        distance = floyd_warshall(vertex_count, edges)
        assert all(
            distance[vertex][vertex] is not None
            and distance[vertex][vertex] >= 0
            for vertex in range(vertex_count)
        )
        answers = [distance[source][target] for source, target in queries]
    assert len(answers) == len(queries)
    assert all(
        value is None or LLONG_MIN <= value <= LLONG_MAX
        for value in answers
    )

    name = f"case_{index:03d}"
    input_lines = [
        f"DISTANCE {vertex_count} {len(edges)} {len(queries)}",
        *(f"{source} {target} {cost}" for source, target, cost in edges),
        *(f"{source} {target}" for source, target in queries),
    ]
    output_lines = [
        "OK",
        *("INF" if value is None else str(value) for value in answers),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def write_exception_case(
    out_dir: Path,
    index: int,
    mode: str,
    vertex_count: int,
    edges: list[tuple[int, int, int]],
    expected: str,
) -> None:
    name = f"case_{index:03d}"
    input_lines = [
        f"{mode} {vertex_count} {len(edges)} 0",
        *(f"{source} {target} {cost}" for source, target, cost in edges),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        expected + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    index = 0
    write_distance_case(out_dir, index, 0, [], [])
    index += 1
    write_distance_case(out_dir, index, 1, [(0, 0, 0), (0, 0, 7)], [(0, 0)])
    index += 1
    write_distance_case(
        out_dir,
        index,
        5,
        [(0, 1, 4), (0, 1, 2), (1, 2, -5), (2, 3, 4), (3, 3, 0)],
        [(0, 0), (0, 1), (0, 3), (3, 0), (4, 4), (4, 0)],
    )
    index += 1
    write_distance_case(
        out_dir, index, 2, [(0, 1, INF_SENTINEL)], [(0, 1), (1, 0)]
    )
    index += 1
    write_distance_case(
        out_dir,
        index,
        7,
        [
            (0, 1, -4), (0, 2, 3), (1, 3, 8), (1, 4, -2),
            (2, 4, -7), (3, 5, 1), (4, 5, 6), (5, 6, -3),
        ],
        [(source, target) for source in range(7) for target in range(7)],
    )
    index += 1

    write_exception_case(
        out_dir, index, "NEGATIVE", 4, [(2, 3, -2), (3, 2, 1)],
        "NEGATIVE_CYCLE"
    )
    index += 1
    write_exception_case(
        out_dir, index, "NEGATIVE", 1, [(0, 0, -1)], "NEGATIVE_CYCLE"
    )
    index += 1
    write_exception_case(
        out_dir,
        index,
        "OVERFLOW",
        3,
        [(0, 1, LLONG_MAX), (1, 2, 1)],
        "OVERFLOW",
    )
    index += 1
    write_exception_case(
        out_dir,
        index,
        "OVERFLOW",
        3,
        [(0, 1, LLONG_MIN), (1, 2, -1)],
        "OVERFLOW",
    )
    index += 1

    rng = random.Random(20260722)
    for _ in range(36):
        vertex_count = rng.randint(2, 18)
        potential = [rng.randint(-500, 500) for _ in range(vertex_count)]
        edges: list[tuple[int, int, int]] = []
        for source in range(vertex_count):
            if rng.randrange(3) == 0:
                edges.append((source, source, rng.randint(0, 30)))
            for target in range(vertex_count):
                if source == target or rng.randrange(5) != 0:
                    continue
                reduced_cost = rng.randint(0, 80)
                cost = (
                    reduced_cost - potential[source] + potential[target]
                )
                edges.append((source, target, cost))
                if rng.randrange(6) == 0:
                    edges.append((source, target, cost + rng.randint(0, 20)))
        rng.shuffle(edges)
        if vertex_count <= 8:
            queries = [
                (source, target)
                for source in range(vertex_count)
                for target in range(vertex_count)
            ]
        else:
            queries = [
                (rng.randrange(vertex_count), rng.randrange(vertex_count))
                for _ in range(80)
            ]
        write_distance_case(
            out_dir, index, vertex_count, edges, queries
        )
        index += 1

    # A sparse case whose all-pairs output is still manageable, but for which
    # an accidental cubic Floyd--Warshall implementation is prohibitively slow.
    vertex_count = 2_000
    edges = [
        (vertex, vertex + 1, vertex % 11 - 5)
        for vertex in range(vertex_count - 1)
    ]
    prefix = [0]
    for _, _, cost in edges:
        prefix.append(prefix[-1] + cost)
    queries = [
        (0, vertex_count - 1),
        (0, 1_000),
        (123, 1_987),
        (1_999, 1_999),
        (1_999, 0),
        (1_000, 999),
    ]
    answers = [
        prefix[target] - prefix[source] if source <= target else None
        for source, target in queries
    ]
    write_distance_case(
        out_dir, index, vertex_count, edges, queries, answers
    )


if __name__ == "__main__":
    main()
