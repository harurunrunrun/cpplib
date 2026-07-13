#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from dataclasses import dataclass
from pathlib import Path


Operation = tuple[str, int, int | None]


@dataclass
class Case:
    colors: list[bool]
    values: list[int]
    edges: list[tuple[int, int]]
    operations: list[Operation]


def brute_output(case: Case) -> list[str]:
    n = len(case.colors)
    graph: list[list[int]] = [[] for _ in range(n)]
    for left, right in case.edges:
        graph[left].append(right)
        graph[right].append(left)
    colors = case.colors.copy()
    values = case.values.copy()
    output: list[str] = []
    for operation, vertex, argument in case.operations:
        if operation == "T":
            colors[vertex] = not colors[vertex]
        elif operation == "C":
            assert argument is not None
            colors[vertex] = argument != 0
        elif operation == "V":
            assert argument is not None
            values[vertex] = argument
        elif operation == "G":
            output.append(f"{int(colors[vertex])} {values[vertex]}")
        else:
            stack = [vertex]
            visited = {vertex}
            component: list[int] = []
            while stack:
                current = stack.pop()
                component.append(current)
                for to in graph[current]:
                    if to not in visited and colors[to] == colors[current]:
                        visited.add(to)
                        stack.append(to)
            output.append(
                f"{max(values[member] for member in component)} {len(component)}"
            )
    return output


def encode_cases(cases: list[Case]) -> tuple[list[str], list[str]]:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        n = len(case.colors)
        input_lines.append(f"{n} {len(case.operations)}")
        input_lines.append(" ".join(str(int(color)) for color in case.colors))
        input_lines.append(" ".join(map(str, case.values)))
        input_lines.extend(f"{left} {right}" for left, right in case.edges)
        for operation, vertex, argument in case.operations:
            if argument is None:
                input_lines.append(f"{operation} {vertex}")
            else:
                input_lines.append(f"{operation} {vertex} {argument}")
        output_lines.extend(brute_output(case))
    return input_lines, output_lines


def small_cases() -> list[Case]:
    result = [
        Case(
            [False],
            [-9],
            [],
            [
                ("Q", 0, None),
                ("T", 0, None),
                ("G", 0, None),
                ("V", 0, -20),
                ("Q", 0, None),
                ("C", 0, 0),
                ("Q", 0, None),
            ],
        ),
        Case(
            [False, False, True, True, False, True],
            [-8, -4, -30, -2, -11, -7],
            [(0, 1), (1, 2), (1, 3), (3, 4), (3, 5)],
            [
                ("Q", 0, None),
                ("Q", 3, None),
                ("T", 1, None),
                ("Q", 2, None),
                ("V", 5, 100),
                ("Q", 1, None),
                ("C", 4, 1),
                ("Q", 4, None),
                ("G", 5, None),
            ],
        ),
    ]

    rng = random.Random(20260714)
    operation_names = ("Q", "Q", "Q", "T", "C", "V", "G")
    for _ in range(70):
        n = rng.randint(2, 36)
        colors = [bool(rng.getrandbits(1)) for _ in range(n)]
        values = [rng.randint(-10**9, 10**9) for _ in range(n)]
        edges = [(vertex, rng.randrange(vertex)) for vertex in range(1, n)]
        rng.shuffle(edges)
        operations: list[Operation] = []
        for _ in range(rng.randint(70, 180)):
            operation = rng.choice(operation_names)
            vertex = rng.randrange(n)
            if operation == "C":
                operations.append((operation, vertex, rng.randrange(2)))
            elif operation == "V":
                operations.append(
                    (operation, vertex, rng.randint(-10**9, 10**9))
                )
            else:
                operations.append((operation, vertex, None))
        operations.extend(("Q", vertex, None) for vertex in range(n))
        result.append(Case(colors, values, edges, operations))
    return result


def star_case(n: int = 100_000) -> tuple[list[str], list[str]]:
    colors = [False] * n
    values = [-(vertex + 1) for vertex in range(n)]
    operations: list[str] = []
    output: list[str] = []
    leaf_one_count = 0
    center_color = False
    center_value = values[0]

    def query(vertex: int) -> None:
        operations.append(f"Q {vertex}")
        if vertex == 0 or colors[vertex] == center_color:
            component_size = 1 + (
                leaf_one_count if center_color else n - 1 - leaf_one_count
            )
            output.append(f"{center_value} {component_size}")
        else:
            output.append(f"{values[vertex]} 1")

    query(0)
    center_value = 10**15
    values[0] = center_value
    operations.append(f"V 0 {center_value}")
    step = 0
    while len(operations) < 99_996:
        leaf = 1 + step % (n - 1)
        operations.append(f"T {leaf}")
        colors[leaf] = not colors[leaf]
        leaf_one_count += 1 if colors[leaf] else -1
        if step % 997 == 0 and len(operations) < 99_996:
            query(0)
        if step % 1999 == 0 and len(operations) < 99_996:
            query(leaf)
        if step % 10007 == 0 and len(operations) < 99_996:
            operations.append("T 0")
            center_color = not center_color
            colors[0] = center_color
            query(0)
        step += 1
    query(0)

    input_lines = ["1", f"{n} {len(operations)}"]
    input_lines.append(" ".join("1" if color else "0" for color in [False] * n))
    input_lines.append(" ".join(str(-(vertex + 1)) for vertex in range(n)))
    input_lines.extend(f"0 {vertex}" for vertex in range(1, n))
    input_lines.extend(operations)
    return input_lines, output


def path_case(n: int = 100_000) -> tuple[list[str], list[str]]:
    values = [vertex - n for vertex in range(n)]
    operations: list[str] = []
    output: list[str] = []
    prefix_maximum = values[0]
    toggled = 0

    operations.append("Q 0")
    output.append(f"-1 {n}")
    while len(operations) < 99_996 and toggled + 1 < n:
        vertex = toggled
        operations.append(f"T {vertex}")
        toggled += 1
        prefix_maximum = max(prefix_maximum, values[vertex])
        if vertex == 25_000 and len(operations) < 99_996:
            values[0] = 10**15
            prefix_maximum = values[0]
            operations.append(f"V 0 {values[0]}")
        if len(operations) < 99_996:
            if vertex % 2 == 0:
                operations.append("Q 0")
                output.append(f"{prefix_maximum} {toggled}")
            else:
                operations.append(f"Q {n - 1}")
                output.append(f"-1 {n - toggled}")
    operations.append("Q 0")
    output.append(f"{prefix_maximum} {toggled}")

    initial_values = [vertex - n for vertex in range(n)]
    input_lines = ["1", f"{n} {len(operations)}"]
    input_lines.append(" ".join("0" for _ in range(n)))
    input_lines.append(" ".join(map(str, initial_values)))
    input_lines.extend(f"{vertex - 1} {vertex}" for vertex in range(1, n))
    input_lines.extend(operations)
    return input_lines, output


def write_case(
    out_dir: Path,
    stem: str,
    input_lines: list[str],
    output_lines: list[str],
) -> None:
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    small_input, small_output = encode_cases(small_cases())
    write_case(out_dir, "case_00", small_input, small_output)
    write_case(out_dir, "case_01", *star_case())
    write_case(out_dir, "case_02", *path_case())


if __name__ == "__main__":
    main()
