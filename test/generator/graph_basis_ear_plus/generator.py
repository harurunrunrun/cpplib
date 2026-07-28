#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import itertools
import random
from pathlib import Path


def minimum_cycle_basis(
    n: int, edges: list[tuple[int, int, int]]
) -> tuple[int, int]:
    candidates: list[tuple[int, int]] = []
    for mask in range(1, 1 << len(edges)):
        parity = [0] * n
        weight = 0
        for edge_id, (u, v, value) in enumerate(edges):
            if mask >> edge_id & 1:
                parity[u] ^= 1
                parity[v] ^= 1
                weight += value
        if not any(parity):
            candidates.append((weight, mask))
    candidates.sort()
    basis = [0] * len(edges)
    total = 0
    rank = 0
    for weight, original in candidates:
        mask = original
        for pivot in range(len(edges) - 1, -1, -1):
            if not (mask >> pivot & 1):
                continue
            if basis[pivot]:
                mask ^= basis[pivot]
            else:
                basis[pivot] = mask
                total += weight
                rank += 1
                break
    return total, rank


def connected_without(
    n: int,
    edges: list[tuple[int, int]],
    removed: int,
) -> bool:
    remaining = [vertex for vertex in range(n) if vertex != removed]
    if len(remaining) <= 1:
        return True
    graph = [[] for _ in range(n)]
    for u, v in edges:
        if u == removed or v == removed:
            continue
        graph[u].append(v)
        graph[v].append(u)
    seen = {remaining[0]}
    queue = collections.deque([remaining[0]])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if to not in seen:
                seen.add(to)
                queue.append(to)
    return all(vertex in seen for vertex in remaining)


def has_ear_decomposition(
    n: int, edges: list[tuple[int, int]]
) -> bool:
    if n < 2 or not edges or any(u == v for u, v in edges):
        return False
    if not connected_without(n, edges, -1):
        return False
    if any(not connected_without(n, edges, vertex) for vertex in range(n)):
        return False
    return len(edges) - n + 1 > 0


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    weighted_cases: list[tuple[int, list[tuple[int, int, int]]]] = [
        (0, []),
        (2, [(0, 1, 3), (0, 1, 5)]),
        (3, [(0, 1, 1), (1, 2, 2), (2, 0, 3), (0, 0, 4)]),
    ]
    for _ in range(150):
        n = rng.randint(0, 7)
        possible = [
            (u, v) for u in range(n) for v in range(u, n)
        ]
        rng.shuffle(possible)
        edge_count = rng.randint(0, min(12, len(possible) + 3))
        edges: list[tuple[int, int, int]] = []
        for _ in range(edge_count):
            if not possible:
                break
            u, v = rng.choice(possible)
            edges.append((u, v, rng.randint(0, 25)))
        weighted_cases.append((n, edges))
    ear_cases: list[tuple[int, list[tuple[int, int]]]] = [
        (2, [(0, 1)]),
        (2, [(0, 1), (0, 1)]),
        (4, [(0, 1), (1, 2), (2, 0), (0, 3), (3, 1)]),
    ]
    for _ in range(190):
        n = rng.randint(0, 9)
        edges: list[tuple[int, int]] = []
        for u, v in itertools.combinations(range(n), 2):
            if rng.random() < 0.35:
                edges.append((u, v))
                if rng.random() < 0.08:
                    edges.append((u, v))
        if n and rng.random() < 0.08:
            vertex = rng.randrange(n)
            edges.append((vertex, vertex))
        rng.shuffle(edges)
        ear_cases.append((n, edges))
    input_lines = [str(len(weighted_cases) + len(ear_cases))]
    output_lines: list[str] = []
    for n, edges in weighted_cases:
        input_lines.append(f"0 {n} {len(edges)}")
        input_lines.extend(f"{u} {v} {weight}" for u, v, weight in edges)
        total, rank = minimum_cycle_basis(n, edges)
        output_lines.append(f"{total} {rank}")
    for n, edges in ear_cases:
        input_lines.append(f"1 {n} {len(edges)}")
        input_lines.extend(f"{u} {v}" for u, v in edges)
        output_lines.append(str(int(has_ear_decomposition(n, edges))))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
