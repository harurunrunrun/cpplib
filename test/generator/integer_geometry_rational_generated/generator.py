#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from fractions import Fraction
from pathlib import Path


def encode(value: Fraction) -> str:
    return f"{value.numerator} {value.denominator}"


def answer(value: Fraction) -> str:
    if value.denominator == 1:
        return str(value.numerator)
    return f"{value.numerator}/{value.denominator}"


def random_fraction(generator: random.Random, bits: int = 280) -> Fraction:
    numerator = generator.getrandbits(bits)
    if generator.randrange(2):
        numerator = -numerator
    denominator = generator.getrandbits(bits - 17) | 1
    return Fraction(numerator, denominator)


def cross(first: tuple[Fraction, Fraction], second: tuple[Fraction, Fraction]) -> Fraction:
    return first[0] * second[1] - first[1] * second[0]


def subtract(first: tuple[Fraction, Fraction], second: tuple[Fraction, Fraction]) -> tuple[Fraction, Fraction]:
    return first[0] - second[0], first[1] - second[1]


def point_text(point: tuple[Fraction, Fraction]) -> str:
    return f"{encode(point[0])} {encode(point[1])}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    generator = random.Random(2026072601)
    inputs: list[str] = []
    outputs: list[str] = []
    for _ in range(10):
        first = random_fraction(generator)
        second = random_fraction(generator)
        if second == 0:
            second = Fraction(1, 3)
        for operation, result in (
            ("A", first + second),
            ("S", first - second),
            ("M", first * second),
            ("D", first / second),
        ):
            inputs.append(f"{operation} {encode(first)} {encode(second)}")
            outputs.append(answer(result))
        inputs.append(f"C {encode(first)} {encode(second)}")
        outputs.append(str((first > second) - (first < second)))

    for _ in range(8):
        points = [
            (random_fraction(generator, 210), random_fraction(generator, 210))
            for _ in range(3)
        ]
        determinant = cross(
            subtract(points[1], points[0]),
            subtract(points[2], points[0]),
        )
        inputs.append("O " + " ".join(point_text(point) for point in points))
        outputs.append(str((determinant > 0) - (determinant < 0)))

    intersection_count = 0
    while intersection_count < 8:
        points = [
            (random_fraction(generator, 190), random_fraction(generator, 190))
            for _ in range(4)
        ]
        first_direction = subtract(points[1], points[0])
        second_direction = subtract(points[3], points[2])
        denominator = cross(first_direction, second_direction)
        if denominator == 0:
            continue
        parameter = cross(
            subtract(points[2], points[0]), second_direction
        ) / denominator
        intersection = (
            points[0][0] + first_direction[0] * parameter,
            points[0][1] + first_direction[1] * parameter,
        )
        inputs.append("I " + " ".join(point_text(point) for point in points))
        outputs.append(f"{answer(intersection[0])} {answer(intersection[1])}")
        intersection_count += 1

    inputs.append("I 0 1 0 1 1 1 0 1 0 1 2 1 1 1 2 1")
    outputs.append("none")

    (args.out_dir / "case_00.in").write_text(
        f"{len(inputs)}\n" + "\n".join(inputs) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
