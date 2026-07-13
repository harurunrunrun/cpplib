#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import itertools
import random
from pathlib import Path


def cost(matrix, facilities):
    clients = len(matrix[0])
    return sum(min(matrix[facility][client] for facility in facilities) for client in range(clients))


def add(matrix, k):
    selected = []
    while len(selected) < k:
        candidates = [facility for facility in range(len(matrix)) if facility not in selected]
        chosen = min(candidates, key=lambda facility: (cost(matrix, selected + [facility]), facility))
        selected.append(chosen)
    return tuple(sorted(selected))


def drop(matrix, k):
    selected = tuple(range(len(matrix)))
    while len(selected) > k:
        selected = min(
            (tuple(value for value in selected if value != removed) for removed in selected),
            key=lambda facilities: (cost(matrix, facilities), facilities),
        )
    return selected


def teitz(matrix, initial):
    current = tuple(sorted(initial))
    for _ in range(100):
        candidates = []
        closed = [facility for facility in range(len(matrix)) if facility not in current]
        for removed in current:
            for added in closed:
                trial = tuple(sorted((set(current) - {removed}) | {added}))
                if cost(matrix, trial) < cost(matrix, current):
                    candidates.append(trial)
        if not candidates:
            break
        current = min(candidates, key=lambda facilities: (cost(matrix, facilities), facilities))
    return current


def line(name, matrix, facilities):
    return f"{name} {cost(matrix, facilities)} {len(facilities)} " + " ".join(map(str, facilities))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        ([[abs(f * 4 - c * 3) for c in range(7)] for f in range(6)], 2),
        ([[0, 1, 1, 4], [0, 1, 1, 4], [4, 1, 1, 0], [4, 1, 1, 0]], 2),
        ([[] for _ in range(5)], 3),
        ([[(f * 17 + c * 23 + f * c * 7) % 31 for c in range(9)] for f in range(8)], 3),
    ]
    rng = random.Random(202607131)
    for case_index in range(14):
        facility_count = rng.randint(2, 8)
        client_count = rng.randint(1, 9)
        if case_index == 0:
            k = 1
        elif case_index == 1:
            k = facility_count
        else:
            k = rng.randint(1, facility_count)
        matrix = [
            [rng.randint(0, 40) for _ in range(client_count)]
            for _ in range(facility_count)
        ]
        cases.append((matrix, k))
    for case_id, (matrix, k) in enumerate(cases):
        initial = tuple(range(k))
        added = add(matrix, k)
        dropped = drop(matrix, k)
        exchanged = teitz(matrix, initial)
        add_drop = teitz(matrix, added)
        optimum = min(itertools.combinations(range(len(matrix)), k), key=lambda facilities: (cost(matrix, facilities), facilities))
        expected = "\n".join([
            line("ADD", matrix, added),
            line("DROP", matrix, dropped),
            line("TEITZ", matrix, exchanged),
            line("ADD_DROP", matrix, add_drop),
            line("OPT", matrix, optimum),
        ]) + "\n"
        lines = [f"{len(matrix)} {len(matrix[0])} {k}"]
        lines.extend(" ".join(map(str, row)) for row in matrix)
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
