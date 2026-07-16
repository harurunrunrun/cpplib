#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import math
import random
from pathlib import Path


Permutation = tuple[int, ...]


def compose(first: Permutation, second: Permutation) -> Permutation:
    return tuple(first[second[index]] for index in range(len(first)))


def group_order(degree: int, generators: list[Permutation]) -> int:
    identity = tuple(range(degree))
    reached = {identity}
    queue = [identity]
    for current in queue:
        for generator in generators:
            following = compose(generator, current)
            if following not in reached:
                reached.add(following)
                queue.append(following)
    return len(reached)


def cycle(degree: int) -> Permutation:
    return tuple((index + 1) % degree for index in range(degree))


def write_cases(directory: Path, cases: list[tuple[int, list[Permutation], int]]) -> None:
    lines = [str(len(cases))]
    output: list[str] = []
    for degree, generators, order in cases:
        lines.append(f"{degree} {len(generators)}")
        lines.extend(" ".join(map(str, generator)) for generator in generators)
        output.append(str(order))
    (directory / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / "case_00.out").write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071623)
    cases: list[tuple[int, list[Permutation], int]] = [(0, [], 1), (8, [], 1)]
    for degree in range(1, 9):
        permutations = list(itertools.permutations(range(degree)))
        for _ in range(12):
            generators = [rng.choice(permutations) for _ in range(rng.randrange(1, 5))]
            cases.append((degree, generators, group_order(degree, generators)))

    for degree in (20, 30, 50):
        transposition = list(range(degree))
        transposition[0], transposition[1] = transposition[1], transposition[0]
        cases.append((degree, [cycle(degree), tuple(transposition)], math.factorial(degree)))
        cases.append((degree, [cycle(degree)], degree))

    write_cases(directory, cases)


if __name__ == "__main__":
    main()

