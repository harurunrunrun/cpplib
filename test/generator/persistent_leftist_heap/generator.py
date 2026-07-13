#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


class Case:
    def __init__(self) -> None:
        self.operations: list[str] = []
        self.answers: list[str] = []
        self.versions: list[list[int]] = [[]]

    def push(self, version: int, value: int) -> None:
        next_heap = self.versions[version].copy()
        bisect.insort(next_heap, value)
        self.versions.append(next_heap)
        self.operations.append(f"0 {version} {value}")
        self.answers.append(str(len(self.versions) - 1))

    def meld(self, left: int, right: int) -> None:
        next_heap = sorted(self.versions[left] + self.versions[right])
        self.versions.append(next_heap)
        self.operations.append(f"1 {left} {right}")
        self.answers.append(str(len(self.versions) - 1))

    def pop(self, version: int) -> None:
        top = self.versions[version][0]
        self.versions.append(self.versions[version][1:])
        self.operations.append(f"2 {version}")
        self.answers.append(f"{top} {len(self.versions) - 1}")

    def fork(self, version: int) -> None:
        self.versions.append(self.versions[version].copy())
        self.operations.append(f"3 {version}")
        self.answers.append(str(len(self.versions) - 1))


def write_case(out_dir: Path, index: int, case: Case) -> None:
    input_text = f"{len(case.operations)}\n" + "\n".join(case.operations) + "\n"
    output_text = "\n".join(case.answers) + "\n"
    (out_dir / f"case_{index:02d}.in").write_text(input_text, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(output_text, encoding="utf-8")


def random_case(rng: random.Random, query_count: int) -> Case:
    case = Case()
    for _ in range(query_count):
        first = rng.randrange(len(case.versions))
        operation = rng.randrange(4)
        if operation == 0:
            case.push(first, rng.randrange(-10**12, 10**12 + 1))
        elif operation == 1 and case.versions[first]:
            case.pop(first)
        elif operation == 2:
            second = rng.randrange(len(case.versions))
            if len(case.versions[first]) + len(case.versions[second]) <= 400:
                case.meld(first, second)
            else:
                case.fork(first)
        else:
            case.fork(first)
    return case


def large_case(size: int) -> tuple[list[str], list[str]]:
    operations: list[str] = []
    answers: list[str] = []
    version = 0
    for value in range(size - 1, -1, -1):
        operations.append(f"0 {version} {value}")
        version += 1
        answers.append(str(version))
    for value in range(size):
        operations.append(f"2 {version}")
        version += 1
        answers.append(f"{value} {version}")
    return operations, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    basic = Case()
    basic.push(0, 5)
    basic.push(1, 2)
    basic.push(1, 7)
    basic.meld(2, 3)
    basic.pop(4)
    basic.meld(2, 2)
    basic.pop(6)
    basic.fork(0)
    write_case(args.out_dir, 0, basic)

    rng = random.Random(2026071315)
    for index in range(1, 26):
        write_case(args.out_dir, index, random_case(rng, 200 + 20 * index))

    operations, answers = large_case(10000)
    (args.out_dir / "case_26.in").write_text(
        f"{len(operations)}\n" + "\n".join(operations) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "case_26.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
