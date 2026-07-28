#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def core_numbers(n: int, edges: list[tuple[int, int]]) -> list[int]:
    graph: list[list[int]] = [[] for _ in range(n)]
    for u, v in edges:
        if u == v:
            continue
        graph[u].append(v)
        graph[v].append(u)
    answer = [0] * n
    maximum = max((len(row) for row in graph), default=0)
    for level in range(1, maximum + 1):
        active = [True] * n
        degree = [len(row) for row in graph]
        queue = collections.deque(
            vertex for vertex in range(n) if degree[vertex] < level
        )
        while queue:
            vertex = queue.popleft()
            if not active[vertex]:
                continue
            active[vertex] = False
            for to in graph[vertex]:
                if active[to]:
                    degree[to] -= 1
                    if degree[to] < level:
                        queue.append(to)
        for vertex in range(n):
            if active[vertex]:
                answer[vertex] = level
    return answer


def connected_without(
    n: int, edges: list[tuple[int, int]], removed: int
) -> bool:
    if n == 0:
        return True
    graph: list[list[int]] = [[] for _ in range(n)]
    for index, (u, v) in enumerate(edges):
        if index == removed or u == v:
            continue
        graph[u].append(v)
        graph[v].append(u)
    seen = {0}
    queue = collections.deque([0])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if to not in seen:
                seen.add(to)
                queue.append(to)
    return len(seen) == n


def has_strong_orientation(n: int, edges: list[tuple[int, int]]) -> bool:
    if not connected_without(n, edges, -1):
        return False
    return all(
        u == v or connected_without(n, edges, index)
        for index, (u, v) in enumerate(edges)
    )


def reachable(
    n: int,
    edges: list[tuple[int, int]],
    source: int,
    skipped_pair: tuple[int, int] | None = None,
) -> set[int]:
    graph: list[list[int]] = [[] for _ in range(n)]
    for edge in set(edges):
        if edge == skipped_pair:
            continue
        graph[edge[0]].append(edge[1])
    seen: set[int] = set()
    stack = [source]
    while stack:
        vertex = stack.pop()
        for to in graph[vertex]:
            if to not in seen:
                seen.add(to)
                stack.append(to)
    return seen


def transitive_reduction(
    n: int, edges: list[tuple[int, int]]
) -> list[tuple[int, int]]:
    return sorted(
        edge for edge in set(edges)
        if edge[1] not in reachable(n, edges, edge[0], edge)
    )


def matching_size(graph: list[list[int]]) -> int:
    right_match = [-1] * len(graph)

    def augment(left: int, seen: list[bool]) -> bool:
        for right in graph[left]:
            if seen[right]:
                continue
            seen[right] = True
            if right_match[right] == -1 or augment(right_match[right], seen):
                right_match[right] = left
                return True
        return False

    return sum(
        augment(left, [False] * len(graph)) for left in range(len(graph))
    )


def dag_answer(
    kind: int, n: int, edges: list[tuple[int, int]]
) -> str:
    if kind == 2:
        reduced = transitive_reduction(n, edges)
        flat = " ".join(f"{u} {v}" for u, v in reduced)
        return f"{len(reduced)}" + (f" {flat}" if flat else "")
    if kind == 3:
        graph = [[] for _ in range(n)]
        for u, v in set(edges):
            graph[u].append(v)
        return str(n - matching_size(graph))
    closure = [reachable(n, edges, vertex) for vertex in range(n)]
    graph = [sorted(row) for row in closure]
    return str(n - matching_size(graph))


def random_undirected(
    rng: random.Random, n: int, probability: float
) -> list[tuple[int, int]]:
    edges: list[tuple[int, int]] = []
    for u in range(n):
        for v in range(u + 1, n):
            if rng.random() < probability:
                edges.append((u, v))
                if rng.random() < 0.08:
                    edges.append((u, v))
    if n and rng.random() < 0.25:
        vertex = rng.randrange(n)
        edges.append((vertex, vertex))
    rng.shuffle(edges)
    return edges


def random_dag(
    rng: random.Random, n: int, probability: float
) -> list[tuple[int, int]]:
    permutation = list(range(n))
    rng.shuffle(permutation)
    edges: list[tuple[int, int]] = []
    for left in range(n):
        for right in range(left + 1, n):
            if rng.random() < probability:
                edge = (permutation[left], permutation[right])
                edges.append(edge)
                if rng.random() < 0.05:
                    edges.append(edge)
    rng.shuffle(edges)
    return edges


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, int, list[tuple[int, int]]]] = [
        (0, 0, []),
        (0, 3, [(0, 1), (0, 1), (1, 2), (2, 2)]),
        (1, 0, []),
        (1, 1, []),
        (1, 2, [(0, 1), (0, 1)]),
        (2, 4, [(0, 1), (1, 2), (0, 2), (2, 3), (0, 3)]),
        (3, 0, []),
        (4, 0, []),
    ]
    for kind in (0, 1):
        for _ in range(120):
            n = rng.randint(0, 18)
            cases.append((
                kind,
                n,
                random_undirected(rng, n, rng.uniform(0.03, 0.55)),
            ))
    for kind in (2, 3, 4):
        for _ in range(100):
            n = rng.randint(0, 13)
            cases.append((
                kind,
                n,
                random_dag(rng, n, rng.uniform(0.02, 0.65)),
            ))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for kind, n, edges in cases:
        input_lines.append(f"{kind} {n} {len(edges)}")
        input_lines.extend(f"{u} {v}" for u, v in edges)
        if kind == 0:
            core = core_numbers(n, edges)
            output_lines.append(
                " ".join(map(str, [max(core, default=0), *core]))
            )
        elif kind == 1:
            output_lines.append(str(int(has_strong_orientation(n, edges))))
        else:
            output_lines.append(dag_answer(kind, n, edges))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
