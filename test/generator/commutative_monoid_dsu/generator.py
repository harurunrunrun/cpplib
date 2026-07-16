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
    vertex_count = 37
    values = [rng.randrange(-10**12, 10**12 + 1) for _ in range(vertex_count)]
    adjacency = [set() for _ in range(vertex_count)]
    commands: list[str] = []
    answers: list[int] = []
    operation_types = ["M", "M", "S", "Z", "P", "G", "N"]
    for _ in range(7000):
        operation = rng.choice(operation_types)
        if operation in {"M", "S"}:
            first = rng.randrange(vertex_count)
            second = first if rng.randrange(8) == 0 else rng.randrange(vertex_count)
            reached = component(first, adjacency)
            if operation == "M":
                answers.append(int(second not in reached))
                adjacency[first].add(second)
                adjacency[second].add(first)
            else:
                answers.append(int(second in reached))
            commands.append(f"{operation} {first} {second}")
        elif operation == "Z":
            vertex = rng.randrange(vertex_count)
            commands.append(f"Z {vertex}")
            answers.append(len(component(vertex, adjacency)))
        elif operation == "P":
            vertex = rng.randrange(vertex_count)
            commands.append(f"P {vertex}")
            answers.append(sum(values[v] for v in component(vertex, adjacency)))
        elif operation == "G":
            commands.append("G")
            answers.append(component_count(adjacency))
        else:
            commands.append("N")
            answers.append(vertex_count)
    return values, commands, answers


def make_large_case() -> tuple[list[int], list[str], list[int]]:
    vertex_count = 100_000
    values = [(index % 2001) - 1000 for index in range(vertex_count)]
    commands: list[str] = []
    answers: list[int] = []
    for vertex in range(vertex_count - 1):
        commands.append(f"M {vertex} {vertex + 1}")
        answers.append(1)
    total = sum(values)
    for vertex in range(0, vertex_count, 97):
        commands.append(f"P {vertex}")
        answers.append(total)
    commands.extend([
        "M 0 99999",
        "M 50000 50000",
        "S 0 99999",
        "Z 54321",
        "G",
        "N",
    ])
    answers.extend([0, 0, 1, vertex_count, 1, vertex_count])
    return values, commands, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    write_case(output_directory, 0, *make_random_case(random.Random(2026071601)))
    write_case(output_directory, 1, *make_large_case())


if __name__ == "__main__":
    main()

