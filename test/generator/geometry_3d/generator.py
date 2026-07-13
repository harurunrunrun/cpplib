#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def cross(left: tuple[int, int, int], right: tuple[int, int, int]) -> tuple[int, int, int]:
    return (
        left[1] * right[2] - left[2] * right[1],
        left[2] * right[0] - left[0] * right[2],
        left[0] * right[1] - left[1] * right[0],
    )


def triple(first: tuple[int, int, int], second: tuple[int, int, int], third: tuple[int, int, int]) -> int:
    product = cross(second, third)
    return sum(first[index] * product[index] for index in range(3))


def subtract(left: tuple[int, int, int], right: tuple[int, int, int]) -> tuple[int, int, int]:
    return tuple(left[index] - right[index] for index in range(3))  # type: ignore[return-value]


def make_case(rng: random.Random, query_count: int, bound: int) -> tuple[list[str], list[str]]:
    operations: list[str] = []
    answers: list[str] = []
    for _ in range(query_count):
        operation = rng.randrange(5)
        if operation == 0:
            left = tuple(rng.randint(-bound, bound) for _ in range(3))
            right = tuple(rng.randint(-bound, bound) for _ in range(3))
            result = cross(left, right)
            operations.append("0 " + " ".join(map(str, left + right)))
            answers.append(" ".join(map(str, result)))
        elif operation == 1:
            points = [
                tuple(rng.randint(-bound, bound) for _ in range(3))
                for _ in range(4)
            ]
            result = triple(
                subtract(points[1], points[0]),
                subtract(points[2], points[0]),
                subtract(points[3], points[0]),
            )
            operations.append(
                "1 " + " ".join(str(value) for point in points for value in point)
            )
            answers.append(str(result))
        elif operation == 2:
            axis = rng.randrange(3)
            first = [rng.randint(-bound, bound) for _ in range(3)]
            second = first.copy()
            second[axis] += rng.randint(1, max(1, bound))
            point = [rng.randint(-bound, bound) for _ in range(3)]
            closest = first.copy()
            closest[axis] = min(max(point[axis], first[axis]), second[axis])
            squared_distance = sum(
                (point[index] - closest[index]) ** 2 for index in range(3)
            )
            operations.append(
                "2 " + " ".join(map(str, first + second + point))
            )
            answers.append(str(squared_distance))
        elif operation == 3:
            center = [rng.randint(-bound, bound) for _ in range(3)]
            radius = rng.randint(0, max(1, bound))
            point = [rng.randint(-bound, bound) for _ in range(3)]
            squared_distance = sum(
                (point[index] - center[index]) ** 2 for index in range(3)
            )
            relation = (radius * radius > squared_distance) - (
                radius * radius < squared_distance
            )
            operations.append(
                "3 " + " ".join(map(str, center + [radius] + point))
            )
            answers.append(str(relation))
        else:
            axis = rng.randrange(3)
            first = [rng.randint(-bound, bound) for _ in range(3)]
            second = first.copy()
            step = rng.randint(1, max(1, bound))
            if rng.randrange(2):
                step = -step
            second[axis] += step
            plane_point = [rng.randint(-bound, bound) for _ in range(3)]
            normal = [0, 0, 0]
            normal[axis] = 1
            result = first.copy()
            result[axis] = plane_point[axis]
            operations.append(
                "4 " + " ".join(
                    map(str, first + second + plane_point + normal)
                )
            )
            answers.append(" ".join(map(str, result)))
    return operations, answers


def write_case(
    out_dir: Path,
    index: int,
    operations: list[str],
    answers: list[str],
) -> None:
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(operations)}\n" + "\n".join(operations) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    manual_operations = [
        "0 1 0 0 0 1 0",
        "1 0 0 0 1 0 0 0 1 0 0 0 1",
        "2 0 0 0 5 0 0 2 3 4",
        "3 0 0 0 5 3 4 0",
        "4 1 2 -4 1 2 6 0 0 0 0 0 1",
    ]
    manual_answers = ["0 0 1", "1", "25", "0", "1 2 0"]
    write_case(args.out_dir, 0, manual_operations, manual_answers)

    rng = random.Random(2026071317)
    for index in range(1, 21):
        operations, answers = make_case(rng, 200 + index * 20, 10**6)
        write_case(args.out_dir, index, operations, answers)
    operations, answers = make_case(rng, 50000, 10**4)
    write_case(args.out_dir, 21, operations, answers)


if __name__ == "__main__":
    main()
