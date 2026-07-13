#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    path: Path,
    number: int,
    points: list[tuple[int, int]],
    queries: list[tuple[int, int]],
    k: int,
) -> None:
    lines = [f"{len(points)} {len(queries)} {k}"]
    lines.extend(f"{x} {y}" for x, y in points)
    lines.extend(f"{x} {y}" for x, y in queries)
    answers: list[str] = []
    for x, y in queries:
        ranked = sorted(
            ((px - x) ** 2 + (py - y) ** 2, index)
            for index, (px, py) in enumerate(points)
        )
        answers.append(" ".join(str(index) for _, index in ranked[:k]))
    (path / f"case_{number:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (path / f"case_{number:02d}.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        0,
        [(0, 0), (0, 0), (1, 0), (0, 1), (-1, 0), (0, -1), (10, 10), (9, 10)],
        [(0, 0), (10, 9), (2, 2)],
        3,
    )

    rng = random.Random(20260713)
    points = [(rng.randrange(-100, 101), rng.randrange(-100, 101)) for _ in range(48)]
    points[17] = points[3]
    queries = [(rng.randrange(-120, 121), rng.randrange(-120, 121)) for _ in range(24)]
    write_case(out_dir, 1, points, queries, 5)

    write_case(
        out_dir,
        2,
        [(7, -4)] * 7 + [(8, -4), (6, -4)],
        [(7, -4), (7, -3), (100, 100)],
        6,
    )

    large = 4_000_000_000_000_000_000
    write_case(
        out_dir,
        3,
        [(-large, 0), (large, 0), (0, -large), (0, large), (0, 0)],
        [(large, large), (-large, -large), (0, 0)],
        2,
    )


if __name__ == "__main__":
    main()
