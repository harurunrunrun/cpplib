#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def make_case(rng: random.Random, n: int) -> str:
    points = [(rng.randrange(-100, 101), rng.randrange(-100, 101)) for _ in range(n)]
    demand = [0] + [rng.randrange(1, 8) for _ in range(n - 1)]
    capacity = max(max(demand, default=0), rng.randrange(8, 20))
    distance = [
        [
            abs(points[i][0] - points[j][0])
            + abs(points[i][1] - points[j][1])
            + (0 if i == j else (i * 7 + j * 11) % 5)
            for j in range(n)
        ]
        for i in range(n)
    ]
    lines = [f"{n} {capacity}"]
    lines.extend(f"{x} {y}" for x, y in points)
    lines.append(" ".join(map(str, demand)))
    lines.extend(" ".join(map(str, row)) for row in distance)
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026072901)

    sizes = [1, 2, 3, 5, 8, 11]
    sizes.extend(rng.randrange(2, 18) for _ in range(44))
    cases = [make_case(rng, n) for n in sizes]
    (out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n" + "\n".join(cases) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "OK\n" * len(cases),
        encoding="utf-8",
    )

    stress_sizes = [24, 31, 40]
    stress = [make_case(rng, n) for n in stress_sizes]
    (out_dir / "case_01.in").write_text(
        str(len(stress)) + "\n" + "\n".join(stress) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_01.out").write_text(
        "OK\n" * len(stress),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
