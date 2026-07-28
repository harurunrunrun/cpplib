#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    parser.add_argument("--seed", type=int, default=20260729)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)

    rng = random.Random(arguments.seed)
    points = [
        (rng.randint(-12, 12), rng.randint(-12, 12))
        for _ in range(180)
    ]
    points.extend([(0, 0), (0, 0), (-12, 12), (12, -12)])
    queries: list[tuple[int, int, int, int]] = [
        (-20, 20, -20, 20),
        (0, 0, -20, 20),
        (-20, 20, 0, 0),
        (0, 1, 0, 1),
        (12, 13, -12, -11),
    ]
    for _ in range(500):
        first_x = rng.randint(-15, 15)
        second_x = rng.randint(-15, 15)
        first_y = rng.randint(-15, 15)
        second_y = rng.randint(-15, 15)
        lower_x, upper_x = sorted((first_x, second_x))
        lower_y, upper_y = sorted((first_y, second_y))
        queries.append((lower_x, upper_x, lower_y, upper_y))

    lines = [f"{len(points)} {len(queries)}"]
    lines.extend(f"{x} {y}" for x, y in points)
    lines.extend(
        f"{lower_x} {upper_x} {lower_y} {upper_y}"
        for lower_x, upper_x, lower_y, upper_y in queries
    )
    (output / "case_00.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (output / "case_00.out").write_text("", encoding="utf-8")


if __name__ == "__main__":
    main()
