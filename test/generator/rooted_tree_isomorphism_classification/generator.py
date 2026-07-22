#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def signatures(parents: list[int]) -> list[str]:
    children = [[] for _ in range(len(parents) + 1)]
    for vertex, parent in enumerate(parents, 1):
        children[parent].append(vertex)
    order = [0]
    for vertex in order:
        order.extend(children[vertex])
    result = [""] * len(children)
    for vertex in reversed(order):
        result[vertex] = "(" + "".join(sorted(result[to] for to in children[vertex])) + ")"
    return result


def permuted_parents(parents: list[int], rng: random.Random) -> list[int]:
    size = len(parents) + 1
    permutation = [0, *range(1, size)]
    tail = permutation[1:]
    rng.shuffle(tail)
    permutation[1:] = tail
    result = [-1] * len(parents)
    for vertex, parent in enumerate(parents, 1):
        result[permutation[vertex] - 1] = permutation[parent]
    return result


def multiplicity_parents(max_count: int, repetitions: int) -> list[int]:
    parents: list[int] = []
    for count in range(1, max_count + 1):
        for _ in range(repetitions):
            parent = len(parents) + 1
            parents.append(0)
            parents.extend([parent] * count)
    return parents


def write_case(out_dir: Path, index: int, parents: list[int]) -> None:
    canonical = signatures(parents)
    class_count = len(set(canonical))
    lines = [str(len(parents) + 1)]
    if parents:
        lines.append(" ".join(map(str, parents)))
    expected = [str(class_count)]
    expected.extend(
        "".join("1" if left == right else "0" for right in canonical)
        for left in canonical
    )
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(expected) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        [],
        [0],
        [0, 0],
        [0, 1, 2, 3],
        [0, 0, 1, 1, 2, 2],
        # Vertices 1 and 5 are the same two-leaf subtree at different depths.
        # A depth-local relative rank can incorrectly give them different ids.
        [0, 0, 1, 1, 2, 5, 5],
        multiplicity_parents(20, 2),
    ]
    for index, parents in enumerate(fixed):
        write_case(out_dir, index, parents)

    rng = random.Random(20260714)
    index = len(fixed)
    for _ in range(18):
        size = rng.randrange(1, 51)
        parents = [rng.randrange(vertex) for vertex in range(1, size)]
        write_case(out_dir, index, parents)
        index += 1
        write_case(out_dir, index, permuted_parents(parents, rng))
        index += 1


if __name__ == "__main__":
    main()
