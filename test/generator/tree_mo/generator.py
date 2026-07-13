#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[
    int,
    int,
    list[int],
    list[tuple[int, int]],
    list[tuple[int, int]],
]


def solve(case: Case) -> list[int]:
    n, root, values, edges, queries = case
    graph = [[] for _ in range(n)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)

    parent = [-2] * n
    depth = [0] * n
    parent[root] = -1
    order = [root]
    for vertex in order:
        for to in graph[vertex]:
            if to == parent[vertex]:
                continue
            parent[to] = vertex
            depth[to] = depth[vertex] + 1
            order.append(to)

    answer: list[int] = []
    for query_left, query_right in queries:
        left = query_left
        right = query_right
        path_values: set[int] = set()
        while left != right:
            if depth[left] >= depth[right]:
                path_values.add(values[left])
                left = parent[left]
            else:
                path_values.add(values[right])
                right = parent[right]
        path_values.add(values[left])
        answer.append(len(path_values))
    return answer


def write_case(out_dir: Path, index: int, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, root, values, edges, queries in cases:
        input_lines.append(f"{n} {root} {len(queries)}")
        input_lines.append(" ".join(map(str, values)))
        input_lines.extend(f"{left} {right}" for left, right in edges)
        input_lines.extend(f"{left} {right}" for left, right in queries)
        output_lines.extend(map(str, solve((n, root, values, edges, queries))))

    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + ("\n" if output_lines else ""),
        encoding="utf-8",
    )


def random_case(rng: random.Random, n: int, query_count: int) -> Case:
    edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
    rng.shuffle(edges)
    value_count = rng.randrange(1, n + 1)
    values = [rng.randrange(value_count) for _ in range(n)]
    queries = [(rng.randrange(n), rng.randrange(n)) for _ in range(query_count)]
    return n, rng.randrange(n), values, edges, queries


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed: list[Case] = [
        (1, 0, [0], [], [(0, 0), (0, 0)]),
        (
            7,
            0,
            [0, 1, 0, 2, 1, 2, 3],
            [(0, 1), (0, 2), (1, 3), (1, 4), (2, 5), (5, 6)],
            [(3, 4), (3, 6), (2, 2), (0, 6), (6, 3)],
        ),
        (
            32,
            31,
            [vertex % 5 for vertex in range(32)],
            [(vertex - 1, vertex) for vertex in range(1, 32)],
            [(0, 31), (7, 19), (31, 31), (4, 28), (28, 4)],
        ),
        (
            40,
            17,
            [0] * 20 + list(range(20)),
            [(0, vertex) for vertex in range(1, 40)],
            [(1, 2), (0, 39), (17, 17), (20, 39), (39, 20)],
        ),
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260714)
    write_case(
        out_dir,
        1,
        [random_case(rng, n, n + 25) for n in range(2, 65)],
    )
    write_case(
        out_dir,
        2,
        [
            random_case(rng, n, min(4 * n, 1200))
            for n in (127, 256, 511, 768, 1024)
        ],
    )

    n = 1024
    path_edges = [(vertex - 1, vertex) for vertex in range(1, n)]
    path_queries = [
        (rng.randrange(n), rng.randrange(n))
        for _ in range(3000)
    ]
    write_case(
        out_dir,
        3,
        [(
            n,
            n - 1,
            [vertex % 97 for vertex in range(n)],
            path_edges,
            path_queries,
        )],
    )


if __name__ == "__main__":
    main()
