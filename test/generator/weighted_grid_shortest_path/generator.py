#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def shortest_path(
    cost: list[list[int]],
    start: tuple[int, int],
    target: tuple[int, int],
) -> int:
    rows = len(cost)
    columns = len(cost[0])
    infinity = 10**100
    distance = [[infinity] * columns for _ in range(rows)]
    sr, sc = start
    distance[sr][sc] = cost[sr][sc]
    queue = [(cost[sr][sc], sr, sc)]
    while queue:
        current, row, column = heapq.heappop(queue)
        if current != distance[row][column]:
            continue
        if (row, column) == target:
            return current
        for dr, dc in ((-1, 0), (0, 1), (1, 0), (0, -1)):
            nr, nc = row + dr, column + dc
            if not (0 <= nr < rows and 0 <= nc < columns):
                continue
            candidate = current + cost[nr][nc]
            if candidate < distance[nr][nc]:
                distance[nr][nc] = candidate
                heapq.heappush(queue, (candidate, nr, nc))
    raise AssertionError("rectangular grid is connected")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    maximum = (1 << 63) - 1
    cases: list[
        tuple[list[list[int]], tuple[int, int], tuple[int, int]]
    ] = [
        ([[maximum]], (0, 0), (0, 0)),
        (
            [[maximum - 5, 10], [1, 1]],
            (0, 0),
            (1, 1),
        ),
        (
            [[5, 8, 1], [2, 100, 2], [9, 1, 3]],
            (2, 1),
            (0, 2),
        ),
    ]
    source = random.Random(0x6A71D)
    for _ in range(160):
        rows = source.randint(1, 8)
        columns = source.randint(1, 8)
        grid = [
            [source.randint(0, 30) for _ in range(columns)]
            for _ in range(rows)
        ]
        start = (source.randrange(rows), source.randrange(columns))
        target = (source.randrange(rows), source.randrange(columns))
        cases.append((grid, start, target))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for grid, start, target in cases:
        rows = len(grid)
        columns = len(grid[0])
        input_lines.append(
            f"{rows} {columns} {start[0]} {start[1]} {target[0]} {target[1]}"
        )
        input_lines.extend(" ".join(map(str, row)) for row in grid)
        output_lines.append(str(shortest_path(grid, start, target)))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
