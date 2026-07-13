#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    points = [
        (rng.randrange(-50, 51), rng.randrange(-50, 51), point_id)
        for point_id in range(700)
    ]
    rng.shuffle(points)
    queries = [
        (-100, 100, -100, 100),
        (0, 0, 0, 0),
        (-50, -50, -50, 50),
        (-10, 10, -10, 10),
    ]
    for _ in range(500):
        x1, x2 = sorted((rng.randrange(-70, 71), rng.randrange(-70, 71)))
        y1, y2 = sorted((rng.randrange(-70, 71), rng.randrange(-70, 71)))
        queries.append((x1, x2, y1, y2))

    input_lines = [f"{len(points)} {len(queries)}"]
    input_lines.extend(f"{x} {y} {point_id}" for x, y, point_id in points)
    output_lines: list[str] = []
    for min_x, max_x, min_y, max_y in queries:
        input_lines.append(f"{min_x} {max_x} {min_y} {max_y}")
        ids = sorted(
            point_id
            for x, y, point_id in points
            if min_x <= x <= max_x and min_y <= y <= max_y
        )
        output_lines.append(" ".join([str(len(ids)), *map(str, ids)]))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
