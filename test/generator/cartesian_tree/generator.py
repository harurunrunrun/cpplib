#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def build(values: list[int], maximum: bool) -> tuple[int, list[int], list[int], list[int]]:
    n = len(values)
    parent = [-1] * n
    left = [-1] * n
    right = [-1] * n
    stack: list[int] = []

    def better(a: int, b: int) -> bool:
        return a > b if maximum else a < b

    for index, value in enumerate(values):
        last = -1
        while stack and better(value, values[stack[-1]]):
            last = stack.pop()
        if stack:
            parent[index] = stack[-1]
            right[stack[-1]] = index
        if last != -1:
            parent[last] = index
            left[index] = last
        stack.append(index)

    return (-1 if not stack else stack[0]), parent, left, right


def format_vector(values: list[int]) -> str:
    return " ".join([str(len(values)), *map(str, values)])


def solve(cases: list[tuple[str, list[int]]]) -> str:
    output: list[str] = []
    for mode, values in cases:
        root, parent, left, right = build(values, mode == "MAX")
        output.append(f"{root} {len(values)} {int(not values)}")
        output.append(format_vector(parent))
        output.append(format_vector(left))
        output.append(format_vector(right))
    return "\n".join(output) + "\n"


def write_case(
    out_dir: Path,
    index: int,
    cases: list[tuple[str, list[int]]],
) -> None:
    stem = f"case_{index:02d}"
    lines = [str(len(cases))]
    for mode, values in cases:
        lines.append(" ".join([mode, str(len(values)), *map(str, values)]))
    (out_dir / f"{stem}.in").write_text(
        "\n".join(lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        solve(cases),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ("MIN", []),
        ("MIN", [3, 1, 1, 2]),
        ("MAX", [3, 1, 1, 2]),
        ("MIN", [1, 1, 1, 1]),
        ("MAX", [1, 1, 1, 1]),
        ("MIN", [1, 2, 3, 4, 5]),
        ("MIN", [5, 4, 3, 2, 1]),
        ("MAX", [1, 2, 3, 4, 5]),
        ("MAX", [5, 4, 3, 2, 1]),
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20261018)
    for case_index in range(1, 4):
        cases: list[tuple[str, list[int]]] = []
        for _ in range(250):
            n = rng.randrange(0, 101)
            mode = rng.choice(["MIN", "MAX"])
            values = [rng.randrange(-10, 11) for _ in range(n)]
            cases.append((mode, values))
        write_case(out_dir, case_index, cases)


if __name__ == "__main__":
    main()
