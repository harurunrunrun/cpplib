#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(size: int, edges: list[tuple[int, int]]) -> tuple[int, list[int]]:
    reach = [[False] * size for _ in range(size)]
    for vertex in range(size):
        reach[vertex][vertex] = True
    for left, right in edges:
        reach[left][right] = True
    for middle in range(size):
        for left in range(size):
            if not reach[left][middle]:
                continue
            for right in range(size):
                reach[left][right] |= reach[middle][right]
    representative = [
        min(other for other in range(size)
            if reach[vertex][other] and reach[other][vertex])
        for vertex in range(size)
    ]
    return len(set(representative)), representative


def write_case(
    out_dir: Path,
    name: str,
    size: int,
    edges: list[tuple[int, int]],
) -> None:
    count, representative = solve(size, edges)
    input_lines = [
        f"{size} {len(edges)}",
        *(f"{left} {right}" for left, right in edges),
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        f"{count}\n" + " ".join(map(str, representative)) + "\n",
        encoding="ascii",
    )

def write_deep_chain(out_dir: Path) -> None:
    size = 200_000
    (out_dir / "deep_chain.in").write_text(
        f"{size} {size - 1}\n"
        + "".join(f"{vertex} {vertex + 1}\n" for vertex in range(size - 1)),
        encoding="ascii",
    )
    (out_dir / "deep_chain.out").write_text(
        f"{size}\n" + " ".join(map(str, range(size))) + "\n",
        encoding="ascii",
    )



def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "empty", 0, [])
    write_case(out_dir, "single", 1, [])
    write_case(out_dir, "fixed", 7, [
        (0, 1), (1, 2), (2, 0), (2, 3), (3, 4), (4, 3), (5, 6),
    ])
    write_deep_chain(out_dir)
    source = random.Random(730125)
    for case_index in range(30):
        size = source.randint(1, 10)
        edges = [
            (left, right)
            for left in range(size)
            for right in range(size)
            if source.randrange(5) == 0
        ]
        write_case(out_dir, f"random_{case_index:02d}", size, edges)


if __name__ == "__main__":
    main()
