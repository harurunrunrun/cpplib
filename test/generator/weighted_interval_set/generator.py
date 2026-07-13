#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

LIMIT = 48


def intervals(values: list[str | None]) -> list[tuple[int, int, str]]:
    result: list[tuple[int, int, str]] = []
    left = 0
    while left < len(values):
        value = values[left]
        if value is None:
            left += 1
            continue
        right = left + 1
        while right < len(values) and values[right] == value:
            right += 1
        result.append((left, right, value))
        left = right
    return result


def format_entries(entries: list[tuple[int, int, str]]) -> str:
    fields = [str(len(entries))]
    for left, right, value in entries:
        fields.extend((str(left), str(right), value))
    return " ".join(fields)


def solve(commands: list[str]) -> str:
    values: list[str | None] = [None] * LIMIT
    output: list[str] = []

    for command in commands:
        fields = command.split()
        kind = fields[0]

        if kind in {"SET", "ASSIGN"}:
            left, right = map(int, fields[1:3])
            value = fields[3]
            for x in range(left, right):
                values[x] = value
        elif kind == "ERASE":
            left, right = map(int, fields[1:3])
            for x in range(left, right):
                values[x] = None
        elif kind == "CLEAR":
            values = [None] * LIMIT
        elif kind == "GET":
            x = int(fields[1])
            output.append(values[x] if values[x] is not None else "-")
        elif kind == "INTERVAL":
            x = int(fields[1])
            found = next(
                (
                    (left, right, value)
                    for left, right, value in intervals(values)
                    if left <= x < right
                ),
                None,
            )
            output.append(
                "-" if found is None
                else f"{found[0]} {found[1]} {found[2]}"
            )
        elif kind == "ENUM":
            query_left, query_right = map(int, fields[1:3])
            found = [
                entry
                for entry in intervals(values)
                if query_left < query_right
                if entry[0] < query_right and query_left < entry[1]
            ]
            output.append(format_entries(found))
        elif kind == "ENUMALL":
            output.append(format_entries(intervals(values)))
        elif kind in {"PROD", "ALLPROD"}:
            if kind == "PROD":
                query_left, query_right = map(int, fields[1:3])
            else:
                query_left, query_right = 0, LIMIT
            product = "".join(
                value
                for left, right, value in intervals(values)
                if query_left < query_right
                if left < query_right and query_left < right
            )
            output.append(f"[{product}]")
        elif kind == "CONTAINS":
            output.append(str(int(values[int(fields[1])] is not None)))
        elif kind == "COVERED":
            left, right = map(int, fields[1:3])
            output.append(str(int(all(values[x] is not None for x in range(left, right)))))
        elif kind == "MEX":
            x = int(fields[1])
            while x < LIMIT and values[x] is not None:
                x += 1
            output.append(str(x))
        elif kind == "SIZE":
            output.append(str(len(intervals(values))))
        elif kind == "EMPTY":
            output.append(str(int(not intervals(values))))
        else:
            raise AssertionError(f"unknown command: {command}")

    return "\n".join(output) + ("\n" if output else "")


def write_case(out_dir: Path, index: int, commands: list[str]) -> None:
    stem = f"case_{index:02d}"
    input_text = f"{len(commands)}\n" + "\n".join(commands) + "\n"
    (out_dir / f"{stem}.in").write_text(input_text, encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(solve(commands), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        "EMPTY",
        "SET 0 10 A",
        "SET 3 7 B",
        "SIZE",
        "PROD 1 9",
        "INTERVAL 5",
        "ENUM 2 8",
        "ASSIGN 3 7 A",
        "SIZE",
        "INTERVAL 5",
        "SET 2 8 B",
        "ERASE 4 6",
        "PROD 0 10",
        "COVERED 0 10",
        "ASSIGN 4 6 B",
        "PROD 0 10",
        "COVERED 0 10",
        "MEX 0",
        "GET 10",
        "ENUMALL",
        "CLEAR",
        "EMPTY",
        "PROD 0 48",
        "SET 0 2 A",
        "SET 2 4 B",
        "SET 4 6 C",
        "PROD 1 5",
        "ALLPROD",
        "CONTAINS 3",
        "ENUM 2 2",
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20261017)
    query_kinds = [
        "GET",
        "INTERVAL",
        "ENUM",
        "ENUMALL",
        "PROD",
        "ALLPROD",
        "CONTAINS",
        "COVERED",
        "MEX",
        "SIZE",
        "EMPTY",
    ]

    for case_index in range(1, 4):
        commands: list[str] = []
        for _ in range(700):
            choice = rng.randrange(100)
            if choice < 38:
                left = rng.randrange(LIMIT + 1)
                right = rng.randrange(LIMIT + 1)
                if left > right:
                    left, right = right, left
                value = chr(ord("A") + rng.randrange(5))
                kind = rng.choice(["SET", "ASSIGN"])
                commands.append(f"{kind} {left} {right} {value}")
            elif choice < 55:
                left = rng.randrange(LIMIT + 1)
                right = rng.randrange(LIMIT + 1)
                if left > right:
                    left, right = right, left
                commands.append(f"ERASE {left} {right}")
            elif choice < 57:
                commands.append("CLEAR")
            else:
                kind = rng.choice(query_kinds)
                if kind in {"GET", "INTERVAL", "CONTAINS", "MEX"}:
                    commands.append(f"{kind} {rng.randrange(LIMIT)}")
                elif kind in {"ENUM", "PROD", "COVERED"}:
                    left = rng.randrange(LIMIT + 1)
                    right = rng.randrange(LIMIT + 1)
                    if left > right:
                        left, right = right, left
                    commands.append(f"{kind} {left} {right}")
                else:
                    commands.append(kind)
        write_case(out_dir, case_index, commands)


if __name__ == "__main__":
    main()
