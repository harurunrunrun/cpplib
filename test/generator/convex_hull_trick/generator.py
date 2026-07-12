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
    lines: list[tuple[int, int]] = []
    slope = 10**6
    for _ in range(400):
        slope -= rng.randrange(5)
        intercept = rng.randrange(-10**12, 10**12 + 1)
        lines.append((slope, intercept))

    arbitrary_queries = [-(10**9), -1, 0, 1, 10**9]
    arbitrary_queries.extend(rng.randrange(-10**9, 10**9 + 1) for _ in range(500))
    monotone_queries = sorted(
        [-(10**9), -1, 0, 1, 10**9]
        + [rng.randrange(-10**9, 10**9 + 1) for _ in range(500)]
    )
    outputs = [
        str(min(a * x + b for a, b in lines))
        for x in arbitrary_queries + monotone_queries
    ]

    input_lines = [f"{len(lines)} {len(arbitrary_queries)} {len(monotone_queries)}"]
    input_lines.extend(f"{a} {b}" for a, b in lines)
    input_lines.extend(map(str, arbitrary_queries))
    input_lines.extend(map(str, monotone_queries))
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

    fixed_cases = [
        ([(3, -8_000_000_000_000_000_000),
          (2, 8_000_000_000_000_000_000),
          (1, -7_000_000_000_000_000_000)], [-10, 0, 10]),
        ([(0, 3_000_000_000_000_000_000)], [-1, 0, 1]),
    ]
    for index, (fixed_lines, queries) in enumerate(fixed_cases, start=1):
        fixed_input = [f"{len(fixed_lines)} {len(queries)} 0"]
        fixed_input.extend(f"{a} {b}" for a, b in fixed_lines)
        fixed_input.extend(map(str, queries))
        fixed_output = [str(min(a * x + b for a, b in fixed_lines)) for x in queries]
        (out_dir / f"case_{index:02d}.in").write_text(
            "\n".join(fixed_input) + "\n", encoding="utf-8")
        (out_dir / f"case_{index:02d}.out").write_text(
            "\n".join(fixed_output) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
