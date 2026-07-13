#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def dbscan(points: list[list[int]], epsilon: int, minimum_points: int):
    radius_squared = epsilon * epsilon
    neighbors = [[] for _ in points]
    for left in range(len(points)):
        for right in range(left, len(points)):
            distance = sum((a - b) ** 2 for a, b in zip(points[left], points[right]))
            if distance <= radius_squared:
                neighbors[left].append(right)
                if left != right:
                    neighbors[right].append(left)
    for row in neighbors:
        row.sort()
    labels = [-2] * len(points)
    cluster_count = 0
    for start in range(len(points)):
        if labels[start] != -2:
            continue
        if len(neighbors[start]) < minimum_points:
            labels[start] = -1
            continue
        cluster = cluster_count
        cluster_count += 1
        labels[start] = cluster
        queue = deque(neighbors[start])
        queued = [False] * len(points)
        for point in neighbors[start]:
            queued[point] = True
        while queue:
            point = queue.popleft()
            if labels[point] == -1:
                labels[point] = cluster
            if labels[point] != -2:
                continue
            labels[point] = cluster
            if len(neighbors[point]) < minimum_points:
                continue
            for following in neighbors[point]:
                if not queued[following]:
                    queued[following] = True
                    queue.append(following)
    return labels, cluster_count


def write_case(directory: Path, index: int, points: list[list[int]],
               epsilon: int, minimum_points: int) -> None:
    labels, count = dbscan(points, epsilon, minimum_points)
    data = f"{len(points)} {len(points[0])} {epsilon} {minimum_points}\n"
    data += "".join(" ".join(map(str, point)) + "\n" for point in points)
    output = f"{count}\n" + " ".join(map(str, labels)) + "\n"
    (directory / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [[0], [1], [2], [20], [21], [100]], 2, 2)
    write_case(args.out_dir, 1, [[0, 0], [0, 0], [3, 4]], 0, 2)
    rng = random.Random(2026071309)
    for index in range(2, 16):
        dimension = rng.randint(1, 3)
        count = rng.randint(1, 55)
        points = [[rng.randint(-12, 12) for _ in range(dimension)]
                  for _ in range(count)]
        write_case(args.out_dir, index, points, rng.randint(0, 7),
                   rng.randint(1, min(count, 8)))


if __name__ == "__main__":
    main()
