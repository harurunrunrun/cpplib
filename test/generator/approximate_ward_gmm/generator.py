#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from fractions import Fraction
from pathlib import Path


def ward_merges(points: list[list[int]]) -> list[tuple[int, int, int]]:
    n = len(points)
    centroids = [[Fraction(value) for value in point] for point in points]
    sizes = [1] * n
    active = list(range(n))
    result: list[tuple[int, int, int]] = []
    for step in range(n - 1):
        candidates = []
        for left_index, left in enumerate(active):
            for right in active[left_index + 1:]:
                distance_squared = sum(
                    (centroids[left][coordinate] - centroids[right][coordinate]) ** 2
                    for coordinate in range(len(points[0]))
                )
                cost = Fraction(sizes[left] * sizes[right], sizes[left] + sizes[right]) * distance_squared
                candidates.append((cost, left, right))
        _, left, right = min(candidates)
        merged_size = sizes[left] + sizes[right]
        merged_centroid = [
            (centroids[left][coordinate] * sizes[left]
             + centroids[right][coordinate] * sizes[right]) / merged_size
            for coordinate in range(len(points[0]))
        ]
        merged = n + step
        centroids.append(merged_centroid)
        sizes.append(merged_size)
        active = [node for node in active if node != left and node != right]
        active.append(merged)
        active.sort()
        result.append((left, right, merged_size))
    return result


def write_case(directory: Path, index: int, points: list[list[int]], components: int,
               seed: int) -> None:
    lines = [f"{len(points)} {len(points[0])} {components} {seed}"]
    lines.extend(" ".join(map(str, point)) for point in points)
    merges = ward_merges(points)
    output = str(len(merges)) + "".join(
        f" {left} {right} {size}" for left, right, size in merges
    ) + "\n"
    (directory / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [[0]], 1, 1)
    write_case(args.out_dir, 1, [[-10], [-9], [10], [11]], 2, 2)
    write_case(args.out_dir, 2, [[0, 0], [0, 1], [9, 9], [10, 9], [20, -3]], 3, 3)
    rng = random.Random(2026071315)
    for index in range(3, 21):
        dimension = rng.randint(1, 3)
        count = rng.randint(2, 11)
        points = [[rng.randint(-15, 15) for _ in range(dimension)] for _ in range(count)]
        write_case(args.out_dir, index, points, rng.randint(1, min(3, count)), rng.getrandbits(64))


if __name__ == "__main__":
    main()
