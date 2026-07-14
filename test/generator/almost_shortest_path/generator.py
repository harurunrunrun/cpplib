#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]


def shortest_path_edges_brute(
    n: int,
    edges: list[Edge],
    source: int,
    target: int,
    banned: set[int] | None = None,
) -> tuple[int | None, set[int]]:
    banned = banned or set()
    graph: list[list[int]] = [[] for _ in range(n)]
    for edge_index, (from_, _, _) in enumerate(edges):
        if edge_index not in banned:
            graph[from_].append(edge_index)

    best: int | None = None
    used_on_best: set[int] = set()
    path: list[int] = []
    seen = [False] * n

    def dfs(vertex: int, cost: int) -> None:
        nonlocal best, used_on_best
        if best is not None and cost > best:
            return
        if vertex == target:
            if best is None or cost < best:
                best = cost
                used_on_best = set(path)
            elif cost == best:
                used_on_best.update(path)
            return
        seen[vertex] = True
        for edge_index in graph[vertex]:
            _, to, weight = edges[edge_index]
            if seen[to]:
                continue
            path.append(edge_index)
            dfs(to, cost + weight)
            path.pop()
        seen[vertex] = False

    dfs(source, 0)
    return best, used_on_best


def solve_brute(
    n: int, edges: list[Edge], source: int, target: int
) -> int | None:
    shortest, removed = shortest_path_edges_brute(
        n, edges, source, target
    )
    if shortest is None:
        return None
    answer, _ = shortest_path_edges_brute(
        n, edges, source, target, removed
    )
    return answer


def write_case(
    out_dir: Path,
    case_index: int,
    n: int,
    edges: list[Edge],
    source: int,
    target: int,
) -> None:
    answer = solve_brute(n, edges, source, target)
    name = f"case_{case_index:03d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join([
            f"{n} {len(edges)} {source} {target}",
            *(f"{from_} {to} {weight}" for from_, to, weight in edges),
        ]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        f"{answer if answer is not None else -1}\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[Edge], int, int]] = [
        (7, [
            (0, 1, 1), (0, 2, 1), (0, 3, 2), (0, 4, 3),
            (1, 5, 2), (2, 6, 4), (3, 6, 2), (4, 6, 4),
            (5, 6, 1),
        ], 0, 6),
        (2, [(0, 1, 1), (0, 1, 1), (0, 1, 3)], 0, 1),
        (4, [
            (0, 1, 0), (0, 2, 0), (1, 3, 1), (2, 3, 1),
            (0, 3, 2),
        ], 0, 3),
        (3, [(0, 1, 4)], 0, 2),
        (2, [(0, 1, 4)], 0, 1),
        (3, [(0, 1, 0), (1, 0, 0), (0, 2, 1)], 0, 0),
    ]

    source = random.Random(5070127)
    for _ in range(120):
        n = source.randint(1, 8)
        edge_count = source.randint(0, 14 if n > 1 else 0)
        edges: list[Edge] = []
        for _ in range(edge_count):
            from_ = source.randrange(n)
            to = source.randrange(n - 1)
            if to >= from_:
                to += 1
            edges.append((from_, to, source.randint(1, 7)))
        cases.append((n, edges, source.randrange(n), source.randrange(n)))

    for _ in range(120):
        n = source.randint(1, 9)
        edges = []
        if n > 1:
            for _ in range(source.randint(0, 18)):
                from_ = source.randrange(n - 1)
                to = source.randrange(from_ + 1, n)
                edges.append((from_, to, source.randint(0, 7)))
        cases.append((n, edges, source.randrange(n), source.randrange(n)))

    for case_index, case in enumerate(cases):
        write_case(out_dir, case_index, *case)


if __name__ == "__main__":
    main()
