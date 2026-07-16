#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


class Oracle:
    def __init__(self) -> None:
        self.values: list[int] = []
        self.commands: list[str] = []
        self.output: list[str] = []

    def insert(self, value: int) -> None:
        self.commands.append(f"INSERT {value}")
        bisect.insort(self.values, value)

    def erase(self, value: int) -> None:
        self.commands.append(f"ERASE {value}")
        index = bisect.bisect_left(self.values, value)
        found = index != len(self.values) and self.values[index] == value
        self.output.append(str(int(found)))
        if found:
            self.values.pop(index)

    def erase_all(self, value: int) -> None:
        self.commands.append(f"ERASE_ALL {value}")
        left = bisect.bisect_left(self.values, value)
        right = bisect.bisect_right(self.values, value)
        self.output.append(str(right - left))
        del self.values[left:right]

    def contains(self, value: int) -> None:
        self.commands.append(f"CONTAINS {value}")
        index = bisect.bisect_left(self.values, value)
        self.output.append(
            str(int(index != len(self.values) and self.values[index] == value))
        )

    def count(self, value: int) -> None:
        self.commands.append(f"COUNT {value}")
        self.output.append(
            str(bisect.bisect_right(self.values, value)
                - bisect.bisect_left(self.values, value))
        )

    def median(self) -> None:
        if len(self.values) % 2 == 1:
            self.commands.append("ODD")
            self.output.append(str(self.values[len(self.values) // 2]))
        else:
            self.commands.append("EVEN")
            middle = len(self.values) // 2
            self.output.append(
                f"{self.values[middle - 1]} {self.values[middle]}"
            )

    def lower(self) -> None:
        self.commands.append("LOWER")
        self.output.append(str(self.values[(len(self.values) - 1) // 2]))

    def upper(self) -> None:
        self.commands.append("UPPER")
        self.output.append(str(self.values[len(self.values) // 2]))

    def size(self) -> None:
        self.commands.append("SIZE")
        self.output.append(str(len(self.values)))

    def empty(self) -> None:
        self.commands.append("EMPTY")
        self.output.append(str(int(not self.values)))

    def clear(self) -> None:
        self.commands.append("CLEAR")
        self.values.clear()


def write_case(out_dir: Path, name: str, oracle: Oracle) -> None:
    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(oracle.commands)), *oracle.commands]) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(oracle.output) + ("\n" if oracle.output else ""),
        encoding="utf-8",
    )


def boundary_case() -> Oracle:
    oracle = Oracle()
    oracle.empty()
    oracle.size()
    for value in (0, 0, -10**18, 10**18, 0):
        oracle.insert(value)
        oracle.lower()
        oracle.upper()
        oracle.median()
    oracle.count(0)
    oracle.contains(7)
    oracle.erase(0)
    oracle.erase_all(0)
    oracle.erase_all(0)
    oracle.size()
    oracle.clear()
    oracle.empty()
    oracle.insert(42)
    oracle.erase(42)
    oracle.empty()
    return oracle


def random_case() -> Oracle:
    rng = random.Random(2026071603)
    oracle = Oracle()
    for _ in range(12000):
        if not oracle.values:
            oracle.insert(rng.randint(-100, 100))
            continue
        operation = rng.randrange(12)
        value = rng.randint(-120, 120)
        if operation < 4 and len(oracle.values) < 400:
            oracle.insert(value)
        elif operation < 6:
            oracle.erase(value)
        elif operation == 6:
            oracle.erase_all(value)
        elif operation == 7:
            oracle.contains(value)
        elif operation == 8:
            oracle.count(value)
        elif operation == 9:
            oracle.lower()
        elif operation == 10:
            oracle.upper()
        else:
            oracle.median()
        if rng.randrange(1000) == 0:
            oracle.clear()
            oracle.empty()
    return oracle


def large_case() -> Oracle:
    oracle = Oracle()
    for value in range(100000):
        oracle.commands.append(f"INSERT {value}")
    oracle.values = list(range(100000))
    oracle.size()
    oracle.lower()
    oracle.upper()
    oracle.median()
    oracle.erase_all(50000)
    oracle.median()
    oracle.erase(0)
    oracle.lower()
    oracle.upper()
    oracle.median()
    oracle.clear()
    oracle.empty()
    return oracle


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "case_00_boundary", boundary_case())
    write_case(out_dir, "case_01_random", random_case())
    write_case(out_dir, "case_02_large", large_case())


if __name__ == "__main__":
    main()
