#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def assign(points: list[list[float]], centers: list[list[float]]) -> tuple[list[int], float]:
    labels: list[int] = []
    error = 0.0
    for point in points:
        distances = [sum((a - b) ** 2 for a, b in zip(point, center))
                     for center in centers]
        label = min(range(len(centers)), key=lambda index: (distances[index], index))
        labels.append(label)
        error += distances[label]
    return labels, error


def lloyd(points: list[list[float]], centers: list[list[float]], limit: int):
    previous: list[int] | None = None
    iterations = 0
    for iteration in range(limit):
        labels, _ = assign(points, centers)
        next_centers = [[0.0] * len(points[0]) for _ in centers]
        counts = [0] * len(centers)
        for point, label in zip(points, labels):
            counts[label] += 1
            for coordinate, value in enumerate(point):
                next_centers[label][coordinate] += value
        for cluster in range(len(centers)):
            if counts[cluster] == 0:
                next_centers[cluster] = centers[cluster].copy()
            else:
                next_centers[cluster] = [value / counts[cluster]
                                         for value in next_centers[cluster]]
        maximum_shift = max(sum((a - b) ** 2 for a, b in zip(old, new))
                            for old, new in zip(centers, next_centers))
        unchanged = labels == previous
        previous = labels
        centers = next_centers
        iterations = iteration + 1
        if unchanged or maximum_shift <= 0.0:
            break
    labels, error = assign(points, centers)
    return centers, labels, error, iterations


def write_case(directory: Path, index: int, points: list[list[int]],
               centers: list[list[int]], limit: int) -> None:
    result_centers, labels, error, iterations = lloyd(
        [[float(value) for value in point] for point in points],
        [[float(value) for value in center] for center in centers], limit
    )
    data = f"{len(points)} {len(points[0])} {len(centers)} {limit}\n"
    data += "".join(" ".join(map(str, point)) + "\n" for point in points)
    data += "".join(" ".join(map(str, center)) + "\n" for center in centers)
    output = f"{iterations} {error:.6f}\n"
    output += "".join(" ".join(f"{value:.6f}" for value in center) + "\n"
                      for center in result_centers)
    output += " ".join(map(str, labels)) + "\n"
    (directory / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [[0], [1], [9], [10]], [[0], [10]], 20)
    write_case(args.out_dir, 1, [[2, 2]] * 5, [[2, 2], [2, 2], [100, 100]], 10)
    write_case(args.out_dir, 2, [[-5, 2], [8, -1], [0, 0]], [[0, 0]], 0)
    rng = random.Random(2026071307)
    for index in range(3, 15):
        dimension = rng.randint(1, 4)
        count = rng.randint(4, 40)
        cluster_count = rng.randint(1, min(count, 7))
        points = [[rng.randint(-30, 30) for _ in range(dimension)]
                  for _ in range(count)]
        centers = [points[position].copy() for position in
                   rng.sample(range(count), cluster_count)]
        write_case(args.out_dir, index, points, centers, 40)


if __name__ == "__main__":
    main()
