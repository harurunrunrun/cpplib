#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(
    n: int,
    root: int,
    colors: list[int],
    edges: list[tuple[int, int]],
) -> list[tuple[int, int]]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)

    parent = [-2] * n
    parent[root] = -1
    order = [root]
    for vertex in order:
        for to in graph[vertex]:
            if to == parent[vertex]:
                continue
            parent[to] = vertex
            order.append(to)

    answer: list[tuple[int, int]] = []
    for subtree_root in range(n):
        count = [0] * n
        stack = [subtree_root]
        maximum = 0
        while stack:
            vertex = stack.pop()
            color = colors[vertex]
            count[color] += 1
            maximum = max(maximum, count[color])
            for to in graph[vertex]:
                if parent[to] == vertex:
                    stack.append(to)
        answer.append((maximum, sum(value == maximum for value in count)))
    return answer


def write_case(
    out_dir: Path,
    index: int,
    cases: list[tuple[int, int, list[int], list[tuple[int, int]]]],
) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for n, root, colors, edges in cases:
        input_lines.append(f"{n} {root}")
        input_lines.append(" ".join(map(str, colors)))
        input_lines.extend(f"{u} {v}" for u, v in edges)
        output_lines.extend(
            f"{maximum} {modes}"
            for maximum, modes in solve(n, root, colors, edges)
        )

    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        (1, 0, [0], []),
        (7, 0, [0, 1, 0, 1, 2, 2, 2], [
            (0, 1), (0, 2), (1, 3), (1, 4), (2, 5), (5, 6),
        ]),
        (10, 9, [vertex % 3 for vertex in range(10)], [
            (vertex - 1, vertex) for vertex in range(1, 10)
        ]),
        (12, 0, [0] * 12, [(0, vertex) for vertex in range(1, 12)]),
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260713)
    for file_index, sizes in enumerate((range(2, 35), (64, 127, 256)), 1):
        cases = []
        for n in sizes:
            edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
            rng.shuffle(edges)
            root = rng.randrange(n)
            color_count = rng.randrange(1, n + 1)
            colors = [rng.randrange(color_count) for _ in range(n)]
            cases.append((n, root, colors, edges))
        write_case(out_dir, file_index, cases)


if __name__ == "__main__":
    main()
