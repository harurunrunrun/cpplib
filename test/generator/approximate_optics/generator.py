#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import math
import random
from pathlib import Path


def optics_order(points: list[list[int]], radius: int, minimum_points: int) -> list[int]:
    n = len(points)
    distance = [[0.0] * n for _ in range(n)]
    for left in range(n):
        for right in range(left + 1, n):
            value = math.dist(points[left], points[right])
            distance[left][right] = distance[right][left] = value
    core = [math.inf] * n
    for point in range(n):
        neighbors = sorted(value for value in distance[point] if value <= radius)
        if len(neighbors) >= minimum_points:
            core[point] = neighbors[minimum_points - 1]
    reach = [math.inf] * n
    processed = [False] * n
    order: list[int] = []
    for start in range(n):
        if processed[start]:
            continue
        queue: list[tuple[float, int]] = []

        def process(point: int) -> None:
            processed[point] = True
            order.append(point)
            if not math.isfinite(core[point]):
                return
            for neighbor in range(n):
                if processed[neighbor] or distance[point][neighbor] > radius:
                    continue
                candidate = max(core[point], distance[point][neighbor])
                if candidate < reach[neighbor]:
                    reach[neighbor] = candidate
                    heapq.heappush(queue, (candidate, neighbor))

        process(start)
        while queue:
            value, point = heapq.heappop(queue)
            if processed[point] or value != reach[point]:
                continue
            process(point)
    return order


def write_case(directory: Path, index: int, points: list[list[int]], radius: int,
               minimum_points: int) -> None:
    dimension = len(points[0]) if points else 1
    lines = [f"{len(points)} {dimension} {radius} {minimum_points}"]
    lines.extend(" ".join(map(str, point)) for point in points)
    order = optics_order(points, radius, minimum_points)
    output = str(len(order)) + "".join(f" {point}" for point in order) + "\n"
    (directory / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [], 2, 1)
    write_case(args.out_dir, 1, [[0], [1], [2], [20], [21], [100]], 2, 2)
    write_case(args.out_dir, 2, [[0, 0], [0, 0], [3, 4], [3, 5]], 0, 2)
    rng = random.Random(2026071314)
    for index in range(3, 23):
        dimension = rng.randint(1, 4)
        count = rng.randint(1, 55)
        points = [[rng.randint(-20, 20) for _ in range(dimension)] for _ in range(count)]
        write_case(args.out_dir, index, points, rng.randint(0, 12), rng.randint(1, 8))


if __name__ == "__main__":
    main()
