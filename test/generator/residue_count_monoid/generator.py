#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Operation = tuple[str, int, int, int]


def normalized(value: int, modulus: int) -> int:
    return value % modulus


def solve_naive(
    values: list[int],
    modulus: int,
    operations: list[Operation],
) -> list[list[int]]:
    current = [normalized(value, modulus) for value in values]
    answers: list[list[int]] = []
    for operation_type, left, right, delta in operations:
        if operation_type == "A":
            shift = normalized(delta, modulus)
            for index in range(left, right):
                current[index] = (current[index] + shift) % modulus
        else:
            count = [0] * modulus
            for value in current[left:right]:
                count[value] += 1
            answers.append(count)
    return answers


class ReferenceTree:
    def __init__(self, values: list[int], modulus: int) -> None:
        self.modulus = modulus
        self.size = 1
        while self.size < len(values):
            self.size *= 2
        self.count = [[0] * modulus for _ in range(2 * self.size)]
        self.lazy = [0] * self.size
        for index, value in enumerate(values):
            self.count[self.size + index][value % modulus] = 1
        for node in range(self.size - 1, 0, -1):
            self._pull(node)

    def _pull(self, node: int) -> None:
        self.count[node] = [
            self.count[2 * node][residue]
            + self.count[2 * node + 1][residue]
            for residue in range(self.modulus)
        ]

    def _apply(self, node: int, shift: int) -> None:
        shift %= self.modulus
        if shift:
            previous = self.count[node]
            self.count[node] = [
                previous[(residue - shift) % self.modulus]
                for residue in range(self.modulus)
            ]
            if node < self.size:
                self.lazy[node] = (self.lazy[node] + shift) % self.modulus

    def _push(self, node: int) -> None:
        if self.lazy[node]:
            self._apply(2 * node, self.lazy[node])
            self._apply(2 * node + 1, self.lazy[node])
            self.lazy[node] = 0

    def add(self, left: int, right: int, delta: int) -> None:
        def visit(node: int, node_left: int, node_right: int) -> None:
            if node_right <= left or right <= node_left:
                return
            if left <= node_left and node_right <= right:
                self._apply(node, delta)
                return
            self._push(node)
            middle = (node_left + node_right) // 2
            visit(2 * node, node_left, middle)
            visit(2 * node + 1, middle, node_right)
            self._pull(node)

        visit(1, 0, self.size)

    def query(self, left: int, right: int) -> list[int]:
        def visit(node: int, node_left: int, node_right: int) -> list[int]:
            if node_right <= left or right <= node_left:
                return [0] * self.modulus
            if left <= node_left and node_right <= right:
                return self.count[node][:]
            self._push(node)
            middle = (node_left + node_right) // 2
            first = visit(2 * node, node_left, middle)
            second = visit(2 * node + 1, middle, node_right)
            return [
                first[residue] + second[residue]
                for residue in range(self.modulus)
            ]

        return visit(1, 0, self.size)


def write_case(
    output_directory: Path,
    index: int,
    count_type: str,
    modulus: int,
    values: list[int],
    operations: list[Operation],
    expected: list[list[int]],
) -> None:
    name = f"case_{index:02d}"
    input_lines = [
        f"{count_type} {modulus}",
        f"{len(values)} {len(operations)}",
        " ".join(map(str, values)),
    ]
    for operation_type, left, right, delta in operations:
        if operation_type == "A":
            input_lines.append(f"A {left} {right} {delta}")
        else:
            input_lines.append(f"Q {left} {right}")
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "".join(" ".join(map(str, answer)) + "\n" for answer in expected),
        encoding="utf-8",
    )


def random_operations(
    rng: random.Random,
    size: int,
    count: int,
) -> list[Operation]:
    result: list[Operation] = []
    special_delta = [
        -(1 << 63),
        (1 << 63) - 1,
        -10**18,
        10**18,
        -1,
        0,
        1,
    ]
    for index in range(count):
        left = rng.randrange(size + 1)
        right = rng.randrange(left, size + 1)
        if index % 3 != 2:
            delta = (
                special_delta[index % len(special_delta)]
                if index % 11 == 0
                else rng.randrange(-10**18, 10**18 + 1)
            )
            result.append(("A", left, right, delta))
        else:
            result.append(("Q", left, right, 0))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output_directory = Path(arguments.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    manual: list[Operation] = [
        ("Q", 0, 7, 0),
        ("A", 0, 7, -1),
        ("Q", 0, 7, 0),
        ("A", 1, 6, (1 << 63) - 1),
        ("Q", 1, 6, 0),
        ("A", 2, 2, -(1 << 63)),
        ("A", 0, 4, -(1 << 63)),
        ("Q", 0, 7, 0),
        ("Q", 3, 3, 0),
    ]
    values = [-(1 << 63), -8, -1, 0, 1, 8, (1 << 63) - 1]
    write_case(
        output_directory,
        0,
        "I32",
        3,
        values,
        manual,
        solve_naive(values, 3, manual),
    )

    rng = random.Random(20260714)
    case_index = 1
    for modulus in [1, 2, 3, 5, 7]:
        for count_type in ["I32", "I64"]:
            size = rng.randrange(1, 81)
            current = [rng.randrange(-(1 << 63), 1 << 63) for _ in range(size)]
            operations = random_operations(rng, size, 1200)
            write_case(
                output_directory,
                case_index,
                count_type,
                modulus,
                current,
                operations,
                solve_naive(current, modulus, operations),
            )
            case_index += 1

    size = 100000
    operation_count = 100000
    maximum_values = [
        ((index * 1000003 + 97) % 2000000011) - 1000000005
        for index in range(size)
    ]
    tree = ReferenceTree(maximum_values, 3)
    maximum_operations: list[Operation] = []
    maximum_expected: list[list[int]] = []
    for index in range(operation_count):
        left = (index * 1009) % size
        width = (index * 9176 + 1) % (size - left + 1)
        right = left + width
        if index % 2 == 0:
            delta = [
                -(1 << 63),
                (1 << 63) - 1,
                -1000000000000000000,
                1000000000000000000,
                index,
            ][index % 5]
            maximum_operations.append(("A", left, right, delta))
            tree.add(left, right, delta)
        else:
            maximum_operations.append(("Q", left, right, 0))
            maximum_expected.append(tree.query(left, right))
    write_case(
        output_directory,
        case_index,
        "I32",
        3,
        maximum_values,
        maximum_operations,
        maximum_expected,
    )


if __name__ == "__main__":
    main()
