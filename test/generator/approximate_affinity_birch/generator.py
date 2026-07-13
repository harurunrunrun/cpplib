#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from dataclasses import dataclass
from pathlib import Path


@dataclass
class Feature:
    count: int
    centroid: list[float]
    variance: list[float]


def distance(left: list[float], right: list[float]) -> float:
    return math.dist(left, right)


def append(feature: Feature, point: list[int]) -> Feature:
    count = feature.count + 1
    mean = []
    variance = []
    for old_mean, old_variance, value in zip(feature.centroid, feature.variance, point):
        new_mean = old_mean + (value - old_mean) / count
        new_variance = old_variance * (count - 1) / count
        new_variance += abs(value - old_mean) / count * abs(value - new_mean)
        mean.append(new_mean)
        variance.append(new_variance)
    return Feature(count, mean, variance)


def merge(left: Feature, right: Feature) -> Feature:
    count = left.count + right.count
    left_weight = left.count / count
    right_weight = right.count / count
    mean = []
    variance = []
    for lm, rm, lv, rv in zip(left.centroid, right.centroid, left.variance, right.variance):
        mean.append(lm + (rm - lm) * right_weight)
        variance.append(lv * left_weight + rv * right_weight
                        + left_weight * right_weight * (lm - rm) ** 2)
    return Feature(count, mean, variance)


def birch(points: list[list[int]], threshold: float, maximum_features: int):
    features: list[Feature] = []
    for point in points:
        if not features:
            features.append(Feature(1, list(map(float, point)), [0.0] * len(point)))
            continue
        nearest = min(range(len(features)), key=lambda index: (distance(point, features[index].centroid), index))
        candidate = append(features[nearest], point)
        if math.sqrt(sum(candidate.variance)) <= threshold:
            features[nearest] = candidate
        else:
            features.append(Feature(1, list(map(float, point)), [0.0] * len(point)))
        if len(features) > maximum_features:
            left, right = min(
                ((left, right) for left in range(len(features)) for right in range(left + 1, len(features))),
                key=lambda pair: (distance(features[pair[0]].centroid, features[pair[1]].centroid), pair),
            )
            features[left] = merge(features[left], features[right])
            del features[right]
    labels = [
        min(range(len(features)), key=lambda index: (distance(point, features[index].centroid), index))
        for point in points
    ]
    return features, labels


def write_case(directory: Path, index: int, points: list[list[int]], threshold: float,
               maximum_features: int) -> None:
    dimension = len(points[0]) if points else 1
    lines = [f"{len(points)} {dimension} {threshold} {maximum_features}"]
    lines.extend(" ".join(map(str, point)) for point in points)
    features, labels = birch(points, threshold, maximum_features) if points else ([], [])
    output = str(len(features))
    output += "".join(f" {feature.count}" for feature in features)
    output += "".join(f" {label}" for label in labels)
    (directory / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (directory / f"case_{index:02d}.out").write_text(output + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [], 1.0, 1)
    write_case(args.out_dir, 1, [[0]], 0.0, 1)
    write_case(args.out_dir, 2, [[0], [1], [2], [20], [21]], 1.0, 3)
    rng = random.Random(2026071316)
    thresholds = [0.25, 1.5, 4.5, 20.0]
    for index in range(3, 23):
        dimension = rng.randint(1, 3)
        count = rng.randint(2, 45)
        points = [[rng.randint(-20, 20) for _ in range(dimension)] for _ in range(count)]
        write_case(args.out_dir, index, points, rng.choice(thresholds), rng.randint(1, min(8, count)))


if __name__ == "__main__":
    main()
