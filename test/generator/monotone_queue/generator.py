#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def line(values: list[int]) -> str:
    return " ".join(map(str, values))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[list[int], int]] = [
        ([5], 1),
        ([3, 3, 3, 3], 2),
        ([9, 1, 8, 2, 7, 3, 6, 4, 5], 4),
        (list(range(40)), 40),
        (list(range(40, 0, -1)), 7),
    ]
    for _ in range(20):
        n = rng.randrange(1, 180)
        window = rng.randrange(1, n + 1)
        values = [rng.randrange(-20, 21) for _ in range(n)]
        cases.append((values, window))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for values, window in cases:
        input_lines.extend([f"{len(values)} {window}", line(values)])
        minima: list[int] = []
        maxima: list[int] = []
        indices: list[int] = []
        for right in range(window, len(values) + 1):
            left = right - window
            part = values[left:right]
            minimum = min(part)
            minima.append(minimum)
            maxima.append(max(part))
            indices.append(max(i for i in range(left, right) if values[i] == minimum))
        output_lines.extend([line(minima), line(maxima), line(indices)])

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
