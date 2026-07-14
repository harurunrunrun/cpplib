#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(left_size: int, right_size: int, pairs: list[tuple[int, int]]) -> int:
    graph = [[] for _ in range(left_size)]
    for left, right in pairs:
        graph[left].append(right)
    match = [-1] * right_size

    def augment(vertex: int, seen: list[bool]) -> bool:
        for to in graph[vertex]:
            if seen[to]:
                continue
            seen[to] = True
            if match[to] == -1 or augment(match[to], seen):
                match[to] = vertex
                return True
        return False

    return sum(augment(left, [False] * right_size) for left in range(left_size))


def write_case(out_dir: Path, name: str, left_size: int, right_size: int, pairs: list[tuple[int, int]], answer: int | None = None) -> None:
    assert 1 <= left_size <= 50000 and 1 <= right_size <= 50000
    assert 1 <= len(pairs) <= 150000
    assert all(0 <= left < left_size and 0 <= right < right_size for left, right in pairs)
    if answer is None:
        answer = solve(left_size, right_size, pairs)
    (out_dir / f"{name}.in").write_text(
        "\n".join([f"{left_size} {right_size} {len(pairs)}", *(f"{a + 1} {b + 1}" for a, b in pairs)]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "official", 5, 4, [(4, 1), (0, 1), (3, 2), (2, 0), (1, 1), (3, 3)], 3)
    write_case(out_dir, "one", 1, 1, [(0, 0)], 1)
    source = random.Random(127)
    for case_index in range(40):
        left_size = source.randint(1, 8)
        right_size = source.randint(1, 8)
        all_pairs = [(a, b) for a in range(left_size) for b in range(right_size)]
        pairs = source.sample(all_pairs, source.randint(1, len(all_pairs)))
        write_case(out_dir, f"random_{case_index:02d}", left_size, right_size, pairs)
    maximum = [(index, index) for index in range(50000)]
    maximum.extend((index % 50000, index % 50000) for index in range(100000))
    write_case(out_dir, "maximum", 50000, 50000, maximum, 50000)


if __name__ == "__main__":
    main()
