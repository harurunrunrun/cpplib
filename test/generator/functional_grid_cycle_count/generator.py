#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def cycle_count(directions: list[str]) -> int:
    if not directions:
        return 0
    row_count = len(directions)
    column_count = len(directions[0])
    next_vertex: list[int] = []
    for row, line in enumerate(directions):
        for column, direction in enumerate(line):
            next_row = row
            next_column = column
            if direction == "N":
                next_row -= 1
            elif direction == "S":
                next_row += 1
            elif direction == "W":
                next_column -= 1
            else:
                next_column += 1
            next_vertex.append(next_row * column_count + next_column)

    state = [0] * (row_count * column_count)
    answer = 0
    traversal = 0
    for start in range(len(state)):
        if state[start] == -1:
            continue
        traversal += 1
        vertex = start
        while state[vertex] == 0:
            state[vertex] = traversal
            vertex = next_vertex[vertex]
        if state[vertex] == traversal:
            answer += 1
        vertex = start
        while state[vertex] == traversal:
            state[vertex] = -1
            vertex = next_vertex[vertex]
    return answer


def random_grid(
    source: random.Random,
    row_count: int,
    column_count: int,
) -> list[str]:
    result: list[str] = []
    for row in range(row_count):
        line = []
        for column in range(column_count):
            options = []
            if row > 0:
                options.append("N")
            if row + 1 < row_count:
                options.append("S")
            if column > 0:
                options.append("W")
            if column + 1 < column_count:
                options.append("E")
            line.append(source.choice(options))
        result.append("".join(line))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[str]] = [
        [],
        ["EW"],
        ["S", "N"],
        ["ES", "NW"],
        ["EWS", "NWN"],
    ]
    source = random.Random(41002)
    for _ in range(160):
        row_count = source.randint(1, 8)
        column_count = source.randint(1, 8)
        if row_count == 1 and column_count == 1:
            column_count = 2
        cases.append(random_grid(source, row_count, column_count))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for directions in cases:
        row_count = len(directions)
        column_count = len(directions[0]) if directions else 0
        input_lines.append(f"{row_count} {column_count}")
        input_lines.extend(directions)
        output_lines.append(str(cycle_count(directions)))

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
