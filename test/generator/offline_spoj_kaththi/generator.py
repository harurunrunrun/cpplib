#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


Case = tuple[list[str], int | None]


def solve_independent(grid: list[str]) -> int:
    row_count = len(grid)
    column_count = len(grid[0])
    distance = [[10**9] * column_count for _ in range(row_count)]
    distance[0][0] = 0
    queue = [(0, 0, 0)]
    while queue:
        current, row, column = heapq.heappop(queue)
        if distance[row][column] != current:
            continue
        for delta_row, delta_column in ((-1, 0), (0, 1), (1, 0), (0, -1)):
            next_row = row + delta_row
            next_column = column + delta_column
            if not (0 <= next_row < row_count and 0 <= next_column < column_count):
                continue
            candidate = current + (grid[row][column] != grid[next_row][next_column])
            if candidate < distance[next_row][next_column]:
                distance[next_row][next_column] = candidate
                heapq.heappush(queue, (candidate, next_row, next_column))
    return distance[-1][-1]


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[Case],
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= 10
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for grid, answer in cases:
        row_count = len(grid)
        column_count = len(grid[0])
        assert 2 <= row_count <= 1000
        assert 2 <= column_count <= 1000
        assert all(len(row) == column_count for row in grid)
        input_lines.append(f"{row_count} {column_count}")
        input_lines.extend(grid)
        if answer is None:
            answer = solve_independent(grid)
        output_lines.append(str(answer))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(
        out_dir,
        "official",
        [
            (["aa", "aa"], 0),
            (["abc", "def"], 3),
            (["akaccc", "aaacfc", "amdfcc", "aokhdd", "zyxwdp", "zyxwdd"], 2),
            (["abbbc", "abacc", "aaacc", "aefci", "cdgdd"], 2),
        ],
    )

    rng = random.Random(100000112)
    random_cases: list[Case] = []
    for _ in range(200):
        row_count = rng.randint(2, 9)
        column_count = rng.randint(2, 9)
        alphabet_size = rng.randint(1, 6)
        grid = [
            "".join(chr(ord("a") + rng.randrange(alphabet_size)) for _ in range(column_count))
            for _ in range(row_count)
        ]
        random_cases.append((grid, None))
    for offset in range(0, len(random_cases), 10):
        write_suite(out_dir, f"random_{offset // 10:02d}", random_cases[offset:offset + 10])

    maximum_test_count = [
        ([chr(ord("a") + case) * 2, chr(ord("a") + case) * 2], 0)
        for case in range(10)
    ]
    write_suite(out_dir, "maximum_test_count", maximum_test_count)

    maximum_grid = [
        "".join("a" if (row + column) % 2 == 0 else "b" for column in range(1000))
        for row in range(1000)
    ]
    write_suite(
        out_dir,
        "maximum_grid",
        [(maximum_grid, 1998)],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
