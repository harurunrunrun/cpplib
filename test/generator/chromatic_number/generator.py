#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def exact_chromatic_number(n: int, edges: list[tuple[int, int]]) -> int:
    if n == 0:
        return 0
    adjacent = [0] * n
    for u, v in edges:
        adjacent[u] |= 1 << v
        adjacent[v] |= 1 << u

    colors = [-1] * n
    answer = n

    def search(colored: int, used: int) -> None:
        nonlocal answer
        if used >= answer:
            return
        if colored == n:
            answer = used
            return
        candidates = [vertex for vertex in range(n) if colors[vertex] < 0]
        vertex = max(
            candidates,
            key=lambda value: (
                len({colors[to] for to in range(n)
                     if adjacent[value] >> to & 1 and colors[to] >= 0}),
                adjacent[value].bit_count(),
            ),
        )
        forbidden = 0
        for to in range(n):
            if adjacent[vertex] >> to & 1 and colors[to] >= 0:
                forbidden |= 1 << colors[to]
        for color in range(used):
            if forbidden >> color & 1:
                continue
            colors[vertex] = color
            search(colored + 1, used)
            colors[vertex] = -1
        colors[vertex] = used
        search(colored + 1, used + 1)
        colors[vertex] = -1

    search(0, 0)
    return answer


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    answer: int | None = None,
) -> None:
    if answer is None:
        answer = exact_chromatic_number(n, edges)
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{n} {len(edges)}\n" + "".join(f"{u} {v}\n" for u, v in edges),
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        f"{answer}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, 0, [], 0)
    write_case(args.out_dir, 1, 20, [], 1)
    complete = [(u, v) for u in range(20) for v in range(u + 1, 20)]
    write_case(args.out_dir, 2, 20, complete, 20)
    bipartite = [(u, v) for u in range(10) for v in range(10, 20)]
    write_case(args.out_dir, 3, 20, bipartite, 2)
    odd_cycle = [(vertex, (vertex + 1) % 19) for vertex in range(19)]
    write_case(args.out_dir, 4, 19, odd_cycle, 3)

    rng = random.Random(2026071306)
    for index in range(5, 35):
        n = rng.randrange(1, 13)
        numerator = rng.randrange(1, 5)
        edges = [
            (u, v)
            for u in range(n)
            for v in range(u + 1, n)
            if rng.randrange(5) < numerator
        ]
        write_case(args.out_dir, index, n, edges)


if __name__ == "__main__":
    main()
