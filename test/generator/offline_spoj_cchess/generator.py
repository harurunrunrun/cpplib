#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


BOARD_SIZE = 8
VERTEX_COUNT = BOARD_SIZE * BOARD_SIZE
INF = 10**18
MOVES = (
    (1, 2), (1, -2), (-1, 2), (-1, -2),
    (2, 1), (2, -1), (-2, 1), (-2, -1),
)
Query = tuple[int, int, int, int]


def vertex_id(x: int, y: int) -> int:
    return x * BOARD_SIZE + y


def all_pairs_floyd_warshall() -> list[list[int]]:
    distance = [[INF] * VERTEX_COUNT for _ in range(VERTEX_COUNT)]
    for vertex in range(VERTEX_COUNT):
        distance[vertex][vertex] = 0
    for x in range(BOARD_SIZE):
        for y in range(BOARD_SIZE):
            from_ = vertex_id(x, y)
            for dx, dy in MOVES:
                next_x = x + dx
                next_y = y + dy
                if not (0 <= next_x < BOARD_SIZE and 0 <= next_y < BOARD_SIZE):
                    continue
                to = vertex_id(next_x, next_y)
                distance[from_][to] = min(
                    distance[from_][to], x * next_x + y * next_y
                )

    for middle in range(VERTEX_COUNT):
        for from_ in range(VERTEX_COUNT):
            through_middle = distance[from_][middle]
            if through_middle == INF:
                continue
            for to in range(VERTEX_COUNT):
                candidate = through_middle + distance[middle][to]
                if candidate < distance[from_][to]:
                    distance[from_][to] = candidate
    return distance


def validate(queries: list[Query]) -> None:
    assert queries
    assert all(
        0 <= coordinate < BOARD_SIZE
        for query in queries
        for coordinate in query
    )


def answers(
    queries: list[Query], distance: list[list[int]]
) -> list[int]:
    result = []
    for start_x, start_y, target_x, target_y in queries:
        value = distance[
            vertex_id(start_x, start_y)
        ][vertex_id(target_x, target_y)]
        result.append(-1 if value == INF else value)
    return result


def write_case(
    out_dir: Path,
    name: str,
    queries: list[Query],
    distance: list[list[int]],
    *,
    crlf_without_final_newline: bool = False,
) -> None:
    validate(queries)
    input_lines = [" ".join(map(str, query)) for query in queries]
    if crlf_without_final_newline:
        (out_dir / f"{name}.in").write_bytes(
            "\r\n".join(input_lines).encode("ascii")
        )
    else:
        (out_dir / f"{name}.in").write_text(
            "\n".join(input_lines) + "\n",
            encoding="ascii",
        )
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers(queries, distance))) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    distance = all_pairs_floyd_warshall()
    write_case(
        out_dir,
        "official",
        [(2, 5, 5, 2), (4, 7, 3, 2), (1, 2, 3, 4)],
        distance,
    )

    exhaustive = [
        (start_x, start_y, target_x, target_y)
        for start_x in range(BOARD_SIZE)
        for start_y in range(BOARD_SIZE)
        for target_x in range(BOARD_SIZE)
        for target_y in range(BOARD_SIZE)
    ]
    write_case(out_dir, "all_4096_ordered_pairs", exhaustive, distance)

    maximum_distance = max(max(row) for row in distance)
    maximum_queries = [
        (source // BOARD_SIZE, source % BOARD_SIZE,
         target // BOARD_SIZE, target % BOARD_SIZE)
        for source in range(VERTEX_COUNT)
        for target in range(VERTEX_COUNT)
        if distance[source][target] == maximum_distance
    ]
    write_case(
        out_dir,
        "maximum_shortest_path_cost",
        maximum_queries,
        distance,
    )

    write_case(
        out_dir,
        "maximum_coordinates_and_edge_weights",
        [
            (7, 7, 0, 0), (0, 0, 7, 7), (7, 7, 7, 7),
            (7, 7, 5, 6), (5, 6, 7, 7),
            (7, 6, 5, 7), (5, 7, 7, 6),
        ],
        distance,
    )

    write_case(
        out_dir,
        "crlf_without_final_newline",
        [(0, 0, 0, 0), (7, 7, 0, 0), (2, 5, 5, 2)],
        distance,
        crlf_without_final_newline=True,
    )


if __name__ == "__main__":
    main()
