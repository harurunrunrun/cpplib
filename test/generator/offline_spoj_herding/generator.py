#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(grid: list[str]) -> int:
    rows = len(grid)
    columns = len(grid[0])
    delta = {"N": (-1, 0), "S": (1, 0), "W": (0, -1), "E": (0, 1)}
    next_vertex = []
    for row in range(rows):
        for column in range(columns):
            dr, dc = delta[grid[row][column]]
            next_vertex.append((row + dr) * columns + column + dc)
    state = [0] * (rows * columns)
    answer = 0
    for start in range(rows * columns):
        if state[start]:
            continue
        vertex = start
        while state[vertex] == 0:
            state[vertex] = 1
            vertex = next_vertex[vertex]
        answer += state[vertex] == 1
        vertex = start
        while state[vertex] == 1:
            state[vertex] = 2
            vertex = next_vertex[vertex]
    return answer


def random_grid(source: random.Random, rows: int, columns: int) -> list[str]:
    result = []
    for row in range(rows):
        line = []
        for column in range(columns):
            choices = []
            if row: choices.append("N")
            if row + 1 < rows: choices.append("S")
            if column: choices.append("W")
            if column + 1 < columns: choices.append("E")
            line.append(source.choice(choices))
        result.append("".join(line))
    return result


def write_case(out_dir: Path, name: str, grid: list[str], answer: int | None = None) -> None:
    rows = len(grid)
    columns = len(grid[0])
    if answer is None:
        answer = solve(grid)
    (out_dir / f"{name}.in").write_text(f"{rows} {columns}\n" + "\n".join(grid) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "official", ["SWWW", "SEWN", "EEEN"], 2)
    source = random.Random(135)
    for case_index in range(40):
        rows = source.randint(2, 12)
        columns = source.randint(2, 12)
        write_case(out_dir, f"random_{case_index:02d}", random_grid(source, rows, columns))
    maximum = ["EW" * 500 for _ in range(1000)]
    write_case(out_dir, "maximum", maximum, 500000)


if __name__ == "__main__":
    main()
