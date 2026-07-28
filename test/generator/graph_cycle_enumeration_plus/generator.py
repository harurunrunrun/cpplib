#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import fractions
import itertools
import random
from pathlib import Path


def directed_cycles(
    n: int, edges: list[tuple[int, int]]
) -> list[list[int]]:
    graph = [set() for _ in range(n)]
    for u, v in edges:
        graph[u].add(v)
    result: list[list[int]] = []
    for start in range(n):
        path = [start]
        used = {start}

        def search(vertex: int) -> None:
            for to in sorted(graph[vertex]):
                if to == start:
                    result.append(path.copy())
                elif to > start and to not in used:
                    used.add(to)
                    path.append(to)
                    search(to)
                    path.pop()
                    used.remove(to)

        search(start)
    return sorted(result)


def minimum_mean_cycle(
    n: int, edges: list[tuple[int, int, int]]
) -> fractions.Fraction | None:
    weight: dict[tuple[int, int], int] = {}
    for u, v, value in edges:
        key = (u, v)
        weight[key] = min(weight.get(key, value), value)
    cycles = directed_cycles(n, list(weight))
    if not cycles:
        return None
    return min(
        fractions.Fraction(
            sum(
                weight[(cycle[index], cycle[(index + 1) % len(cycle)])]
                for index in range(len(cycle))
            ),
            len(cycle),
        )
        for cycle in cycles
    )


def maximal_cliques(
    n: int, edges: list[tuple[int, int]]
) -> list[list[int]]:
    edge_set = {
        (min(u, v), max(u, v)) for u, v in edges if u != v
    }
    result: list[list[int]] = []
    for mask in range(1 << n):
        vertices = [v for v in range(n) if mask >> v & 1]
        if any(
            (left, right) not in edge_set
            for left, right in itertools.combinations(vertices, 2)
        ):
            continue
        if any(
            all(
                (min(vertex, inside), max(vertex, inside)) in edge_set
                for inside in vertices
            )
            for vertex in range(n)
            if not (mask >> vertex & 1)
        ):
            continue
        result.append(vertices)
    return sorted(result)


def is_cactus(n: int, edges: list[tuple[int, int]]) -> bool:
    if any(u == v for u, v in edges):
        return False
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for edge_id, (u, v) in enumerate(edges):
        graph[u].append((v, edge_id))
        graph[v].append((u, edge_id))
    order = [-1] * n
    low = [-1] * n
    stack: list[int] = []
    timer = 0
    valid = True

    def check_component(component: list[int]) -> None:
        nonlocal valid
        if len(component) <= 1:
            return
        vertices: set[int] = set()
        degree: dict[int, int] = {}
        for edge_id in component:
            u, v = edges[edge_id]
            vertices.update((u, v))
            degree[u] = degree.get(u, 0) + 1
            degree[v] = degree.get(v, 0) + 1
        if len(component) != len(vertices) or any(
            degree[vertex] != 2 for vertex in vertices
        ):
            valid = False

    def dfs(vertex: int, parent_edge: int) -> None:
        nonlocal timer
        order[vertex] = low[vertex] = timer
        timer += 1
        for to, edge_id in graph[vertex]:
            if edge_id == parent_edge:
                continue
            if order[to] == -1:
                stack.append(edge_id)
                dfs(to, edge_id)
                low[vertex] = min(low[vertex], low[to])
                if order[vertex] <= low[to]:
                    component: list[int] = []
                    while True:
                        current = stack.pop()
                        component.append(current)
                        if current == edge_id:
                            break
                    check_component(component)
            elif order[to] < order[vertex]:
                stack.append(edge_id)
                low[vertex] = min(low[vertex], order[to])

    for vertex in range(n):
        if order[vertex] == -1:
            dfs(vertex, -1)
    return valid


def encode_sets(values: list[list[int]]) -> str:
    output = [str(len(values))]
    for row in values:
        output.extend((str(len(row)), *(str(value) for value in row)))
    return " ".join(output)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, int, list[tuple[int, ...]]]] = [
        (0, 0, []),
        (0, 3, [(0, 1, 4), (1, 2, -2), (2, 0, 1)]),
        (0, 2, [(0, 0, -3), (0, 1, 5), (1, 0, 0)]),
        (1, 0, []),
        (1, 4, [(0, 1), (1, 2), (0, 2), (2, 3)]),
        (2, 3, [(0, 1), (1, 2), (2, 0), (1, 1)]),
        (3, 2, [(0, 1), (0, 1)]),
        (3, 4, [(0, 1), (1, 2), (2, 0), (0, 3), (1, 3)]),
    ]
    for _ in range(100):
        n = rng.randint(0, 8)
        weighted: list[tuple[int, int, int]] = []
        for u in range(n):
            for v in range(n):
                if rng.random() < 0.2:
                    weighted.append((u, v, rng.randint(-20, 25)))
                    if rng.random() < 0.08:
                        weighted.append((u, v, rng.randint(-20, 25)))
        cases.append((0, n, weighted))
    for kind in (1, 3):
        for _ in range(100):
            n = rng.randint(0, 10)
            edges: list[tuple[int, int]] = []
            for u, v in itertools.combinations(range(n), 2):
                if rng.random() < 0.3:
                    edges.append((u, v))
                    if rng.random() < 0.06:
                        edges.append((u, v))
            if n and rng.random() < 0.08:
                u = rng.randrange(n)
                edges.append((u, u))
            cases.append((kind, n, edges))
    for _ in range(100):
        n = rng.randint(0, 8)
        edges = [
            (u, v)
            for u in range(n)
            for v in range(n)
            if rng.random() < 0.18
        ]
        cases.append((2, n, edges))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for kind, n, raw_edges in cases:
        input_lines.append(f"{kind} {n} {len(raw_edges)}")
        input_lines.extend(" ".join(map(str, edge)) for edge in raw_edges)
        if kind == 0:
            answer = minimum_mean_cycle(
                n, [tuple(edge) for edge in raw_edges]  # type: ignore[arg-type]
            )
            output_lines.append(
                "none" if answer is None
                else f"{answer.numerator} {answer.denominator}"
            )
        elif kind == 1:
            output_lines.append(encode_sets(maximal_cliques(
                n, [tuple(edge) for edge in raw_edges]  # type: ignore[arg-type]
            )))
        elif kind == 2:
            output_lines.append(encode_sets(directed_cycles(
                n, [tuple(edge) for edge in raw_edges]  # type: ignore[arg-type]
            )))
        else:
            output_lines.append(str(int(is_cactus(
                n, [tuple(edge) for edge in raw_edges]  # type: ignore[arg-type]
            ))))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
