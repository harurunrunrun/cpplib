#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MOD = 1_000_000_007
Matrix = tuple[int, int, int, int]
Query = tuple[str, tuple[int, ...]]
IDENTITY: Matrix = (1, 0, 0, 1)


def multiply(left: Matrix, right: Matrix) -> Matrix:
    return (
        (left[0] * right[0] + left[1] * right[2]) % MOD,
        (left[0] * right[1] + left[1] * right[3]) % MOD,
        (left[2] * right[0] + left[3] * right[2]) % MOD,
        (left[2] * right[1] + left[3] * right[3]) % MOD,
    )


def product(values: list[Matrix], left: int, right: int) -> Matrix:
    result = IDENTITY
    for index in range(left, right):
        result = multiply(result, values[index])
    return result


def write_case(
    out_dir: Path,
    index: int,
    initial: list[Matrix],
    queries: list[Query],
) -> None:
    values = initial.copy()
    input_lines = [f"{len(values)} {len(queries)}"]
    input_lines += [" ".join(map(str, matrix)) for matrix in values]
    output_lines: list[str] = []
    for kind, arguments in queries:
        input_lines.append(f"{kind} {' '.join(map(str, arguments))}".rstrip())
        if kind == "S":
            position = arguments[0]
            values[position] = tuple(arguments[1:])  # type: ignore[assignment]
        elif kind == "G":
            answer = values[arguments[0]]
            output_lines.append(" ".join(map(str, answer)))
        elif kind == "P":
            answer = product(values, 0, arguments[0])
            output_lines.append(" ".join(map(str, answer)))
        elif kind == "R":
            answer = product(values, arguments[0], arguments[1])
            output_lines.append(" ".join(map(str, answer)))
        else:
            answer = product(values, 0, len(values))
            output_lines.append(" ".join(map(str, answer)))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + ("\n" if output_lines else ""),
        encoding="utf-8",
    )


def random_matrix(generator: random.Random) -> Matrix:
    return tuple(generator.randrange(MOD) for _ in range(4))  # type: ignore[return-value]


def random_queries(
    generator: random.Random, size: int, count: int
) -> list[Query]:
    result: list[Query] = []
    for _ in range(count):
        kinds = ["P", "R", "A"]
        if size:
            kinds += ["S", "G"]
        kind = generator.choice(kinds)
        if kind == "S":
            result.append((kind, (generator.randrange(size), *random_matrix(generator))))
        elif kind == "G":
            result.append((kind, (generator.randrange(size),)))
        elif kind == "P":
            result.append((kind, (generator.randrange(size + 1),)))
        elif kind == "R":
            left = generator.randrange(size + 1)
            result.append((kind, (left, generator.randrange(left, size + 1))))
        else:
            result.append((kind, ()))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    generator = random.Random(2026071615)
    cases: list[tuple[list[Matrix], list[Query]]] = [
        ([], [("P", (0,)), ("R", (0, 0)), ("A", ())]),
        ([IDENTITY], [("G", (0,)), ("P", (1,)), ("A", ())]),
        (
            [(1, 2, 3, 4), (0, 1, 1, 0)],
            [("R", (0, 2)), ("R", (1, 2)), ("S", (0, 2, 0, 0, 3)), ("A", ())],
        ),
    ]
    for _ in range(30):
        size = generator.randrange(0, 100)
        initial = [random_matrix(generator) for _ in range(size)]
        cases.append((initial, random_queries(generator, size, 300)))
    size = 4096
    initial = [random_matrix(generator) for _ in range(size)]
    cases.append((initial, random_queries(generator, size, 800)))

    for index, case in enumerate(cases):
        write_case(args.out_dir, index, *case)


if __name__ == "__main__":
    main()
