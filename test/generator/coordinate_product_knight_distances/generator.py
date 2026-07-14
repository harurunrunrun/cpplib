#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOVES = (
    (1, 2), (1, -2), (-1, 2), (-1, -2),
    (2, 1), (2, -1), (-2, 1), (-2, -1),
)


def all_pairs(board_size: int) -> list[list[int]]:
    vertex_count = board_size * board_size
    infinity = 10**30
    distance = [[infinity] * vertex_count for _ in range(vertex_count)]
    for vertex in range(vertex_count):
        distance[vertex][vertex] = 0
    for x in range(board_size):
        for y in range(board_size):
            source = x * board_size + y
            for dx, dy in MOVES:
                next_x = x + dx
                next_y = y + dy
                if 0 <= next_x < board_size and 0 <= next_y < board_size:
                    target = next_x * board_size + next_y
                    distance[source][target] = min(
                        distance[source][target],
                        x * next_x + y * next_y,
                    )
    for middle in range(vertex_count):
        for source in range(vertex_count):
            for target in range(vertex_count):
                distance[source][target] = min(
                    distance[source][target],
                    distance[source][middle] + distance[middle][target],
                )
    return distance


def write_case(
    out_dir: Path,
    name: str,
    board_size: int,
    queries: list[tuple[int, int, int, int]],
) -> None:
    distance = all_pairs(board_size)
    infinity = 10**30
    answers = []
    for start_x, start_y, target_x, target_y in queries:
        value = distance[
            start_x * board_size + start_y
        ][target_x * board_size + target_y]
        answers.append("-1" if value == infinity else str(value))
    input_lines = [
        f"{board_size} {len(queries)}",
        *(" ".join(map(str, query)) for query in queries),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(answers) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    for board_size in range(1, 9):
        queries = [
            (start_x, start_y, target_x, target_y)
            for start_x in range(board_size)
            for start_y in range(board_size)
            for target_x in range(board_size)
            for target_y in range(board_size)
        ]
        write_case(out_dir, f"exhaustive_{board_size}", board_size, queries)

    rng = random.Random(914387)
    for index in range(8, 24):
        board_size = rng.randint(1, 8)
        queries = [
            (
                rng.randrange(board_size),
                rng.randrange(board_size),
                rng.randrange(board_size),
                rng.randrange(board_size),
            )
            for _ in range(100)
        ]
        write_case(out_dir, f"random_{index:02d}", board_size, queries)


if __name__ == "__main__":
    main()
