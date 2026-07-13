#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path

MOD = 998244353
Edge = tuple[int, int, int]


def brute_undirected(n: int, edges: list[Edge]) -> int:
    if n == 0:
        return 1
    answer = 0
    for selected in itertools.combinations(range(len(edges)), n - 1):
        parent = list(range(n))

        def find(vertex: int) -> int:
            while parent[vertex] != vertex:
                parent[vertex] = parent[parent[vertex]]
                vertex = parent[vertex]
            return vertex

        product = 1
        tree = True
        for index in selected:
            from_, to, weight = edges[index]
            from_root = find(from_)
            to_root = find(to)
            if from_root == to_root:
                tree = False
                break
            parent[from_root] = to_root
            product = product * weight % MOD
        if tree and all(find(vertex) == find(0) for vertex in range(n)):
            answer = (answer + product) % MOD
    return answer


def brute_out(n: int, root: int, edges: list[Edge]) -> int:
    answer = 0
    for selected in itertools.combinations(range(len(edges)), n - 1):
        indegree = [0] * n
        graph: list[list[int]] = [[] for _ in range(n)]
        product = 1
        for index in selected:
            from_, to, weight = edges[index]
            indegree[to] += 1
            graph[from_].append(to)
            product = product * weight % MOD
        if indegree[root] != 0:
            continue
        if any(indegree[vertex] != 1 for vertex in range(n) if vertex != root):
            continue
        reached = [False] * n
        stack = [root]
        reached[root] = True
        while stack:
            vertex = stack.pop()
            for to in graph[vertex]:
                if not reached[to]:
                    reached[to] = True
                    stack.append(to)
        if all(reached):
            answer = (answer + product) % MOD
    return answer


def brute_in(n: int, root: int, edges: list[Edge]) -> int:
    answer = 0
    for selected in itertools.combinations(range(len(edges)), n - 1):
        outdegree = [0] * n
        reverse: list[list[int]] = [[] for _ in range(n)]
        product = 1
        for index in selected:
            from_, to, weight = edges[index]
            outdegree[from_] += 1
            reverse[to].append(from_)
            product = product * weight % MOD
        if outdegree[root] != 0:
            continue
        if any(outdegree[vertex] != 1 for vertex in range(n) if vertex != root):
            continue
        reached = [False] * n
        stack = [root]
        reached[root] = True
        while stack:
            vertex = stack.pop()
            for from_ in reverse[vertex]:
                if not reached[from_]:
                    reached[from_] = True
                    stack.append(from_)
        if all(reached):
            answer = (answer + product) % MOD
    return answer


def write_case(
    out_dir: Path,
    index: int,
    mode: str,
    n: int,
    edges: list[Edge],
    root: int,
    expected: int | None = None,
) -> None:
    if expected is None:
        if mode == "U":
            expected = brute_undirected(n, edges)
        elif mode == "O":
            expected = brute_out(n, root, edges)
        elif mode == "I":
            expected = brute_in(n, root, edges)
        else:
            raise ValueError(mode)

    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join(
            [
                f"{mode} {n} {len(edges)} {root}",
                *[f"{from_} {to} {weight}" for from_, to, weight in edges],
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        f"{expected % MOD}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases: list[tuple[str, int, list[Edge], int]] = [
        ("U", 0, [], -1),
        ("U", 1, [(0, 0, 5), (0, 0, -3)], -1),
        ("O", 1, [(0, 0, 5), (0, 0, -3)], 0),
        ("I", 1, [(0, 0, 5), (0, 0, -3)], 0),
        ("U", 2, [(0, 1, 2), (0, 1, -3), (1, 0, 5), (0, 0, 99)], -1),
        (
            "O",
            3,
            [(0, 1, 2), (0, 2, 3), (1, 2, 5), (2, 1, 7), (1, 0, -2)],
            0,
        ),
        (
            "I",
            3,
            [(1, 0, 2), (2, 0, 3), (2, 1, 5), (1, 2, 7), (0, 1, -2)],
            0,
        ),
        ("U", 4, [(0, 1, 1), (2, 3, 1)], -1),
    ]

    rng = random.Random(20260801)
    for n in range(1, 7):
        for _ in range(3):
            edges: list[Edge] = []
            for _ in range(rng.randrange(10)):
                edges.append(
                    (
                        rng.randrange(n),
                        rng.randrange(n),
                        rng.randrange(-3, 4),
                    )
                )
            root = rng.randrange(n)
            cases.append(("U", n, edges, -1))
            cases.append(("O", n, edges, root))
            cases.append(("I", n, edges, root))

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)

    large_n = 220
    undirected = [
        (from_, to, 1)
        for from_ in range(large_n)
        for to in range(from_ + 1, large_n)
    ]
    directed = [
        (from_, to, 1)
        for from_ in range(large_n)
        for to in range(large_n)
        if from_ != to
    ]
    expected = pow(large_n, large_n - 2, MOD)
    offset = len(cases)
    write_case(out_dir, offset, "U", large_n, undirected, -1, expected)
    write_case(out_dir, offset + 1, "O", large_n, directed, 97, expected)
    write_case(out_dir, offset + 2, "I", large_n, directed, 97, expected)


if __name__ == "__main__":
    main()
