#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
from pathlib import Path


Point = tuple[float, float]


def write_case(
    out_dir: Path,
    index: int,
    center: Point,
    radius: float,
    polygon: list[Point],
    signed_area: float,
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{len(polygon)} {center[0]} {center[1]} {radius}"]
    lines += [f"{x} {y}" for x, y in polygon]
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(
        f"{signed_area:.8f}\n{abs(signed_area):.8f}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ((0.0, 0.0), 1.0, [(-2.0, -2.0), (2.0, -2.0), (2.0, 2.0), (-2.0, 2.0)], math.pi),
        ((0.0, 0.0), 1.0, [(-2.0, 2.0), (2.0, 2.0), (2.0, -2.0), (-2.0, -2.0)], -math.pi),
        ((0.0, 0.0), 10.0, [(0.0, 0.0), (1.0, 0.0), (0.0, 1.0)], 0.5),
        ((0.0, 0.0), 10.0, [(0.0, 1.0), (1.0, 0.0), (0.0, 0.0)], -0.5),
        ((0.0, 0.0), 2.0, [(0.0, -5.0), (5.0, -5.0), (5.0, 5.0), (0.0, 5.0)], 2.0 * math.pi),
        ((0.0, 0.0), 2.0, [(0.0, 5.0), (5.0, 5.0), (5.0, -5.0), (0.0, -5.0)], -2.0 * math.pi),
        ((2.0, -3.0), 1.0, [(0.0, -5.0), (4.0, -5.0), (4.0, -1.0), (0.0, -1.0)], math.pi),
        ((0.0, 0.0), 0.0, [(-1.0, -1.0), (1.0, -1.0), (1.0, 1.0), (-1.0, 1.0)], 0.0),
    ]
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
