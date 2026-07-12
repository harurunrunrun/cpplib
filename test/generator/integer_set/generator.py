#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def make_case(limit: int, seed: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    values: set[int] = set()
    commands: list[str] = []
    outputs: list[str] = []

    def ordered() -> list[int]:
        return sorted(values)

    def query(command: str, output: object) -> None:
        commands.append(command)
        outputs.append(str(output))

    def add_insert(x: int) -> None:
        commands.append(f"INSERT {x}")
        values.add(x)

    def add_erase(x: int) -> None:
        commands.append(f"ERASE {x}")
        values.discard(x)

    def successor(x: int, strict: bool) -> int | None:
        a = ordered()
        pos = bisect.bisect_right(a, x) if strict else bisect.bisect_left(a, x)
        return a[pos] if pos < len(a) else None

    def predecessor(x: int, strict: bool) -> int | None:
        a = ordered()
        pos = (bisect.bisect_left(a, x) if strict else bisect.bisect_right(a, x)) - 1
        return a[pos] if pos >= 0 else None

    def kth(x: int, k: int, greater: bool, strict: bool) -> int | None:
        if greater:
            a = [value for value in ordered() if value > x or (not strict and value == x)]
        else:
            a = [value for value in reversed(ordered()) if value < x or (not strict and value == x)]
        return a[k] if 0 <= k < len(a) else None

    query("EMPTY", 1)
    query("MIN", "NONE")
    query("MAX", "NONE")
    query("LIST", "0")
    query("TRY_INSERT -1", "THROW")
    query(f"TRY_INSERT {limit}", "THROW")
    query("TRY_ERASE -1", "THROW")
    query(f"UTRY_INSERT {limit}", "THROW")
    query(f"UTRY_ERASE {limit + 7}", "THROW")

    for x in (0, limit - 1, 1, limit // 2, limit // 2 + 1, 16, 17):
        add_insert(x)
    add_insert(0)
    add_erase(limit // 3)

    for x in (-4, -1, 0, 1, 15, 16, 17, limit - 1, limit, limit + 8):
        query(f"CONTAIN {x}", int(x in values))
        query(f"LEAST {x}", optional(successor(x, False)))
        query(f"MORE {x}", optional(successor(x, True)))
        query(f"MOST {x}", optional(predecessor(x, False)))
        query(f"LESS {x}", optional(predecessor(x, True)))

    for l, r in ((-9, -1), (-9, 1), (0, limit), (1, limit - 1), (limit - 1, limit + 9),
                 (limit, limit + 9), (20, 10)):
        query(f"RANGE {l} {r}", sum(l <= value < r for value in values))

    for x in (0, limit - 1, limit, limit + 9):
        query(f"UCONTAIN {x}", int(x in values))
        query(f"ULEAST {x}", optional(successor(x, False)))
        query(f"UMORE {x}", optional(successor(x, True)))
        query(f"UMOST {x}", optional(predecessor(x, False)))
        query(f"ULESS {x}", optional(predecessor(x, True)))
    query(f"URANGE 0 {limit + 9}", len(values))
    query(f"URANGE {limit} {limit + 9}", 0)

    for x in (-1, 0, 16, limit - 1, limit + 1):
        for k in (-1, 0, 1, len(values), len(values) + 2):
            query(f"KGE {x} {k}", optional(kth(x, k, True, False)))
            query(f"KGT {x} {k}", optional(kth(x, k, True, True)))
            query(f"KLE {x} {k}", optional(kth(x, k, False, False)))
            query(f"KLT {x} {k}", optional(kth(x, k, False, True)))
    for x in (0, limit - 1, limit, limit + 1):
        for k in (0, 1, len(values) + 1):
            query(f"UKGE {x} {k}", optional(kth(x, k, True, False)))
            query(f"UKGT {x} {k}", optional(kth(x, k, True, True)))
            query(f"UKLE {x} {k}", optional(kth(x, k, False, False)))
            query(f"UKLT {x} {k}", optional(kth(x, k, False, True)))

    query("COPY 2", 1)
    query("MOVE", 1)
    query("LIST", " ".join(map(str, [len(values), *ordered()])))

    if limit <= 300:
        for x in range(limit):
            add_insert(x)
        query(f"RANGE 0 {limit}", limit)
        query(f"KGE 0 {limit - 1}", limit - 1)
        query(f"KLE {limit - 1} {limit - 1}", 0)
        add_insert(limit - 1)
        for x in range(0, limit, 2):
            add_erase(x)
        query("LIST", " ".join(map(str, [len(values), *ordered()])))
        query("COPY 2", 1)
        query("MOVE", 1)
    for step in range(2600):
        kind = rng.randrange(18)
        x = rng.randrange(limit)
        if kind < 4:
            add_insert(x)
        elif kind < 6:
            add_erase(x)
        elif kind == 6:
            query(f"CONTAIN {x}", int(x in values))
        elif kind == 7:
            l = rng.randrange(-20, limit + 20)
            r = rng.randrange(-20, limit + 20)
            query(f"RANGE {l} {r}", sum(l <= value < r for value in values))
        elif kind in (8, 9, 10, 11):
            boundary = rng.randrange(-5, limit + 6)
            names = ("LEAST", "MORE", "MOST", "LESS")
            answers = (successor(boundary, False), successor(boundary, True),
                       predecessor(boundary, False), predecessor(boundary, True))
            query(f"{names[kind - 8]} {boundary}", optional(answers[kind - 8]))
        elif kind in (12, 13, 14, 15):
            boundary = rng.randrange(-5, limit + 6)
            k = rng.randrange(-3, len(values) + 4)
            names = ("KGE", "KGT", "KLE", "KLT")
            args = ((True, False), (True, True), (False, False), (False, True))
            greater, strict = args[kind - 12]
            query(f"{names[kind - 12]} {boundary} {k}", optional(kth(boundary, k, greater, strict)))
        elif kind == 16:
            query("MIN", optional(min(values) if values else None))
            query("MAX", optional(max(values) if values else None))
            query("EMPTY", int(not values))
        else:
            query("COPY 2", 1)
            query("MOVE", 1)

        if step in (701, 1903):
            commands.append("CLEAR")
            values.clear()
            query("LIST", "0")

    query("LIST", " ".join(map(str, [len(values), *ordered()])))
    return commands, outputs


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    for case_id, (type_id, limit, seed) in enumerate(((0, 257, 20260713), (1, 1000003, 20260714))):
        commands, outputs = make_case(limit, seed)
        (out_dir / f"case_{case_id:02d}.in").write_text(
            f"{type_id} {len(commands)}\n" + "\n".join(commands) + "\n",
            encoding="utf-8",
        )
        (out_dir / f"case_{case_id:02d}.out").write_text(
            "\n".join(outputs) + "\n",
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
