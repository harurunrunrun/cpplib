#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def visible(grid: list[str], first: tuple[int, int], second: tuple[int, int]) -> bool:
    r1, c1 = first
    r2, c2 = second
    if r1 == r2:
        return all(grid[r1][column] != "A" for column in range(min(c1, c2) + 1, max(c1, c2)))
    if c1 == c2:
        return all(grid[row][c1] != "A" for row in range(min(r1, r2) + 1, max(r1, r2)))
    return False


def solve(grid: list[str]) -> int:
    vacant = [(row, column) for row in range(len(grid)) for column in range(len(grid[0])) if grid[row][column] == "H"]
    assert len(vacant) <= 18
    answer = 0
    for mask in range(1 << len(vacant)):
        if mask.bit_count() <= answer:
            continue
        chosen = [vacant[index] for index in range(len(vacant)) if mask >> index & 1]
        if all(not visible(grid, chosen[i], chosen[j]) for i in range(len(chosen)) for j in range(i + 1, len(chosen))):
            answer = len(chosen)
    return answer


def write_case(out_dir: Path, name: str, grids: list[tuple[list[str], int | None]]) -> None:
    lines = [str(len(grids))]
    answers = []
    for grid, answer in grids:
        row_count = len(grid)
        column_count = len(grid[0])
        assert 1 <= row_count <= 300 and 1 <= column_count <= 300
        lines.append(f"{row_count} {column_count}")
        lines.extend(" ".join(row) for row in grid)
        answers.append(str(solve(grid) if answer is None else answer))
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(answers) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "basic", [(["HHHHHHH"] * 4, 4), (["HAH", "HDH", "HAH"], None)])
    source = random.Random(132)
    grids = []
    for _ in range(35):
        rows = source.randint(1, 4)
        columns = source.randint(1, 4)
        grid = ["".join(source.choices("HAD", weights=(5, 2, 2), k=columns)) for _ in range(rows)]
        grids.append((grid, None))
    write_case(out_dir, "random", grids)
    write_case(out_dir, "maximum", [(["H" * 300 for _ in range(300)], 300)])


if __name__ == "__main__":
    main()
