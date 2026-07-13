#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def assignment(distance: list[list[int]], medoids: list[int]):
    labels: list[int] = []
    cost = 0
    for point in range(len(distance)):
        position = min(range(len(medoids)),
                       key=lambda pos: (distance[point][medoids[pos]], medoids[pos]))
        labels.append(position)
        cost += distance[point][medoids[position]]
    return labels, cost


def pam(distance: list[list[int]], k: int, limit: int):
    n = len(distance)
    medoids: list[int] = []
    selected = [False] * n
    while len(medoids) < k:
        candidates = [(assignment(distance, medoids + [candidate])[1], candidate)
                      for candidate in range(n) if not selected[candidate]]
        candidate = min(candidates)[1]
        medoids.append(candidate)
        selected[candidate] = True
    current = assignment(distance, medoids)[1]
    iterations = 0
    while iterations < limit:
        best_cost = current
        best_position: int | None = None
        best_candidate: int | None = None
        for position in range(k):
            for candidate in range(n):
                if selected[candidate]:
                    continue
                trial = medoids.copy()
                trial[position] = candidate
                cost = assignment(distance, trial)[1]
                tie = (best_position is not None and cost == best_cost < current and
                       (medoids[position], candidate) <
                       (medoids[best_position], best_candidate))
                if cost < best_cost or tie:
                    best_cost = cost
                    best_position = position
                    best_candidate = candidate
        if best_position is None:
            break
        selected[medoids[best_position]] = False
        medoids[best_position] = best_candidate
        selected[best_candidate] = True
        current = best_cost
        iterations += 1
    medoids.sort()
    labels, cost = assignment(distance, medoids)
    return medoids, labels, cost, iterations


def write_case(directory: Path, index: int, points: list[list[int]],
               k: int, limit: int) -> None:
    distance = [[sum(abs(a - b) for a, b in zip(left, right))
                 for right in points] for left in points]
    medoids, labels, cost, iterations = pam(distance, k, limit)
    data = f"{len(points)} {k} {limit}\n"
    data += "".join(" ".join(map(str, row)) + "\n" for row in distance)
    output = f"{iterations} {cost:.6f}\n"
    output += " ".join(map(str, medoids)) + "\n"
    output += " ".join(map(str, labels)) + "\n"
    (directory / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [[0], [1], [10], [11]], 2, 20)
    write_case(args.out_dir, 1, [[0, 0], [0, 0], [3, 4]], 3, 10)
    rng = random.Random(2026071308)
    for index in range(2, 14):
        dimension = rng.randint(1, 3)
        count = rng.randint(2, 18)
        points = [[rng.randint(-20, 20) for _ in range(dimension)]
                  for _ in range(count)]
        write_case(args.out_dir, index, points, rng.randint(1, count), 30)


if __name__ == "__main__":
    main()
