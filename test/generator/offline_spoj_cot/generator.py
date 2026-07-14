#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def path_vertices(n: int, edges: list[tuple[int, int]], source: int, target: int) -> list[int]:
    graph = [[] for _ in range(n)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    parent = [-1] * n
    parent[source] = source
    queue = deque([source])
    while queue:
        vertex = queue.popleft()
        for next_vertex in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                queue.append(next_vertex)
    result = []
    current = target
    while current != source:
        result.append(current)
        current = parent[current]
    result.append(source)
    return result


def write_case(
    directory: Path,
    name: str,
    values: list[int],
    edges: list[tuple[int, int]],
    pairs: list[tuple[int, int]],
) -> None:
    n = len(values)
    queries: list[tuple[int, int, int]] = []
    output: list[str] = []
    for index, (left, right) in enumerate(pairs):
        path = path_vertices(n, edges, left, right)
        order = index % len(path)
        queries.append((left, right, order))
        output.append(str(sorted(values[vertex] for vertex in path)[order]))
    lines = [f"{n} {len(queries)}", " ".join(map(str, values))]
    lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    lines.extend(f"{left + 1} {right + 1} {order + 1}" for left, right, order in queries)
    (directory / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / f"{name}.out").write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    directory = Path(parser.parse_args().out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071508)

    write_case(directory, "case_00_single", [-2_147_483_648], [], [(0, 0)] * 5)
    write_case(directory, "case_01_duplicates", [7] * 9, [(i - 1, i) for i in range(1, 9)], [(0, 8), (3, 7), (4, 4)])
    for case in range(2, 12):
        n = 10 + case * 9
        values = [rng.randrange(-1_000_000_000, 1_000_000_001) for _ in range(n)]
        for index in range(0, n, 7):
            values[index] = values[0]
        edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
        pairs = [(rng.randrange(n), rng.randrange(n)) for _ in range(300)]
        write_case(directory, f"case_{case:02d}_random", values, edges, pairs)


if __name__ == "__main__":
    main()
