#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MODULUS = 998_244_353


def brute_weighted(adjacency: list[list[int]], modulus: int) -> int:
    vertex_count = len(adjacency)
    answer = 0
    for root in range(vertex_count):
        candidates = [
            [
                parent
                for parent in range(vertex_count)
                if parent != vertex and adjacency[parent][vertex] != 0
            ]
            for vertex in range(vertex_count)
        ]
        parents = [-1] * vertex_count
        parents[root] = root

        def enumerate_parents(vertex: int, product: int) -> None:
            nonlocal answer
            if vertex == vertex_count:
                for start in range(vertex_count):
                    current = start
                    seen: set[int] = set()
                    while current != root and current != -1 and current not in seen:
                        seen.add(current)
                        current = parents[current]
                    if current != root:
                        return
                answer = (answer + product) % modulus
                return
            if vertex == root:
                enumerate_parents(vertex + 1, product)
                return
            for parent in candidates[vertex]:
                parents[vertex] = parent
                enumerate_parents(
                    vertex + 1,
                    product * adjacency[parent][vertex] % modulus,
                )
            parents[vertex] = -1

        enumerate_parents(0, 1)
    return answer


def binary_weights(adjacency: list[str]) -> list[list[int]]:
    return [[int(edge) for edge in row] for row in adjacency]


def write_case(out_dir: Path, index: int, data: str, answer: int) -> None:
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text(data, encoding="utf-8")
    (out_dir / f"{name}.out").write_text(f"{answer}\n", encoding="utf-8")


def binary_input(adjacency: list[str]) -> str:
    return f"BINARY\n{len(adjacency)}\n" + "\n".join(adjacency) + "\n"


def weighted_input(adjacency: list[list[int]], mode: str = "WEIGHTED") -> str:
    rows = [" ".join(map(str, row)) for row in adjacency]
    return f"{mode}\n{len(adjacency)}\n" + "\n".join(rows) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    index = 0
    binary_cases = [
        ["0"],
        ["1"],
        ["0100", "0010", "0001", "0000"],
        ["0100", "0010", "0001", "1000"],
        ["000", "000", "000"],
        ["01111", "10111", "11011", "11101", "11110"],
    ]
    random_source = random.Random(20260717)
    for _ in range(50):
        vertex_count = random_source.randrange(1, 7)
        binary_cases.append(
            [
                "".join(
                    "1" if random_source.randrange(4) == 0 else "0"
                    for _ in range(vertex_count)
                )
                for _ in range(vertex_count)
            ]
        )
    for adjacency in binary_cases:
        write_case(
            args.out_dir,
            index,
            binary_input(adjacency),
            brute_weighted(binary_weights(adjacency), MODULUS),
        )
        index += 1

    weighted_cases = [
        [[0]],
        [[123456789]],
        [[0, 2], [3, 0]],
        [[9, -2, 0], [5, 11, 3], [4, 0, -7]],
        [[0, MODULUS + 1], [2 * MODULUS + 2, 0]],
        [[0, -MODULUS - 1], [MODULUS + 2, 0]],
    ]
    for _ in range(90):
        vertex_count = random_source.randrange(1, 6)
        weighted_cases.append(
            [
                [
                    0
                    if random_source.randrange(4) == 0
                    else random_source.randrange(-7, 8)
                    for _ in range(vertex_count)
                ]
                for _ in range(vertex_count)
            ]
        )
    for adjacency in weighted_cases:
        write_case(
            args.out_dir,
            index,
            weighted_input(adjacency),
            brute_weighted(adjacency, MODULUS),
        )
        index += 1

    # MOD divides the vertex count.  The single-column rank-one update must
    # work without dividing by the vertex count.
    complete_five = [[1] * 5 for _ in range(5)]
    write_case(
        args.out_dir,
        index,
        weighted_input(complete_five, "WEIGHTED5"),
        brute_weighted(complete_five, 5),
    )
    index += 1

    # Arbitrary-size weighted APIs are not subject to the legacy N <= 8 limit.
    chain_size = 128
    chain = [[0] * chain_size for _ in range(chain_size)]
    expected_chain = 1
    for vertex in range(chain_size - 1):
        weight = vertex % 17 + 1
        chain[vertex][vertex + 1] = weight
        expected_chain = expected_chain * weight % MODULUS
    write_case(
        args.out_dir,
        index,
        weighted_input(chain),
        expected_chain,
    )
    index += 1

    complete_size = 32
    complete = [[1] * complete_size for _ in range(complete_size)]
    write_case(
        args.out_dir,
        index,
        weighted_input(complete),
        pow(complete_size, complete_size - 1, MODULUS),
    )
    index += 1

    complete_binary = ["1" * 8 for _ in range(8)]
    write_case(
        args.out_dir,
        index,
        binary_input(complete_binary),
        2_097_152,
    )
    index += 1

    # Empty input runs the C++ self-test, which also performs randomized naive
    # comparisons and invalid-input checks.
    name = f"case_{index:03d}"
    (args.out_dir / f"{name}.in").write_text("", encoding="utf-8")
    (args.out_dir / f"{name}.out").write_text("", encoding="utf-8")


if __name__ == "__main__":
    main()
