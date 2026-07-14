#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def leader(parent: list[int], vertex: int) -> int:
    while parent[vertex] >= 0:
        vertex = parent[vertex]
    return vertex


def merge(parent: list[int], left: int, right: int) -> None:
    left = leader(parent, left)
    right = leader(parent, right)
    if left == right:
        return
    if parent[left] > parent[right]:
        left, right = right, left
    parent[left] += parent[right]
    parent[right] = left


def write_case(
    out_dir: Path,
    case_index: int,
    size: int,
    queries: list[tuple[int, int, int, int]],
) -> None:
    states: list[list[int]] = [[-1] * size]
    answers: list[int] = []
    for kind, base_query, left, right in queries:
        base = states[base_query + 1]
        if kind == 0:
            current = base[:]
            merge(current, left, right)
            states.append(current)
        else:
            answers.append(int(leader(base, left) == leader(base, right)))
            states.append(base[:])

    stem = f"case_{case_index:03d}"
    lines = [f"{size} {len(queries)}"]
    lines.extend(" ".join(map(str, query)) for query in queries)
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{stem}.out").write_text(
        "\n".join(map(str, answers)) + ("\n" if answers else ""),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [])
    write_case(out_dir, 1, 1, [(1, -1, 0, 0)])
    write_case(
        out_dir,
        2,
        4,
        [
            (0, -1, 0, 1),
            (1, 0, 0, 1),
            (1, -1, 0, 1),
            (0, 0, 1, 2),
            (1, 3, 0, 2),
            (1, 2, 0, 2),
        ],
    )

    rng = random.Random(2026071501)
    for case_index in range(3, 43):
        size = rng.randrange(1, 65)
        query_count = rng.randrange(1, 350)
        queries: list[tuple[int, int, int, int]] = []
        for query in range(query_count):
            queries.append(
                (
                    rng.randrange(2),
                    rng.randrange(-1, query),
                    rng.randrange(size),
                    rng.randrange(size),
                )
            )
        write_case(out_dir, case_index, size, queries)


if __name__ == "__main__":
    main()
