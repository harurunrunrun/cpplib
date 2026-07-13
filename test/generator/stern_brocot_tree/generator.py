#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from collections.abc import Iterable
from pathlib import Path

Fraction = tuple[int, int]
Run = tuple[str, int]


def compare(lhs: Fraction, rhs: Fraction) -> int:
    left = lhs[0] * rhs[1]
    right = rhs[0] * lhs[1]
    return (left > right) - (left < right)


def encode(value: Fraction) -> list[Run]:
    numerator, denominator = value
    result: list[Run] = []
    while numerator != denominator:
        if numerator < denominator:
            count = (denominator - 1) // numerator
            result.append(("L", count))
            denominator -= count * numerator
        else:
            count = (numerator - 1) // denominator
            result.append(("R", count))
            numerator -= count * denominator
    return result


def decode(path: Iterable[Run], initial: Fraction = (1, 1)) -> Fraction:
    numerator, denominator = initial
    for direction, count in reversed(list(path)):
        if direction == "L":
            denominator += numerator * count
        else:
            numerator += denominator * count
    return numerator, denominator


def truncate(path: list[Run], depth: int) -> list[Run] | None:
    if depth < 0:
        return None
    result: list[Run] = []
    for direction, count in path:
        if depth == 0:
            break
        take = min(depth, count)
        result.append((direction, take))
        depth -= take
    return result if depth == 0 else None


def lca(lhs: Fraction, rhs: Fraction) -> Fraction:
    left_path = encode(lhs)
    right_path = encode(rhs)
    common: list[Run] = []
    for left, right in zip(left_path, right_path):
        if left[0] != right[0]:
            break
        common.append((left[0], min(left[1], right[1])))
        if left[1] != right[1]:
            break
    return decode(common)


def enumerate_nodes(max_depth: int) -> list[Fraction]:
    result: list[Fraction] = []
    level: list[tuple[Fraction, Fraction, Fraction]] = [((0, 1), (1, 1), (1, 0))]
    for _ in range(max_depth + 1):
        result.extend(value for _, value, _ in level)
        next_level: list[tuple[Fraction, Fraction, Fraction]] = []
        for lower, value, upper in level:
            left = (lower[0] + value[0], lower[1] + value[1])
            right = (value[0] + upper[0], value[1] + upper[1])
            next_level.append((lower, left, value))
            next_level.append((value, right, upper))
        level = next_level
    return result


NAIVE_NODES = enumerate_nodes(14)


def simplest_open(lower: Fraction, upper: Fraction) -> Fraction:
    for candidate in NAIVE_NODES:
        if compare(lower, candidate) < 0 and compare(candidate, upper) < 0:
            return candidate
    raise AssertionError("naive search depth is insufficient")


def run_query(query: str) -> str:
    tokens = query.split()
    kind = tokens[0]
    if kind == "ENCODE_PATH":
        path = encode((int(tokens[1]), int(tokens[2])))
        fields: list[str] = [str(len(path))]
        for direction, count in path:
            fields.extend((direction, str(count)))
        return " ".join(fields)
    if kind == "DECODE_PATH":
        count = int(tokens[1])
        path = [(tokens[2 + 2 * i], int(tokens[3 + 2 * i])) for i in range(count)]
        return "{} {}".format(*decode(path))
    if kind == "LCA":
        return "{} {}".format(*lca((int(tokens[1]), int(tokens[2])), (int(tokens[3]), int(tokens[4]))))
    if kind == "ANCESTOR":
        depth = int(tokens[1])
        path = truncate(encode((int(tokens[2]), int(tokens[3]))), depth)
        return "-1" if path is None else "{} {}".format(*decode(path))
    if kind == "RANGE":
        path = encode((int(tokens[1]), int(tokens[2])))
        lower = decode(path, (0, 1))
        upper = decode(path, (1, 0))
        return f"{lower[0]} {lower[1]} {upper[0]} {upper[1]}"
    if kind == "DEPTH":
        return str(sum(count for _, count in encode((int(tokens[1]), int(tokens[2])))))
    if kind == "DESCEND":
        path = encode((int(tokens[1]), int(tokens[2])))
        direction, count = tokens[3], int(tokens[4])
        if count:
            if path and path[-1][0] == direction:
                path[-1] = (direction, path[-1][1] + count)
            else:
                path.append((direction, count))
        return "{} {}".format(*decode(path))
    if kind == "OPEN":
        lower = (int(tokens[1]), int(tokens[2]))
        upper = (int(tokens[3]), int(tokens[4]))
        return "{} {}".format(*simplest_open(lower, upper))
    if kind == "SEARCH":
        return f"{tokens[1]} {tokens[2]}"
    raise AssertionError(f"unknown query: {kind}")


def write_case(out_dir: Path, case_id: int, queries: list[str]) -> None:
    answers = [run_query(query) for query in queries]
    (out_dir / f"case_{case_id:02d}.in").write_text(
        str(len(queries)) + "\n" + "\n".join(queries) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{case_id:02d}.out").write_text(
        "\n".join(answers) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [
        "ENCODE_PATH 1 1",
        "ENCODE_PATH 1000000000 1",
        "DECODE_PATH 1 R 999999999",
        "LCA 5 3 8 5",
        "ANCESTOR 0 13 8",
        "ANCESTOR 100 13 8",
        "RANGE 1 1",
        "RANGE 13 8",
        "OPEN 1 1 3 2",
        "OPEN 2 3 1 1",
        "SEARCH 999999937 1000000000 1000000000",
    ])

    rng = random.Random(20260713)
    nodes = enumerate_nodes(9)
    queries: list[str] = []
    for _ in range(240):
        kind = rng.randrange(8)
        value = rng.choice(nodes)
        if kind == 0:
            queries.append(f"ENCODE_PATH {value[0]} {value[1]}")
        elif kind == 1:
            path = encode(value)
            fields = ["DECODE_PATH", str(len(path))]
            for direction, count in path:
                fields.extend((direction, str(count)))
            queries.append(" ".join(fields))
        elif kind == 2:
            other = rng.choice(nodes)
            queries.append(f"LCA {value[0]} {value[1]} {other[0]} {other[1]}")
        elif kind == 3:
            depth = rng.randrange(0, 14)
            queries.append(f"ANCESTOR {depth} {value[0]} {value[1]}")
        elif kind == 4:
            queries.append(f"RANGE {value[0]} {value[1]}")
        elif kind == 5:
            queries.append(f"DEPTH {value[0]} {value[1]}")
        elif kind == 6:
            direction = rng.choice(("L", "R"))
            steps = rng.randrange(0, 20)
            queries.append(f"DESCEND {value[0]} {value[1]} {direction} {steps}")
        else:
            other = rng.choice(nodes)
            if compare(value, other) <= 0:
                lower, upper = value, other
            else:
                lower, upper = other, value
            if lower == upper:
                upper = (upper[0] + upper[1], upper[1])
                divisor = math.gcd(*upper)
                upper = (upper[0] // divisor, upper[1] // divisor)
            queries.append(f"OPEN {lower[0]} {lower[1]} {upper[0]} {upper[1]}")
    write_case(out_dir, 1, queries)

    searches = []
    for _ in range(160):
        numerator = rng.randrange(1, 500)
        denominator = rng.randrange(1, 500)
        divisor = math.gcd(numerator, denominator)
        numerator //= divisor
        denominator //= divisor
        maximum = max(numerator, denominator) + rng.randrange(0, 500)
        searches.append(f"SEARCH {numerator} {denominator} {maximum}")
    write_case(out_dir, 2, searches)


if __name__ == "__main__":
    main()
