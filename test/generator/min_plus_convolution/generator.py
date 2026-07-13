#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def make_convex(rng: random.Random, size: int) -> list[int]:
    if size == 0:
        return []
    values = [rng.randrange(-100, 101)]
    slope = rng.randrange(-20, 21)
    for _ in range(1, size):
        slope += rng.randrange(0, 6)
        values.append(values[-1] + slope)
    return values


def naive(left: list[int], right: list[int]) -> list[int]:
    if not left or not right:
        return []
    result = [10**100] * (len(left) + len(right) - 1)
    for i, left_value in enumerate(left):
        for j, right_value in enumerate(right):
            result[i + j] = min(result[i + j], left_value + right_value)
    return result


def write_case(out_dir: Path, index: int, mode: str,
               left: list[int], right: list[int], answer: list[int]) -> None:
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        f"{mode} {len(left)} {len(right)}\n"
        + " ".join(map(str, left)) + "\n"
        + " ".join(map(str, right)) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071304)
    cases: list[tuple[str, list[int], list[int], list[int]]] = []
    boundary_cases = [
        ("C", [], [1, -2, 3]),
        ("A", [1, -2, 3], []),
        ("C", [7], [-9]),
        ("C", [-5, -2, 1, 4, 7], [3, -8, 2, 2]),
        ("A", [3, -8, 2, 2], [-5, -5, -5, -5]),
        ("B", [-5, -2, 1, 4, 7], [-8, -8, -5, 1]),
    ]
    for mode, left, right in boundary_cases:
        cases.append((mode, left, right, naive(left, right)))
    for _ in range(20):
        n, m = rng.randrange(1, 55), rng.randrange(1, 55)
        convex = make_convex(rng, n)
        arbitrary = [rng.randrange(-500, 501) for _ in range(m)]
        choice = rng.randrange(3)
        if choice == 0:
            cases.append(("C", convex, arbitrary, naive(convex, arbitrary)))
        elif choice == 1:
            cases.append(("A", arbitrary, convex, naive(arbitrary, convex)))
        else:
            other_convex = make_convex(rng, m)
            cases.append(("B", convex, other_convex, naive(convex, other_convex)))

    large_n, large_m = 120000, 100000
    large_convex = [i * i for i in range(large_n)]
    large_arbitrary = [0] * large_m
    large_answer = [max(0, k - (large_m - 1)) ** 2
                    for k in range(large_n + large_m - 1)]
    cases.append(("C", large_convex, large_arbitrary, large_answer))

    for index, (mode, left, right, answer) in enumerate(cases):
        write_case(out_dir, index, mode, left, right, answer)


if __name__ == "__main__":
    main()
