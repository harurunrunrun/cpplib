#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def cross(left: tuple[int, int, int], right: tuple[int, int, int]) -> tuple[int, int, int]:
    x1, y1, z1 = left
    x2, y2, z2 = right
    return y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - y1 * x2


def write_case(out_dir: Path, name: str, seed: int, count: int) -> None:
    rng = random.Random(seed)
    input_lines = [str(count)]
    output_lines: list[str] = []
    for index in range(count):
        query_type = index % 5
        if query_type == 0:
            left = tuple(rng.randrange(-20, 21) for _ in range(3))
            right = tuple(rng.randrange(-20, 21) for _ in range(3))
            input_lines.append(
                "0 " + " ".join(map(str, (*left, *right)))
            )
            output_lines.append(" ".join(map(str, cross(left, right))))
        elif query_type == 1:
            points = [
                tuple(rng.randrange(-10, 11) for _ in range(3))
                for _ in range(4)
            ]
            a, b, c, d = points
            first = tuple(b[i] - a[i] for i in range(3))
            second = tuple(c[i] - a[i] for i in range(3))
            third = tuple(d[i] - a[i] for i in range(3))
            value = sum(
                first[i] * cross(second, third)[i]
                for i in range(3)
            )
            input_lines.append(
                "1 " + " ".join(
                    str(coordinate)
                    for point in points
                    for coordinate in point
                )
            )
            output_lines.append(str(value))
        elif query_type == 2:
            a = tuple(rng.randrange(-20, 21) for _ in range(3))
            length = rng.randrange(1, 11)
            b = (a[0] + length, a[1], a[2])
            point = tuple(rng.randrange(-20, 21) for _ in range(3))
            closest_x = min(max(point[0], a[0]), b[0])
            squared = (
                (point[0] - closest_x) ** 2
                + (point[1] - a[1]) ** 2
                + (point[2] - a[2]) ** 2
            )
            input_lines.append(
                "2 " + " ".join(map(str, (*a, *b, *point)))
            )
            output_lines.append(str(squared))
        elif query_type == 3:
            center = tuple(rng.randrange(-20, 21) for _ in range(3))
            radius = rng.randrange(0, 16)
            point = tuple(rng.randrange(-20, 21) for _ in range(3))
            squared = sum(
                (point[i] - center[i]) ** 2
                for i in range(3)
            )
            relation = 1 if squared < radius * radius else (
                0 if squared == radius * radius else -1
            )
            input_lines.append(
                "3 " + " ".join(map(str, (*center, radius, *point)))
            )
            output_lines.append(str(relation))
        else:
            x = rng.randrange(-20, 21)
            y = rng.randrange(-20, 21)
            first_z = rng.randrange(-20, 0)
            second_z = rng.randrange(1, 21)
            plane_z = rng.randrange(first_z + 1, second_z)
            input_lines.append(
                "4 " + " ".join(map(str, (
                    x, y, first_z,
                    x, y, second_z,
                    rng.randrange(-20, 21),
                    rng.randrange(-20, 21),
                    plane_z,
                    0, 0, rng.choice([-3, -1, 1, 4]),
                )))
            )
            output_lines.append(f"{x} {y} {plane_z}")

    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "small", 2026071316, 25)
    write_case(out_dir, "random", 20260804, 500)


if __name__ == "__main__":
    main()
