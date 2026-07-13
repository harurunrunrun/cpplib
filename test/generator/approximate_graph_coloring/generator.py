#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def adjacency_list(n: int, edges: list[tuple[int, int]]) -> list[list[int]]:
    adjacency = [set() for _ in range(n)]
    for left, right in edges:
        adjacency[left].add(right)
        adjacency[right].add(left)
    return [sorted(values) for values in adjacency]


def greedy(adjacency: list[list[int]], order: list[int]) -> tuple[list[int], int]:
    n = len(adjacency)
    colors = [n] * n
    for vertex in order:
        forbidden = {colors[x] for x in adjacency[vertex] if colors[x] != n}
        color = 0
        while color in forbidden:
            color += 1
        colors[vertex] = color
    return colors, max(colors, default=-1) + 1


def largest_first(adjacency: list[list[int]]) -> list[int]:
    return sorted(range(len(adjacency)), key=lambda vertex: (-len(adjacency[vertex]), vertex))


def smallest_last(adjacency: list[list[int]]) -> list[int]:
    n = len(adjacency)
    active = [True] * n
    removal = []
    while len(removal) < n:
        vertex = min((sum(active[x] for x in adjacency[v]), v) for v in range(n) if active[v])[1]
        active[vertex] = False
        removal.append(vertex)
    removal.reverse()
    return removal


def dsatur(adjacency: list[list[int]]) -> tuple[list[int], int]:
    n = len(adjacency)
    colors = [n] * n
    for _ in range(n):
        uncolored = [v for v in range(n) if colors[v] == n]
        selected = min(
            uncolored,
            key=lambda v: (
                -len({colors[x] for x in adjacency[v] if colors[x] != n}),
                -len(adjacency[v]),
                v,
            ),
        )
        forbidden = {colors[x] for x in adjacency[selected] if colors[x] != n}
        color = 0
        while color in forbidden:
            color += 1
        colors[selected] = color
    return colors, max(colors, default=-1) + 1


def encode(colors: list[int], count: int) -> str:
    return f"{count} {len(colors)}" + "".join(f" {color}" for color in colors)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071314)
    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (0, []), (1, []),
        (5, [(u, v) for u in range(5) for v in range(u + 1, 5)]),
        (6, [(v, (v + 1) % 6) for v in range(6)]),
    ]
    for _ in range(50):
        n = rng.randrange(1, 65)
        probability = rng.uniform(0.02, 0.35)
        edges = []
        for left in range(n):
            for right in range(left + 1, n):
                if rng.random() < probability:
                    edges.append((left, right))
                    if rng.randrange(15) == 0:
                        edges.append((left, right))
        cases.append((n, edges))
    n = 320
    cases.append((n, [(left, right) for left in range(n) for right in range(left + 1, min(n, left + 9))]))

    inputs = [str(len(cases))]
    outputs = []
    for n, edges in cases:
        inputs.append(f"{n} {len(edges)}")
        inputs.extend(f"{left} {right}" for left, right in edges)
        adjacency = adjacency_list(n, edges)
        plain, plain_count = greedy(adjacency, list(range(n)))
        largest_order = largest_first(adjacency)
        largest, largest_count = greedy(adjacency, largest_order)
        smallest, smallest_count = greedy(adjacency, smallest_last(adjacency))
        saturation, saturation_count = dsatur(adjacency)
        outputs.append(
            f"{encode(plain, plain_count)} | {encode(largest, largest_count)} | "
            f"{encode(smallest, smallest_count)} | {encode(largest, largest_count)} | "
            f"{encode(saturation, saturation_count)}"
        )
    (out_dir / "case_00.in").write_text("\n".join(inputs) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
