#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Rect = tuple[int, int, int, int]


def solve(rectangles: list[Rect]) -> int:
    result = 0
    for x in range(-10, 10):
        for y in range(-10, 10):
            parity = 0
            for x1, y1, x2, y2 in rectangles:
                x1, x2 = min(x1, x2), max(x1, x2)
                y1, y2 = min(y1, y2), max(y1, y2)
                if x1 <= x < x2 and y1 <= y < y2:
                    parity ^= 1
            result += parity
    return result


def write_case(out_dir: Path, idx: int, rectangles: list[Rect]) -> None:
    name = f"case_{idx:02d}"
    lines = [str(len(rectangles)), *[f"{x1} {y1} {x2} {y2}" for x1, y1, x2, y2 in rectangles]]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(f"{solve(rectangles)}\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [(0, 0, 3, 3), (1, 1, 4, 4)])
    rng = random.Random(20260912)
    for case_id in range(1, 4):
        rectangles = []
        for _ in range(8):
            rectangles.append(tuple(rng.randrange(-7, 8) for _ in range(4)))  # type: ignore[arg-type]
        write_case(out_dir, case_id, rectangles)


if __name__ == "__main__":
    main()
