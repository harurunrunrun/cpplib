#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def solve(queries: list[str]) -> str:
    versions: list[list[int]] = [[]]
    out: list[str] = []
    for query in queries:
        typ, version_text, x_text = query.split()
        version = int(version_text)
        x = int(x_text)
        values = versions[version]

        if typ == "INSERT":
            nxt = values.copy()
            position = bisect.bisect_left(nxt, x)
            if position == len(nxt) or nxt[position] != x:
                nxt.insert(position, x)
            versions.append(nxt)
            out.append(str(len(versions) - 1))
        elif typ == "ERASE":
            nxt = values.copy()
            position = bisect.bisect_left(nxt, x)
            if position != len(nxt) and nxt[position] == x:
                nxt.pop(position)
            versions.append(nxt)
            out.append(str(len(versions) - 1))
        elif typ == "CONTAINS":
            position = bisect.bisect_left(values, x)
            out.append(str(int(position != len(values) and values[position] == x)))
        elif typ == "KTH":
            out.append(str(values[x] if 0 <= x < len(values) else -1))
        elif typ == "ORDER":
            out.append(str(bisect.bisect_left(values, x)))
        elif typ == "ORDER_UPPER":
            out.append(str(bisect.bisect_right(values, x)))
        elif typ == "LOWER":
            position = bisect.bisect_left(values, x)
            out.append(str(values[position] if position != len(values) else -1))
        elif typ == "UPPER":
            position = bisect.bisect_right(values, x)
            out.append(str(values[position] if position != len(values) else -1))
        elif typ == "MAX_LEQ":
            position = bisect.bisect_right(values, x) - 1
            out.append(str(values[position] if position >= 0 else -1))
        elif typ == "MAX_LESS":
            position = bisect.bisect_left(values, x) - 1
            out.append(str(values[position] if position >= 0 else -1))
        elif typ == "SIZE":
            out.append(str(len(values)))
        elif typ == "DUMP":
            out.append(" ".join(map(str, values)))
        else:
            raise AssertionError(typ)
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    input_text = "\n".join([str(len(queries)), *queries]) + "\n"
    (out_dir / f"{name}.in").write_text(input_text, encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(queries), encoding="utf-8")


def deterministic_queries() -> list[str]:
    return [
        "INSERT 0 40",
        "INSERT 1 10",
        "INSERT 1 70",
        "INSERT 2 30",
        "ERASE 3 40",
        "INSERT 3 55",
        "ERASE 4 10",
        "INSERT 2 10",
        "ERASE 6 999",
        "DUMP 0 0",
        "DUMP 1 0",
        "DUMP 2 0",
        "DUMP 3 0",
        "DUMP 4 0",
        "DUMP 5 0",
        "DUMP 6 0",
        "DUMP 7 0",
        "DUMP 8 0",
        "DUMP 9 0",
        "LOWER 6 56",
        "UPPER 6 55",
        "MAX_LEQ 6 69",
        "MAX_LESS 6 55",
        "ORDER 6 56",
        "ORDER_UPPER 6 55",
        "KTH 6 -1",
        "KTH 6 3",
        "SIZE 6 0",
        "CONTAINS 5 40",
        "CONTAINS 3 40",
    ]


def random_queries(seed: int, count: int) -> list[str]:
    rng = random.Random(seed)
    queries: list[str] = []
    version_count = 1
    query_types = [
        "CONTAINS",
        "KTH",
        "ORDER",
        "ORDER_UPPER",
        "LOWER",
        "UPPER",
        "MAX_LEQ",
        "MAX_LESS",
        "SIZE",
        "DUMP",
    ]
    for _ in range(count):
        action = rng.randrange(100)
        version = rng.randrange(version_count)
        if action < 28:
            queries.append(f"INSERT {version} {rng.randrange(-50, 91)}")
            version_count += 1
        elif action < 52:
            queries.append(f"ERASE {version} {rng.randrange(-50, 91)}")
            version_count += 1
        else:
            typ = rng.choice(query_types)
            if typ == "KTH":
                x = rng.randrange(-4, 40)
            elif typ in {"SIZE", "DUMP"}:
                x = 0
            else:
                x = rng.randrange(-60, 101)
            queries.append(f"{typ} {version} {x}")
    return queries


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, deterministic_queries())
    write_case(out_dir, 1, random_queries(20260921, 350))
    write_case(out_dir, 2, random_queries(20260922, 700))


if __name__ == "__main__":
    main()
