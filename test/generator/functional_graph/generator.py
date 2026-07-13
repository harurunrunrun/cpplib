#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MAX_U64 = (1 << 64) - 1


class Decomposition:
    def __init__(self, successor: list[int]) -> None:
        self.successor = successor
        n = len(successor)
        indegree = [0] * n
        for to in successor:
            indegree[to] += 1
        order = [v for v in range(n) if indegree[v] == 0]
        head = 0
        while head < len(order):
            v = order[head]
            head += 1
            to = successor[v]
            indegree[to] -= 1
            if indegree[to] == 0:
                order.append(to)

        self.component = [-1] * n
        self.distance = [-1] * n
        self.entry = [-1] * n
        self.position = [-1] * n
        self.length = [-1] * n
        self.cycles: list[list[int]] = []
        self.component_sizes: list[int] = []
        for start in range(n):
            if indegree[start] == 0 or self.component[start] != -1:
                continue
            component_id = len(self.cycles)
            cycle: list[int] = []
            v = start
            while True:
                self.component[v] = component_id
                self.distance[v] = 0
                self.entry[v] = v
                self.position[v] = len(cycle)
                cycle.append(v)
                v = successor[v]
                if v == start:
                    break
            for v in cycle:
                self.length[v] = len(cycle)
            self.cycles.append(cycle)
            self.component_sizes.append(len(cycle))

        for v in reversed(order):
            to = successor[v]
            component_id = self.component[to]
            self.component[v] = component_id
            self.distance[v] = self.distance[to] + 1
            self.entry[v] = self.entry[to]
            self.length[v] = self.length[to]
            self.component_sizes[component_id] += 1

    def orbit(self, start: int) -> tuple[list[int], int]:
        first: dict[int, int] = {}
        path: list[int] = []
        v = start
        while v not in first:
            first[v] = len(path)
            path.append(v)
            v = self.successor[v]
        return path, first[v]

    def jump(self, start: int, step: int) -> int:
        path, cycle_begin = self.orbit(start)
        if step < len(path):
            return path[step]
        return path[cycle_begin + (step - cycle_begin) % (len(path) - cycle_begin)]

    def steps_to(self, start: int, target: int) -> int:
        path, _ = self.orbit(start)
        try:
            return path.index(target)
        except ValueError:
            return -1


def make_queries(successor: list[int], rng: random.Random) -> list[tuple[int, ...]]:
    n = len(successor)
    if n == 0:
        return []
    decomposition = Decomposition(successor)
    queries: list[tuple[int, ...]] = []
    vertices = list(range(n)) if n <= 30 else [0, n - 1] + [rng.randrange(n) for _ in range(60)]
    for v in vertices:
        queries.append((0, v))
    jump_steps = [0, 1, 2, 63, 64, 65, 10**18, MAX_U64 - 1, MAX_U64]
    for v in vertices[: min(len(vertices), 15)]:
        for step in jump_steps:
            queries.append((1, v, step))
    pair_count = min(100, max(20, n * 2))
    for _ in range(pair_count):
        queries.append((2, rng.randrange(n), rng.randrange(n)))
    for component_id in range(len(decomposition.cycles)):
        queries.append((3, component_id))
    return queries


def expected_lines(successor: list[int], queries: list[tuple[int, ...]]) -> list[str]:
    decomposition = Decomposition(successor)
    lines = [f"{len(successor)} {int(not successor)} {len(decomposition.cycles)}"]
    for query in queries:
        query_type = query[0]
        if query_type == 0:
            v = query[1]
            values = [
                successor[v],
                decomposition.component[v],
                decomposition.component[v],
                int(decomposition.distance[v] == 0),
                decomposition.distance[v],
                decomposition.entry[v],
                decomposition.position[v],
                decomposition.length[v],
                decomposition.distance[v] + decomposition.length[v],
            ]
            lines.append(" ".join(map(str, values)))
        elif query_type == 1:
            lines.append(str(decomposition.jump(query[1], query[2])))
        elif query_type == 2:
            lines.append(str(decomposition.steps_to(query[1], query[2])))
        else:
            component_id = query[1]
            values = [
                decomposition.component_sizes[component_id],
                len(decomposition.cycles[component_id]),
                *decomposition.cycles[component_id],
            ]
            lines.append(" ".join(map(str, values)))
    return lines


def write_case(
    out_dir: Path,
    index: int,
    successor: list[int],
    rng: random.Random,
) -> None:
    queries = make_queries(successor, rng)
    name = f"case_{index:02d}"
    input_lines = [f"{len(successor)} {len(queries)}", " ".join(map(str, successor))]
    input_lines.extend(" ".join(map(str, query)) for query in queries)
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(
        "\n".join(expected_lines(successor, queries)) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20261020)
    cases: list[list[int]] = [
        [],
        [0],
        [1, 0],
        [1, 2, 0, 1, 3, 5],
        [1, 2, 3, 4, 2, 6, 7, 5, 7, 9],
    ]
    for n in [2, 10, 30, 100, 1000]:
        cases.append([rng.randrange(n) for _ in range(n)])
    cases.append([i + 1 for i in range(49999)] + [49999])
    cases.append([i + 1 for i in range(49999)] + [0])

    for index, successor in enumerate(cases):
        write_case(out_dir, index, successor, rng)


if __name__ == "__main__":
    main()
