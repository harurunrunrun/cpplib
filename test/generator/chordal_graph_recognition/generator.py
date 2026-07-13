#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import random
from pathlib import Path


def is_chordal(n: int, edges: list[tuple[int, int]]) -> bool:
    adjacent = [0] * n
    for left, right in set((min(a, b), max(a, b)) for a, b in edges):
        adjacent[left] |= 1 << right
        adjacent[right] |= 1 << left

    @functools.lru_cache(maxsize=None)
    def solve(mask: int) -> bool:
        if mask == 0:
            return True
        for vertex in range(n):
            if not (mask >> vertex) & 1:
                continue
            neighbors = adjacent[vertex] & mask
            simplicial = True
            remaining = neighbors
            while remaining:
                bit = remaining & -remaining
                to = bit.bit_length() - 1
                if (neighbors ^ bit) & ~adjacent[to]:
                    simplicial = False
                    break
                remaining ^= bit
            if simplicial and solve(mask ^ (1 << vertex)):
                return True
        return False

    return solve((1 << n) - 1)


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    known: bool | None = None,
) -> None:
    input_lines = [f"{n} {len(edges)}"]
    input_lines.extend(f"{left} {right}" for left, right in edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    answer = known if known is not None else is_chordal(n, edges)
    (out_dir / f"case_{index:02d}.out").write_text(
        "YES\n" if answer else "NO\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 0, [], True)
    write_case(out_dir, 1, 4, [(0, 1), (1, 2), (2, 3), (3, 0)], False)
    write_case(
        out_dir,
        2,
        4,
        [(0, 1), (1, 2), (2, 3), (3, 0), (0, 2)],
        True,
    )
    rng = random.Random(2026071612)
    for index in range(3, 11):
        n = 11
        edges = [
            (left, right)
            for left in range(n)
            for right in range(left + 1, n)
            if rng.randrange(100) < 38
        ]
        write_case(out_dir, index, n, edges)

    n = 200000
    write_case(out_dir, 11, n, [(v, v + 1) for v in range(n - 1)], True)
    write_case(
        out_dir,
        12,
        n,
        [(v, v + 1) for v in range(n - 1)] + [(n - 1, 0)],
        False,
    )


if __name__ == "__main__":
    main()
