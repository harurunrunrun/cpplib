#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def prod(mp: dict[int, int], left: int, right: int) -> int:
    return sum(value for key, value in mp.items() if left <= key < right)


def kth(mp: dict[int, int], k: int) -> str:
    keys = sorted(mp)
    if 0 <= k < len(keys):
        return str(keys[k])
    return "NONE"


def order_of_key(mp: dict[int, int], key: int) -> int:
    return sum(1 for current in mp if current < key)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20261113)
    versions: list[dict[int, int]] = [{}]
    commands: list[str] = []
    outputs: list[str] = []

    def add_update(command: str, next_map: dict[int, int]) -> int:
        commands.append(command)
        versions.append(next_map)
        next_version = len(versions) - 1
        outputs.append(str(next_version))
        return next_version

    def add_query(command: str, output: str | int) -> None:
        commands.append(command)
        outputs.append(str(output))

    v1 = add_update("INSERT 0 5 10", {5: 10})
    v2 = add_update(f"INSERT {v1} 2 3", {2: 3, 5: 10})
    v3 = add_update(f"APPLY {v2} 0 6 2", {2: 5, 5: 12})
    add_query(f"PROD {v2} 0 10", 13)
    add_query(f"PROD {v3} 0 10", 17)
    add_query(f"GET {v2} 5", 10)
    add_query(f"GET {v3} 5", 12)
    add_query("GET 0 5", "NONE")
    v4 = add_update(f"SET {v3} 5 100", {2: 5, 5: 100})
    v5 = add_update(f"ERASE {v4} 2", {5: 100})
    add_query(f"ALL_PROD {v5}", 100)

    for _ in range(280):
        base = rng.randrange(len(versions))
        key = rng.randrange(0, 61)
        kind = rng.randrange(12)
        current = versions[base]
        if kind < 2:
            value = rng.randrange(-50, 51)
            next_map = dict(current)
            next_map.setdefault(key, value)
            add_update(f"INSERT {base} {key} {value}", next_map)
        elif kind < 4:
            next_map = dict(current)
            next_map.pop(key, None)
            add_update(f"ERASE {base} {key}", next_map)
        elif kind < 6:
            value = rng.randrange(-50, 51)
            next_map = dict(current)
            if key in next_map:
                next_map[key] = value
            add_update(f"SET {base} {key} {value}", next_map)
        elif kind < 8:
            left = rng.randrange(-5, 66)
            right = rng.randrange(-5, 66)
            if left > right:
                left, right = right, left
            add = rng.randrange(-20, 21)
            next_map = dict(current)
            for current_key in list(next_map):
                if left <= current_key < right:
                    next_map[current_key] += add
            add_update(f"APPLY {base} {left} {right} {add}", next_map)
        elif kind == 8:
            add = rng.randrange(-20, 21)
            next_map = {current_key: value + add for current_key, value in current.items()}
            add_update(f"ALL_APPLY {base} {add}", next_map)
        elif kind == 9:
            left = rng.randrange(-5, 66)
            right = rng.randrange(-5, 66)
            if left > right:
                left, right = right, left
            add_query(f"PROD {base} {left} {right}", prod(current, left, right))
        elif kind == 10:
            query_key = rng.randrange(0, 61)
            value = current.get(query_key)
            add_query(f"GET {base} {query_key}", value if value is not None else "NONE")
        else:
            subkind = rng.randrange(5)
            if subkind == 0:
                add_query(f"SIZE {base}", len(current))
            elif subkind == 1:
                add_query(f"ALL_PROD {base}", prod(current, -1000, 1000))
            elif subkind == 2:
                query_key = rng.randrange(0, 61)
                add_query(f"CONTAINS {base} {query_key}", int(query_key in current))
            elif subkind == 3:
                k = rng.randrange(-2, len(current) + 3)
                add_query(f"KTH {base} {k}", kth(current, k))
            else:
                query_key = rng.randrange(0, 61)
                add_query(f"ORDER {base} {query_key}", order_of_key(current, query_key))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
