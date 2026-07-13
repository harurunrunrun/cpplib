#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import itertools
import random
from pathlib import Path


def cost(matrix, opening, facilities):
    if not facilities:
        return 0
    return sum(opening[f] for f in facilities) + sum(
        min(matrix[f][client] for f in facilities) for client in range(len(matrix[0]))
    )


def greedy(matrix, opening):
    if not matrix[0]:
        return tuple()
    current = min(((f,) for f in range(len(matrix))), key=lambda fs: (cost(matrix, opening, fs), fs))
    while True:
        trials = [tuple(sorted(current + (f,))) for f in range(len(matrix)) if f not in current]
        improving = [fs for fs in trials if cost(matrix, opening, fs) < cost(matrix, opening, current)]
        if not improving:
            return current
        current = min(improving, key=lambda fs: (cost(matrix, opening, fs), fs))


def add_drop(matrix, opening):
    current = greedy(matrix, opening)
    for _ in range(100):
        trials = [tuple(sorted(current + (f,))) for f in range(len(matrix)) if f not in current]
        if len(current) > 1:
            trials.extend(tuple(f for f in current if f != removed) for removed in current)
        improving = [fs for fs in trials if cost(matrix, opening, fs) < cost(matrix, opening, current)]
        if not improving:
            break
        current = min(improving, key=lambda fs: (cost(matrix, opening, fs), fs))
    return current


def line(name, matrix, opening, facilities):
    suffix = "" if not facilities else " " + " ".join(map(str, facilities))
    return f"{name} {cost(matrix, opening, facilities)} {len(facilities)}{suffix}"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        ([[abs(f * 5 - c * 4) for c in range(8)] for f in range(6)], [9, 4, 8, 3, 7, 5]),
        ([[0, 2, 4], [0, 2, 4], [4, 2, 0], [4, 2, 0]], [2, 2, 2, 2]),
        ([[] for _ in range(4)], [5, 0, 3, 2]),
        ([[(f * 19 + c * 11 + f * c * 5) % 37 for c in range(10)] for f in range(7)], [3, 9, 4, 12, 6, 8, 5]),
    ]
    rng = random.Random(202607132)
    for _ in range(14):
        facility_count = rng.randint(2, 8)
        client_count = rng.randint(1, 9)
        matrix = [
            [rng.randint(0, 50) for _ in range(client_count)]
            for _ in range(facility_count)
        ]
        opening = [rng.randint(0, 20) for _ in range(facility_count)]
        cases.append((matrix, opening))
    for case_id, (matrix, opening) in enumerate(cases):
        greedy_result = greedy(matrix, opening)
        add_drop_result = add_drop(matrix, opening)
        if matrix[0]:
            subsets = itertools.chain.from_iterable(
                itertools.combinations(range(len(matrix)), size)
                for size in range(1, len(matrix) + 1)
            )
            optimum = min(subsets, key=lambda fs: (cost(matrix, opening, fs), fs))
        else:
            optimum = tuple()
        expected = "\n".join([
            line("GREEDY", matrix, opening, greedy_result),
            line("ADD_DROP", matrix, opening, add_drop_result),
            line("OPT", matrix, opening, optimum),
        ]) + "\n"
        lines = [f"{len(matrix)} {len(matrix[0])}", " ".join(map(str, opening))]
        lines.extend(" ".join(map(str, row)) for row in matrix)
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
