#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def distance(n: int, edges: list[tuple[int, int]], source: int, target: int) -> int:
    graph: list[list[int]] = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    result = [-1] * n
    result[source] = 0
    queue: deque[int] = deque([source])
    while queue:
        vertex = queue.popleft()
        for to in graph[vertex]:
            if result[to] != -1:
                continue
            result[to] = result[vertex] + 1
            queue.append(to)
    return result[target]


def write_case(
    output_directory: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int]],
    source: int,
    target: int,
) -> None:
    answer = distance(n, edges, source, target)
    stem = f"case_{index:02d}"
    content = [f"{n} {len(edges)} {source} {target}"]
    content.extend(f"{u} {v}" for u, v in edges)
    (output_directory / f"{stem}.in").write_text("\n".join(content) + "\n", encoding="utf-8")
    (output_directory / f"{stem}.out").write_text(f"{answer} {answer}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int]], int, int]] = [
        (1, [], 0, 0),
        (5, [(0, 1), (1, 2), (3, 4)], 0, 4),
        (8, [(i, i + 1) for i in range(7)], 0, 7),
        (7, [(0, 1), (0, 2), (1, 3), (1, 4), (2, 5), (2, 6)], 3, 6),
    ]
    random_engine = random.Random(314159265)
    for n in [6, 8, 10, 12]:
        edge_set = {(i, i + 1) for i in range(n - 1)}
        for u in range(n):
            for v in range(u + 2, n):
                if random_engine.randrange(7) == 0:
                    edge_set.add((u, v))
        cases.append((n, sorted(edge_set), random_engine.randrange(n), random_engine.randrange(n)))

    for index, case in enumerate(cases):
        write_case(output_directory, index, *case)


if __name__ == "__main__":
    main()
