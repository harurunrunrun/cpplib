#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Matrix = list[list[int]]


def irreducible_pairs(
    distances: Matrix,
) -> list[tuple[int, int]] | None:
    vertex_count = len(distances)
    if any(len(row) != vertex_count for row in distances):
        return None
    for vertex in range(vertex_count):
        if distances[vertex][vertex] != 0:
            return None
        for other in range(vertex_count):
            if (
                distances[vertex][other] < 0
                or distances[vertex][other] != distances[other][vertex]
            ):
                return None

    result: list[tuple[int, int]] = []
    for left in range(vertex_count):
        for right in range(left + 1, vertex_count):
            direct = distances[left][right]
            irreducible = True
            for middle in range(vertex_count):
                if middle in (left, right):
                    continue
                through = (
                    distances[left][middle]
                    + distances[middle][right]
                )
                if through < direct:
                    return None
                if through == direct:
                    irreducible = False
            if irreducible:
                result.append((left, right))
    return result


def random_metric(source: random.Random, vertex_count: int) -> Matrix:
    infinity = 10**18
    distances = [
        [0 if left == right else infinity for right in range(vertex_count)]
        for left in range(vertex_count)
    ]
    for vertex in range(1, vertex_count):
        parent = source.randrange(vertex)
        weight = source.randint(1, 40)
        distances[vertex][parent] = weight
        distances[parent][vertex] = weight
    for left in range(vertex_count):
        for right in range(left + 1, vertex_count):
            if source.random() < 0.25:
                weight = source.randint(1, 60)
                distances[left][right] = min(distances[left][right], weight)
                distances[right][left] = distances[left][right]
    for middle in range(vertex_count):
        for left in range(vertex_count):
            for right in range(vertex_count):
                distances[left][right] = min(
                    distances[left][right],
                    distances[left][middle] + distances[middle][right],
                )
    return distances


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Matrix] = [
        [],
        [[0]],
        [[0, 4], [4, 0]],
        [[0, 2, 5], [2, 0, 3], [5, 3, 0]],
        [[1]],
        [[0, 1], [2, 0]],
        [[0, 1, 10], [1, 0, 1], [10, 1, 0]],
    ]
    source = random.Random(41004)
    for _ in range(140):
        cases.append(random_metric(source, source.randint(1, 12)))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for distances in cases:
        input_lines.append(str(len(distances)))
        input_lines.extend(" ".join(map(str, row)) for row in distances)
        pairs = irreducible_pairs(distances)
        if pairs is None:
            output_lines.append("INVALID")
        else:
            suffix = "".join(
                f" {left}:{right}" for left, right in pairs
            )
            output_lines.append(f"{len(pairs)}{suffix}")

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
