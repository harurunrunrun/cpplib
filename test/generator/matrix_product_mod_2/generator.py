#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def multiply_naive(lhs: list[str], rhs: list[str]) -> list[str]:
    rows = len(lhs)
    inner = len(rhs)
    columns = len(rhs[0]) if rhs else 0
    result: list[str] = []
    for row in range(rows):
        values: list[str] = []
        for column in range(columns):
            value = 0
            for middle in range(inner):
                value ^= (lhs[row][middle] == "1") and (rhs[middle][column] == "1")
            values.append("1" if value else "0")
        result.append("".join(values))
    return result


def write_case(
    out_dir: Path,
    index: int,
    lhs: list[str],
    rhs: list[str],
) -> None:
    rows = len(lhs)
    inner = len(rhs)
    columns = len(rhs[0]) if rhs else 0
    input_path = out_dir / f"case_{index:02d}.in"
    with input_path.open("w", encoding="utf-8", newline="\n") as output:
        output.write(f"{rows} {inner} {columns}\n")
        output.writelines(line + "\n" for line in lhs)
        output.writelines(line + "\n" for line in rhs)
    result = multiply_naive(lhs, rhs)
    with (out_dir / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output:
        output.writelines(line + "\n" for line in result)


def write_max_case(out_dir: Path, index: int) -> None:
    size = 4096
    input_path = out_dir / f"case_{index:02d}.in"
    output_path = out_dir / f"case_{index:02d}.out"
    with input_path.open("w", encoding="utf-8", newline="\n") as input_file, \
         output_path.open("w", encoding="utf-8", newline="\n") as output_file:
        input_file.write(f"{size} {size} {size}\n")
        for row in range(size):
            next_column = (row + 1) % size
            line = "".join(
                "1" if column == row or column == next_column else "0"
                for column in range(size)
            )
            input_file.write(line + "\n")
            output_file.write(line + "\n")
        for row in range(size):
            input_file.write("0" * row + "1" + "0" * (size - row - 1) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, ["0"], ["0"])
    write_case(args.out_dir, 1, ["1"], ["1"])
    write_case(args.out_dir, 2, ["101", "011"], ["10", "11", "01"])

    random_source = random.Random(2026071310)
    for index in range(3, 35):
        rows = random_source.randint(1, 18)
        inner = random_source.randint(1, 18)
        columns = random_source.randint(1, 18)
        lhs = [
            "".join(random_source.choice("01") for _ in range(inner))
            for _ in range(rows)
        ]
        rhs = [
            "".join(random_source.choice("01") for _ in range(columns))
            for _ in range(inner)
        ]
        write_case(args.out_dir, index, lhs, rhs)

    write_max_case(args.out_dir, 35)


if __name__ == "__main__":
    main()
