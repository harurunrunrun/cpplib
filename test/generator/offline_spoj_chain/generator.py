#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path

Statement = tuple[int, int, int]


def solve(animal_count: int, statements: list[Statement]) -> int:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(animal_count)]
    false_count = 0
    for relation, left, right in statements:
        required = 0 if relation == 1 else 2
        if not (0 <= left < animal_count and 0 <= right < animal_count):
            false_count += 1
            continue
        if relation == 2 and left == right:
            false_count += 1
            continue
        known = None
        difference = [None] * animal_count
        difference[left] = 0
        queue = deque([left])
        while queue:
            vertex = queue.popleft()
            if vertex == right:
                known = (-difference[vertex]) % 3
                break
            for next_vertex, edge_difference in graph[vertex]:
                if difference[next_vertex] is None:
                    difference[next_vertex] = (difference[vertex] - edge_difference) % 3
                    queue.append(next_vertex)
        if known is not None and known != required:
            false_count += 1
            continue
        if known is None:
            graph[left].append((right, required))
            graph[right].append((left, (-required) % 3))
    return false_count


def write_case(out_dir: Path, name: str, tests: list[tuple[int, list[Statement]]], answers: list[int] | None = None) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for index, (animal_count, statements) in enumerate(tests):
        input_lines.append(f"{animal_count} {len(statements)}")
        input_lines.extend(f"{relation} {left + 1} {right + 1}" for relation, left, right in statements)
        output_lines.append(str(solve(animal_count, statements) if answers is None else answers[index]))
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official", [(100, [
        (1, 100, 0), (2, 0, 1), (2, 1, 2), (2, 2, 2),
        (1, 0, 2), (2, 2, 0), (1, 4, 4),
    ])], [3])
    write_case(out_dir, "basic", [
        (1, [(1, 0, 0), (2, 0, 0), (1, -1, 0)]),
        (3, [(2, 0, 1), (2, 1, 2), (2, 2, 0), (1, 0, 1)]),
    ])

    source = random.Random(149)
    tests = []
    for _ in range(45):
        animal_count = source.randint(1, 25)
        statements = []
        for _ in range(source.randint(1, 100)):
            statements.append((
                source.randint(1, 2),
                source.randint(-1, animal_count),
                source.randint(-1, animal_count),
            ))
        tests.append((animal_count, statements))
    write_case(out_dir, "random", tests)

    animal_count = 50000
    statements = [(1, index - 1, index) for index in range(1, animal_count)]
    statements.extend((2, index % animal_count, (index + 1) % animal_count) for index in range(50001))
    write_case(out_dir, "maximum", [(animal_count, statements)], [50001])


if __name__ == "__main__":
    main()
