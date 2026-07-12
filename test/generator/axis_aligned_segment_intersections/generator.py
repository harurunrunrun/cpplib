#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Segment = tuple[int, int, int, int]


def is_horizontal(segment: Segment) -> bool:
    return segment[1] == segment[3]


def intersects(horizontal: Segment, vertical: Segment) -> bool:
    hx1, hy, hx2, _ = horizontal
    vx, vy1, _, vy2 = vertical
    return min(hx1, hx2) <= vx <= max(hx1, hx2) and min(vy1, vy2) <= hy <= max(vy1, vy2)


def solve(segments: list[Segment]) -> int:
    result = 0
    for i, first in enumerate(segments):
        for second in segments[i + 1 :]:
            if is_horizontal(first) == is_horizontal(second):
                continue
            horizontal, vertical = (first, second) if is_horizontal(first) else (second, first)
            result += intersects(horizontal, vertical)
    return result


def write_case(out_dir: Path, index: int, segments: list[Segment]) -> None:
    stem = f"case_{index:02d}"
    lines = [str(len(segments)), *(" ".join(map(str, segment)) for segment in segments)]
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(f"{solve(segments)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[Segment]] = [
        [],
        [(0, 0, 5, 0), (2, -2, 2, 2)],
        [(0, 0, 5, 0), (0, -1, 0, 1), (5, -1, 5, 1)],
        [(-3, 1, 4, 1), (2, -4, 2, 5), (-3, 1, 4, 1), (2, -4, 2, 5)],
    ]
    rng = random.Random(0xC61A)
    for _ in range(12):
        segments: list[Segment] = []
        for _ in range(rng.randint(1, 80)):
            if rng.randrange(2) == 0:
                x1 = rng.randint(-20, 19)
                x2 = rng.randint(x1 + 1, 20)
                y = rng.randint(-20, 20)
                segments.append((x1, y, x2, y))
            else:
                y1 = rng.randint(-20, 19)
                y2 = rng.randint(y1 + 1, 20)
                x = rng.randint(-20, 20)
                segments.append((x, y1, x, y2))
        cases.append(segments)
    for index, segments in enumerate(cases):
        write_case(out_dir, index, segments)


if __name__ == "__main__":
    main()
