#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import random
from pathlib import Path


def tour_cost(distance: list[list[int]], tour: list[int]) -> int:
    if not tour:
        return 0
    return sum(distance[tour[i]][tour[(i + 1) % len(tour)]] for i in range(len(tour)))


def prim(distance: list[list[int]], root: int) -> list[int]:
    n = len(distance)
    if not n:
        return []
    parent = [-1] * n
    best: list[int | None] = [None] * n
    used = [False] * n
    used[root] = True
    for vertex in range(n):
        if vertex != root:
            best[vertex] = distance[root][vertex]
            parent[vertex] = root
    for _ in range(1, n):
        selected = min(
            (best[v], v) for v in range(n) if not used[v] and best[v] is not None
        )[1]
        used[selected] = True
        for vertex in range(n):
            if not used[vertex] and (best[vertex] is None or distance[selected][vertex] < best[vertex]):
                best[vertex] = distance[selected][vertex]
                parent[vertex] = selected
    return parent


def double_tree(parent: list[int], root: int) -> list[int]:
    n = len(parent)
    if not n:
        return []
    tree = [[] for _ in range(n)]
    for vertex, other in enumerate(parent):
        if other >= 0:
            tree[vertex].append(other)
            tree[other].append(vertex)
    for neighbors in tree:
        neighbors.sort()
    result = []
    stack = [(root, -1)]
    while stack:
        vertex, previous = stack.pop()
        result.append(vertex)
        for other in reversed(tree[vertex]):
            if other != previous:
                stack.append((other, vertex))
    return result


def christofides(distance: list[list[int]], parent: list[int], root: int) -> list[int]:
    n = len(distance)
    if not n:
        return []
    degree = [0] * n
    edges = []
    for vertex, other in enumerate(parent):
        if other >= 0:
            edges.append((vertex, other))
            degree[vertex] += 1
            degree[other] += 1
    odd = [v for v in range(n) if degree[v] % 2]
    choice: dict[int, int] = {}

    @functools.cache
    def matching(mask: int) -> int:
        if not mask:
            return 0
        first = (mask & -mask).bit_length() - 1
        rest = mask ^ (1 << first)
        best: int | None = None
        selected = -1
        for second in range(first + 1, len(odd)):
            if rest & (1 << second):
                candidate = distance[odd[first]][odd[second]] + matching(rest ^ (1 << second))
                if best is None or candidate < best:
                    best = candidate
                    selected = second
        choice[mask] = selected
        assert best is not None
        return best

    mask = (1 << len(odd)) - 1
    matching(mask)
    while mask:
        first = (mask & -mask).bit_length() - 1
        second = choice[mask]
        edges.append((odd[first], odd[second]))
        mask ^= 1 << first
        mask ^= 1 << second

    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for edge_id, (u, v) in enumerate(edges):
        graph[u].append((v, edge_id))
        graph[v].append((u, edge_id))
    edge_used = [False] * len(edges)
    cursor = [0] * n
    stack = [root]
    euler = []
    while stack:
        vertex = stack[-1]
        while cursor[vertex] < len(graph[vertex]) and edge_used[graph[vertex][cursor[vertex]][1]]:
            cursor[vertex] += 1
        if cursor[vertex] == len(graph[vertex]):
            euler.append(vertex)
            stack.pop()
        else:
            other, edge_id = graph[vertex][cursor[vertex]]
            cursor[vertex] += 1
            edge_used[edge_id] = True
            stack.append(other)
    euler.reverse()
    seen = [False] * n
    result = []
    for vertex in euler:
        if not seen[vertex]:
            seen[vertex] = True
            result.append(vertex)
    return result


def exact_tsp(distance: list[list[int]]) -> int:
    n = len(distance)
    if n < 2:
        return 0
    inf = 10**30
    dp = [[inf] * n for _ in range(1 << n)]
    dp[1][0] = 0
    for mask in range(1 << n):
        for last in range(n):
            if dp[mask][last] == inf:
                continue
            for nxt in range(1, n):
                if not (mask >> nxt) & 1:
                    new_mask = mask | (1 << nxt)
                    dp[new_mask][nxt] = min(
                        dp[new_mask][nxt],
                        dp[mask][last] + distance[last][nxt],
                    )
    return min(dp[-1][last] + distance[last][0] for last in range(1, n))


def encode(values: list[int]) -> str:
    return str(len(values)) + "".join(f" {value}" for value in values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071303)
    instances: list[tuple[list[list[int]], int]] = [([], 0), ([[0]], 0)]
    for _ in range(36):
        n = rng.randrange(2, 13)
        points = [(rng.randrange(-50, 51), rng.randrange(-50, 51)) for _ in range(n)]
        distance = [
            [abs(x1 - x2) + abs(y1 - y2) for x2, y2 in points]
            for x1, y1 in points
        ]
        instances.append((distance, rng.randrange(n)))
    n = 180
    distance = [[abs(i - j) for j in range(n)] for i in range(n)]
    instances.append((distance, 97))

    commands = []
    outputs = []
    for distance, root in instances:
        n = len(distance)
        parent = prim(distance, root)
        doubled = double_tree(parent, root)
        christ = christofides(distance, parent, root)
        if 1 < n <= 9:
            optimum = exact_tsp(distance)
            assert tour_cost(distance, doubled) <= 2 * optimum
            assert 2 * tour_cost(distance, christ) <= 3 * optimum
        commands.append(
            f"{n} {root} "
            + " ".join(str(value) for row in distance for value in row)
        )
        outputs.append(
            f"{encode(parent)} | {encode(doubled)} {tour_cost(distance, doubled)}"
            f" | {encode(christ)} {tour_cost(distance, christ)}"
        )
    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
