#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def expected_answers(size, operations):
    versions = []
    answers = []
    for operation_type, base_operation, left, right in operations:
        parent = (
            list(range(size))
            if base_operation == -1
            else versions[base_operation].copy()
        )

        def leader(vertex):
            while parent[vertex] != vertex:
                parent[vertex] = parent[parent[vertex]]
                vertex = parent[vertex]
            return vertex

        left_root = leader(left)
        right_root = leader(right)
        if operation_type == 0:
            if left_root != right_root:
                parent[right_root] = left_root
        else:
            answers.append(str(int(left_root == right_root)))
        versions.append(parent)
    return answers


def write_case(out_dir, index, size, operations):
    lines = [f"{size} {len(operations)}"]
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    answers = expected_answers(size, operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(answers) + ("\n" if answers else ""), encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        (0, []),
        (3, [(1, -1, 0, 0)]),
        (
            4,
            [
                (0, -1, 0, 1),
                (1, 0, 0, 1),
                (0, 1, 1, 2),
                (1, 1, 0, 2),
                (1, 2, 0, 2),
                (0, -1, 2, 3),
                (1, 5, 2, 3),
            ],
        ),
    ]
    rng = random.Random(7421921)
    for _ in range(35):
        size = rng.randrange(1, 33)
        operation_count = rng.randrange(1, 201)
        operations = []
        for operation in range(operation_count):
            operation_type = rng.randrange(2)
            base_operation = rng.randrange(-1, operation)
            left = rng.randrange(size)
            right = rng.randrange(size)
            operations.append(
                (operation_type, base_operation, left, right)
            )
        cases.append((size, operations))

    for index, (size, operations) in enumerate(cases):
        write_case(out_dir, index, size, operations)


if __name__ == "__main__":
    main()
