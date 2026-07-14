#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def arc_cells(size: int, start: int, end: int) -> list[int]:
    if abs(end - start) >= size:
        return list(range(size))
    length = (end - start) % size
    return [(start + offset) % size for offset in range(length)]


def evaluate(size: int, operations: list[tuple[object, ...]]) -> list[str]:
    covered = [False] * size
    result: list[str] = []
    for operation in operations:
        kind = str(operation[0])
        if kind in ("A", "R"):
            cells = arc_cells(size, int(operation[1]), int(operation[2]))
            value = kind == "A"
            for cell in cells:
                covered[cell] = value
        elif kind == "P":
            result.append(str(int(covered[int(operation[1]) % size])))
        elif kind == "C":
            cells = arc_cells(size, int(operation[1]), int(operation[2]))
            result.append(str(int(all(covered[cell] for cell in cells))))
        elif kind == "L":
            result.append(str(sum(covered)))
        elif kind == "N":
            runs = sum(
                covered[index] and (index == 0 or not covered[index - 1])
                for index in range(size)
            )
            result.append(str(runs))
        elif kind == "F":
            result.append(str(int(all(covered))))
        elif kind == "E":
            result.append(str(int(not any(covered))))
        else:
            assert kind == "X"
            covered = [False] * size
    return result


def write_case(
    output_directory: Path,
    index: int,
    size: int,
    operations: list[tuple[object, ...]],
) -> None:
    answers = evaluate(size, operations)
    with (output_directory / f"case_{index:02d}.in").open(
        "w", encoding="utf-8", newline="\n"
    ) as input_file, (output_directory / f"case_{index:02d}.out").open(
        "w", encoding="utf-8", newline="\n"
    ) as output_file:
        input_file.write(f"{size} {len(operations)}\n")
        input_file.writelines(" ".join(map(str, operation)) + "\n"
                              for operation in operations)
        output_file.writelines(answer + "\n" for answer in answers)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    basic: list[tuple[object, ...]] = [
        ("E",), ("F",), ("L",), ("N",),
        ("A", 350, 10), ("N",), ("L",),
        ("P", 350), ("P", 0), ("P", 10),
        ("C", 355, 5), ("C", 5, 15), ("C", 7, 7),
        ("A", 10, 20), ("N",), ("L",), ("P", 10), ("P", 20),
        ("R", 355, 5), ("N",), ("L",),
        ("A", 0, 360), ("F",), ("N",), ("L",),
        ("R", 100, 460), ("E",),
        ("A", -20, 20), ("P", 350), ("P", 20), ("X",), ("E",),
    ]
    write_case(arguments.out_dir, 0, 360, basic)

    source = random.Random(2026071504)
    operations: list[tuple[object, ...]] = []
    kinds = ["A", "R", "P", "C", "L", "N", "F", "E", "X"]
    for _ in range(12000):
        kind = source.choices(kinds, [20, 16, 20, 12, 7, 7, 5, 5, 1])[0]
        if kind in ("A", "R", "C"):
            operations.append((
                kind,
                source.randint(-720, 1080),
                source.randint(-720, 1080),
            ))
        elif kind == "P":
            operations.append((kind, source.randint(-720, 1080)))
        else:
            operations.append((kind,))
    write_case(arguments.out_dir, 1, 720, operations)

    maximum: list[tuple[object, ...]] = [("A", 0, 360)]
    maximum.extend(("P", index) for index in range(199999))
    write_case(arguments.out_dir, 2, 360, maximum)


if __name__ == "__main__":
    main()
