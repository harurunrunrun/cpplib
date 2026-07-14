#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_N = 500
MAX_M = 10_000
MAX_WEIGHT = 1_000
Edge = tuple[int, int, int]
Case = tuple[int, list[Edge], int, int]


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


def solve_brute(case: Case) -> int | None:
    n, edges, source, target = case
    shortest, removed = shortest_path_edges_brute(
        n, edges, source, target
    )
    if shortest is None:
        return None
    answer, _ = shortest_path_edges_brute(
        n, edges, source, target, removed
    )
    return answer


def validate(case: Case) -> None:
    n, edges, source, target = case
    assert 2 <= n <= MAX_N
    assert 1 <= len(edges) <= MAX_M
    assert 0 <= source < n and 0 <= target < n and source != target
    assert len({(from_, to) for from_, to, _ in edges}) == len(edges)
    assert all(
        0 <= from_ < n and 0 <= to < n and from_ != to
        and 1 <= weight <= MAX_WEIGHT
        for from_, to, weight in edges
    )


def write_cases(
    out_dir: Path,
    name: str,
    cases: list[Case],
    answers: list[int | None] | None = None,
) -> None:
    assert cases
    for case in cases:
        validate(case)
    if answers is None:
        answers = [solve_brute(case) for case in cases]
    assert len(answers) == len(cases)

    input_lines: list[str] = []
    for n, edges, source, target in cases:
        input_lines.extend([
            f"{n} {len(edges)}",
            f"{source} {target}",
            *(f"{from_} {to} {weight}" for from_, to, weight in edges),
        ])
    input_lines.append("0 0")
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(
            str(answer if answer is not None else -1) for answer in answers
        ) + "\n",
        encoding="ascii",
    )


def maximum_edge_case() -> Case:
    n = MAX_N
    edges: list[Edge] = [
        (0, 1, 1), (1, n - 1, 1),
        (0, 2, 3), (2, n - 1, 3),
    ]
    used_pairs = {(from_, to) for from_, to, _ in edges}
    for from_ in range(n):
        for to in range(n):
            if from_ == to or (from_, to) in used_pairs:
                continue
            edges.append((from_, to, MAX_WEIGHT))
            used_pairs.add((from_, to))
            if len(edges) == MAX_M:
                return n, edges, 0, n - 1
    raise AssertionError("enough directed pairs must exist")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official: list[Case] = [
        (7, [
            (0, 1, 1), (0, 2, 1), (0, 3, 2), (0, 4, 3),
            (1, 5, 2), (2, 6, 4), (3, 6, 2), (4, 6, 4),
            (5, 6, 1),
        ], 0, 6),
        (4, [
            (0, 1, 1), (1, 2, 1), (1, 3, 1),
            (3, 2, 1), (2, 0, 3), (3, 0, 2),
        ], 0, 2),
        (6, [
            (0, 1, 1), (0, 2, 2), (0, 3, 3), (2, 5, 3),
            (3, 4, 2), (4, 1, 1), (5, 1, 1), (3, 0, 1),
        ], 0, 1),
    ]
    write_cases(out_dir, "official", official, [5, None, 6])

    fixed: list[Case] = [
        (4, [(0, 1, 1), (1, 3, 1), (0, 2, 1), (2, 3, 1),
             (0, 3, 3)], 0, 3),
        (3, [(0, 1, 4)], 0, 2),
        (2, [(0, 1, 4)], 0, 1),
        (5, [(0, 1, 1), (1, 4, 1), (0, 2, 2), (2, 3, 2),
             (3, 4, 2), (4, 0, 7)], 0, 4),
    ]
    write_cases(out_dir, "fixed", fixed)

    source = random.Random(5070127)
    for file_index in range(10):
        cases: list[Case] = []
        for _ in range(16):
            n = source.randint(2, 8)
            source_vertex = source.randrange(n)
            target = source.randrange(n - 1)
            if target >= source_vertex:
                target += 1
            edges = [
                (from_, to, source.randint(1, 9))
                for from_ in range(n)
                for to in range(n)
                if from_ != to and source.randrange(6) == 0
            ]
            if not edges:
                edges.append((source_vertex, target, source.randint(1, 9)))
            cases.append((n, edges, source_vertex, target))
        write_cases(out_dir, f"random_{file_index:02d}", cases)

    max_edges = maximum_edge_case()
    write_cases(out_dir, "max_n_m_weight", [max_edges], [6])

    max_distance_edges: list[Edge] = [(0, MAX_N - 1, 1)]
    max_distance_edges.extend(
        (vertex, vertex + 1, MAX_WEIGHT)
        for vertex in range(MAX_N - 1)
    )
    write_cases(
        out_dir,
        "max_distance",
        [(MAX_N, max_distance_edges, 0, MAX_N - 1)],
        [(MAX_N - 1) * MAX_WEIGHT],
    )


if __name__ == "__main__":
    main()
