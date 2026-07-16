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
    initial_values: list[int],
    commands: list[str],
    answers: list[int],
) -> None:
    (directory / f"case_{index:02d}.in").write_text(
        f"{len(initial_values)} {len(commands)}\n"
        + " ".join(map(str, initial_values))
        + "\n"
        + "\n".join(commands)
        + "\n",
        encoding="utf-8",
    )
    (directory / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="utf-8",
    )


def make_random_case(rng: random.Random):
    vertex_count = 37
    initial_values = [rng.randrange(-100, 101) for _ in range(vertex_count)]
    values = initial_values.copy()
    adjacency = [set() for _ in range(vertex_count)]
    successful_merges = 0
    commands: list[str] = []
    answers: list[int] = []
    operations = ["E", "E", "A", "A", "V", "Q", "S", "Z", "G", "H", "N"]
    for _ in range(7000):
        operation = rng.choice(operations)
        if operation == "E":
            first = rng.randrange(vertex_count)
            second = first if rng.randrange(8) == 0 else rng.randrange(vertex_count)
            reached = component(first, adjacency)
            merged = second not in reached
            answers.append(int(merged))
            if merged:
                successful_merges += 1
            adjacency[first].add(second)
            adjacency[second].add(first)
            commands.append(f"E {first} {second}")
        elif operation == "A":
            vertex = rng.randrange(vertex_count)
            multiplier = rng.choice([-1, 0, 1])
            addend = rng.randrange(-20, 21)
            commands.append(f"A {vertex} {multiplier} {addend}")
            for reached_vertex in component(vertex, adjacency):
                values[reached_vertex] = (
                    multiplier * values[reached_vertex] + addend
                )
        elif operation == "V":
            vertex = rng.randrange(vertex_count)
            commands.append(f"V {vertex}")
            answers.append(values[vertex])
        elif operation == "Q":
            vertex = rng.randrange(vertex_count)
            commands.append(f"Q {vertex}")
            answers.append(sum(values[v] for v in component(vertex, adjacency)))
        elif operation == "S":
            first = rng.randrange(vertex_count)
            second = rng.randrange(vertex_count)
            commands.append(f"S {first} {second}")
            answers.append(int(second in component(first, adjacency)))
        elif operation == "Z":
            vertex = rng.randrange(vertex_count)
            commands.append(f"Z {vertex}")
            answers.append(len(component(vertex, adjacency)))
        elif operation == "G":
            commands.append("G")
            answers.append(component_count(adjacency))
        elif operation == "H":
            commands.append("H")
            answers.append(vertex_count + successful_merges)
        else:
            commands.append("N")
            answers.append(vertex_count)
    return initial_values, commands, answers


def make_large_case():
    vertex_count = 30_000
    initial_values = [(vertex % 101) - 50 for vertex in range(vertex_count)]
    values = initial_values.copy()
    commands: list[str] = []
    answers: list[int] = []

    for vertex in range(0, vertex_count, 7):
        multiplier = -1 if vertex % 2 else 1
        addend = (vertex % 13) - 6
        commands.append(f"A {vertex} {multiplier} {addend}")
        values[vertex] = multiplier * values[vertex] + addend

    for vertex in range(vertex_count - 1):
        commands.append(f"E {vertex} {vertex + 1}")
        answers.append(1)

    for multiplier, addend in [(-1, 3), (1, -8), (0, 11), (-1, 2)]:
        commands.append(f"A 0 {multiplier} {addend}")
        values = [multiplier * value + addend for value in values]

    for vertex in range(0, vertex_count, 29):
        commands.append(f"V {vertex}")
        answers.append(values[vertex])
    commands.extend(
        [
            "Q 12345",
            "E 0 29999",
            "E 7777 7777",
            "S 0 29999",
            "Z 22222",
            "G",
            "H",
            "N",
        ]
    )
    answers.extend(
        [
            sum(values),
            0,
            0,
            1,
            vertex_count,
            1,
            2 * vertex_count - 1,
            vertex_count,
        ]
    )
    return initial_values, commands, answers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    write_case(directory, 0, *make_random_case(random.Random(2026071606)))
    write_case(directory, 1, *make_large_case())


if __name__ == "__main__":
    main()
