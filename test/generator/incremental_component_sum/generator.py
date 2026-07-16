#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def component(start: int, adjacency: list[set[int]]) -> set[int]:
    seen = {start}
    stack = [start]
    while stack:
        vertex = stack.pop()
        for neighbor in adjacency[vertex]:
            if neighbor not in seen:
                seen.add(neighbor)
                stack.append(neighbor)
    return seen


def component_count(adjacency: list[set[int]]) -> int:
    unseen = set(range(len(adjacency)))
    answer = 0
    while unseen:
        reached = component(next(iter(unseen)), adjacency)
        unseen.difference_update(reached)
        answer += 1
    return answer


def write_case(
    directory: Path,
    index: int,
    values: list[int],
    commands: list[str],
    answers: list[int],
) -> None:
    input_text = (
        f"{len(values)} {len(commands)}\n"
        + " ".join(map(str, values))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    output_text = "\n".join(map(str, answers)) + "\n"
    (directory / f"case_{index:02d}.in").write_text(input_text, encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output_text, encoding="utf-8")


def make_random_case(rng: random.Random) -> tuple[list[int], list[str], list[int]]:
    vertex_count = 41
    initial_values = [
        rng.randrange(-10**9, 10**9 + 1) for _ in range(vertex_count)
    ]
    values = initial_values.copy()
    adjacency = [set() for _ in range(vertex_count)]
    commands: list[str] = []
    answers: list[int] = []
    operation_types = ["A", "A", "E", "E", "Q", "S", "Z", "G", "N"]
    for _ in range(8000):
        operation = rng.choice(operation_types)
        if operation == "A":
            vertex = rng.randrange(vertex_count)
            delta = rng.randrange(-10**9, 10**9 + 1)
            values[vertex] += delta
            commands.append(f"A {vertex} {delta}")
        elif operation in {"E", "S"}:
            first = rng.randrange(vertex_count)
            second = first if rng.randrange(8) == 0 else rng.randrange(vertex_count)
            reached = component(first, adjacency)
            if operation == "E":
                answers.append(int(second not in reached))
                adjacency[first].add(second)
                adjacency[second].add(first)
            else:
                answers.append(int(second in reached))
            commands.append(f"{operation} {first} {second}")
        elif operation == "Q":
            vertex = rng.randrange(vertex_count)
            commands.append(f"Q {vertex}")
            answers.append(sum(values[v] for v in component(vertex, adjacency)))
        elif operation == "Z":
            vertex = rng.randrange(vertex_count)
            commands.append(f"Z {vertex}")
            answers.append(len(component(vertex, adjacency)))
        elif operation == "G":
            commands.append("G")
            answers.append(component_count(adjacency))
        else:
            commands.append("N")
            answers.append(vertex_count)
    return initial_values, commands, answers


def make_large_case() -> tuple[list[int], list[str], list[int]]:
    vertex_count = 100_000
    initial_values = [(index % 101) - 50 for index in range(vertex_count)]
    values = initial_values.copy()
    commands: list[str] = []
    answers: list[int] = []
    for vertex in range(vertex_count - 1):
        commands.append(f"E {vertex} {vertex + 1}")
        answers.append(1)
    for vertex in range(vertex_count):
        delta = (vertex % 17) - 8
        commands.append(f"A {vertex} {delta}")
        values[vertex] += delta
    total = sum(values)
    for vertex in range(0, vertex_count, 89):
        commands.append(f"Q {vertex}")
        answers.append(total)
    commands.extend([
        "E 0 99999",
        "E 12345 12345",
        "S 0 99999",
        "Z 77777",
        "G",
        "N",
    ])
    answers.extend([0, 0, 1, vertex_count, 1, vertex_count])
    return initial_values, commands, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    write_case(output_directory, 0, *make_random_case(random.Random(2026071602)))
    write_case(output_directory, 1, *make_large_case())


if __name__ == "__main__":
    main()

