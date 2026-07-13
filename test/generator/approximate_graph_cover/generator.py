#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def graph(n: int, edges: list[tuple[int, int]]) -> tuple[list[list[int]], list[bool]]:
    adjacency = [set() for _ in range(n)]
    loops = [False] * n
    for left, right in edges:
        if left == right:
            loops[left] = True
        else:
            adjacency[left].add(right)
            adjacency[right].add(left)
    return [sorted(values) for values in adjacency], loops


def vertex_cover(n: int, edges: list[tuple[int, int]]) -> tuple[list[int], list[int]]:
    selected = [False] * n
    matched = [False] * n
    for left, right in edges:
        if left == right:
            selected[left] = matched[left] = True
    matching = []
    for index, (left, right) in enumerate(edges):
        if left == right or selected[left] or selected[right]:
            continue
        if not matched[left] and not matched[right]:
            selected[left] = selected[right] = True
            matched[left] = matched[right] = True
            matching.append(index)
    return [v for v in range(n) if selected[v]], matching


def dominating_set(n: int, adjacency: list[list[int]]) -> list[int]:
    dominated = [False] * n
    result = []
    while not all(dominated):
        gains = [sum(not dominated[x] for x in [v] + adjacency[v]) for v in range(n)]
        selected = min(range(n), key=lambda v: (-gains[v], v))
        result.append(selected)
        dominated[selected] = True
        for neighbor in adjacency[selected]:
            dominated[neighbor] = True
    return result


def independent_set(n: int, adjacency: list[list[int]], loops: list[bool]) -> list[int]:
    blocked = [False] * n
    result = []
    for vertex in range(n):
        if blocked[vertex] or loops[vertex]:
            continue
        result.append(vertex)
        blocked[vertex] = True
        for neighbor in adjacency[vertex]:
            blocked[neighbor] = True
    return result


def minimum_degree_independent_set(n: int, adjacency: list[list[int]], loops: list[bool]) -> list[int]:
    active = [not value for value in loops]
    result = []
    while any(active):
        selected = min((sum(active[x] for x in adjacency[v]), v) for v in range(n) if active[v])[1]
        result.append(selected)
        active[selected] = False
        for neighbor in adjacency[selected]:
            active[neighbor] = False
    return result


def encode(values: list[int]) -> str:
    return str(len(values)) + "".join(f" {value}" for value in values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071312)
    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (0, []), (1, []), (1, [(0, 0)]),
        (5, [(0, 1), (0, 1), (1, 2), (3, 3), (3, 4)]),
    ]
    for _ in range(45):
        n = rng.randrange(1, 34)
        edges = []
        for _ in range(rng.randrange(0, n * 4 + 1)):
            edge = (rng.randrange(n), rng.randrange(n))
            edges.append(edge)
            if rng.randrange(8) == 0:
                edges.append(edge)
        cases.append((n, edges))
    n = 500
    cases.append((n, [(v, (v + 1) % n) for v in range(n)] + [(v, (v + 17) % n) for v in range(n)]))

    inputs = [str(len(cases))]
    outputs = []
    for n, edges in cases:
        inputs.append(f"{n} {len(edges)}")
        inputs.extend(f"{left} {right}" for left, right in edges)
        adjacency, loops = graph(n, edges)
        cover, matching = vertex_cover(n, edges)
        dominating = dominating_set(n, adjacency)
        independent = independent_set(n, adjacency, loops)
        minimum_degree = minimum_degree_independent_set(n, adjacency, loops)
        outputs.append(
            f"{encode(cover)} | {encode(matching)} | {encode(dominating)} | "
            f"{encode(independent)} | {encode(minimum_degree)}"
        )
    (out_dir / "case_00.in").write_text("\n".join(inputs) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
