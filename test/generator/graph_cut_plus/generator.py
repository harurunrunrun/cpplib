#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import itertools
import random
from pathlib import Path


def cut_capacity(
    side: set[int], edges: list[tuple[int, int, int]]
) -> int:
    return sum(
        capacity for u, v, capacity in edges if (u in side) != (v in side)
    )


def global_min_cut(n: int, edges: list[tuple[int, int, int]]) -> int:
    if n <= 1:
        return 0
    return min(
        cut_capacity(
            {vertex for vertex in range(n) if mask >> vertex & 1}, edges
        )
        for mask in range(1, 1 << n)
        if mask != (1 << n) - 1
    )


def pair_min_cut(
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    sink: int,
) -> int:
    return min(
        cut_capacity(
            {vertex for vertex in range(n) if mask >> vertex & 1}, edges
        )
        for mask in range(1 << n)
        if mask >> source & 1 and not (mask >> sink & 1)
    )


def is_disconnected(
    n: int,
    edges: list[tuple[int, int]],
    removed: set[int],
    source: int,
    sink: int,
    undirected: bool,
) -> bool:
    graph: list[list[int]] = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        if undirected and u != v:
            graph[v].append(u)
    seen = {source}
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if to not in removed and to not in seen:
                seen.add(to)
                queue.append(to)
    return sink not in seen


def minimum_vertex_cut(
    n: int,
    capacities: list[int],
    edges: list[tuple[int, int]],
    source: int,
    sink: int,
    undirected: bool,
) -> int | None:
    candidates = [
        vertex for vertex in range(n) if vertex not in (source, sink)
    ]
    answer: int | None = None
    for mask in range(1 << len(candidates)):
        removed = {
            candidates[index]
            for index in range(len(candidates))
            if mask >> index & 1
        }
        cost = sum(capacities[vertex] for vertex in removed)
        if answer is not None and answer <= cost:
            continue
        if is_disconnected(
            n, edges, removed, source, sink, undirected
        ):
            answer = cost
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    weighted_cases: list[
        tuple[int, int, list[tuple[int, int, int]]]
    ] = [
        (0, 0, []),
        (0, 1, []),
        (0, 3, [(0, 1, 4), (0, 1, 2), (1, 2, 3)]),
        (1, 4, [(0, 1, 2), (1, 2, 5), (2, 3, 1), (0, 3, 4)]),
    ]
    for kind in (0, 1):
        for _ in range(90):
            n = rng.randint(0, 8)
            edges: list[tuple[int, int, int]] = []
            for u, v in itertools.combinations(range(n), 2):
                if rng.random() < rng.uniform(0.1, 0.65):
                    edges.append((u, v, rng.randint(0, 30)))
                    if rng.random() < 0.12:
                        edges.append((u, v, rng.randint(0, 20)))
            if n and rng.random() < 0.1:
                u = rng.randrange(n)
                edges.append((u, u, rng.randint(0, 20)))
            weighted_cases.append((kind, n, edges))

    vertex_cases: list[
        tuple[int, int, list[int], list[tuple[int, int]], int, int]
    ] = [
        (2, 2, [5, 5], [(0, 1)], 0, 1),
        (2, 3, [9, 2, 9], [(0, 1), (1, 2)], 0, 2),
        (3, 4, [9, 2, 3, 9], [(0, 1), (1, 3), (0, 2), (2, 3)], 0, 3),
    ]
    for kind in (2, 3):
        for _ in range(120):
            n = rng.randint(2, 9)
            source, sink = rng.sample(range(n), 2)
            capacities = [rng.randint(0, 20) for _ in range(n)]
            edges: list[tuple[int, int]] = []
            if kind == 2:
                for u in range(n):
                    for v in range(n):
                        if u != v and rng.random() < 0.24:
                            edges.append((u, v))
            else:
                for u, v in itertools.combinations(range(n), 2):
                    if rng.random() < 0.34:
                        edges.append((u, v))
            vertex_cases.append((
                kind, n, capacities, edges, source, sink
            ))

    input_lines = [str(len(weighted_cases) + len(vertex_cases))]
    output_lines: list[str] = []
    for kind, n, edges in weighted_cases:
        input_lines.append(f"{kind} {n} {len(edges)}")
        input_lines.extend(f"{u} {v} {capacity}" for u, v, capacity in edges)
        if kind == 0:
            output_lines.append(str(global_min_cut(n, edges)))
        else:
            output_lines.append(" ".join(
                str(pair_min_cut(n, edges, u, v))
                for u, v in itertools.combinations(range(n), 2)
            ))
    for kind, n, capacities, edges, source, sink in vertex_cases:
        input_lines.append(f"{kind} {n} {len(edges)}")
        input_lines.append(f"{source} {sink}")
        input_lines.append(" ".join(map(str, capacities)))
        input_lines.extend(f"{u} {v}" for u, v in edges)
        answer = minimum_vertex_cut(
            n, capacities, edges, source, sink, kind == 3
        )
        output_lines.append(str(-1 if answer is None else answer))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
