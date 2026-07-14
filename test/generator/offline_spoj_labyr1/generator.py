#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def solve(grid: list[str]) -> int:
    rows = len(grid)
    columns = len(grid[0])
    open_cells = [
        (row, column)
        for row in range(rows)
        for column in range(columns)
        if grid[row][column] == "."
    ]
    answer = 0
    for start in open_cells:
        distance = {start: 0}
        queue = deque([start])
        while queue:
            row, column = queue.popleft()
            for delta_row, delta_column in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                next_cell = row + delta_row, column + delta_column
                next_row, next_column = next_cell
                if (
                    0 <= next_row < rows
                    and 0 <= next_column < columns
                    and grid[next_row][next_column] == "."
                    and next_cell not in distance
                ):
                    distance[next_cell] = distance[(row, column)] + 1
                    queue.append(next_cell)
        answer = max(answer, max(distance.values(), default=0))
    return answer


def perfect_maze(source: random.Random, logical_rows: int, logical_columns: int) -> list[str]:
    rows = logical_rows * 2 - 1
    columns = logical_columns * 2 - 1
    grid = [["#"] * columns for _ in range(rows)]
    seen = [[False] * logical_columns for _ in range(logical_rows)]
    seen[0][0] = True
    grid[0][0] = "."
    stack = [(0, 0)]
    while stack:
        row, column = stack[-1]
        choices = []
        for delta_row, delta_column in ((-1, 0), (1, 0), (0, -1), (0, 1)):
            next_row = row + delta_row
            next_column = column + delta_column
            if (
                0 <= next_row < logical_rows
                and 0 <= next_column < logical_columns
                and not seen[next_row][next_column]
            ):
                choices.append((next_row, next_column))
        if not choices:
            stack.pop()
            continue
        next_row, next_column = source.choice(choices)
        seen[next_row][next_column] = True
        grid[row + next_row][column + next_column] = "."
        grid[next_row * 2][next_column * 2] = "."
        stack.append((next_row, next_column))
    return ["".join(line) for line in grid]


def write_case(
    out_dir: Path,
    name: str,
    grids: list[list[str]],
    answers: list[int] | None = None,
) -> None:
    input_lines = [str(len(grids))]
    output_lines = []
    for index, grid in enumerate(grids):
        rows = len(grid)
        columns = len(grid[0])
        input_lines.append(f"{columns} {rows}")
        input_lines.extend(grid)
        answer = solve(grid) if answers is None else answers[index]
        output_lines.append(f"Maximum rope length is {answer}.")
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "basic", [
        ["."],
        ["..."],
        [".#.", "###", ".#."],
        [".....", "####.", "....."],
    ])

    source = random.Random(140)
    grids = [
        perfect_maze(source, source.randint(1, 10), source.randint(1, 10))
        for _ in range(35)
    ]
    write_case(out_dir, "random", grids)

    maximum = []
    for row in range(1000):
        if row % 2 == 0:
            maximum.append("." * 1000)
        elif (row // 2) % 2 == 0:
            maximum.append("#" * 999 + ".")
        else:
            maximum.append("." + "#" * 999)
    write_case(out_dir, "maximum", [maximum], [500499])


if __name__ == "__main__":
    main()
