#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def convolve(a: list[int], b: list[int]) -> list[int]:
    if not a or not b:
        return []
    result = [0] * (len(a) + len(b) - 1)
    for i, left in enumerate(a):
        for j, right in enumerate(b):
            result[i + j] += left * right
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases = [([], [1, 2, 3]), ([1, 2, 3], []), ([1, -2, 3], [-4, 5])]
    for _ in range(100):
        n = rng.randrange(1, 81)
        m = rng.randrange(1, 81)
        a = [rng.randrange(-10000, 10001) for _ in range(n)]
        b = [rng.randrange(-10000, 10001) for _ in range(m)]
        cases.append((a, b))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for a, b in cases:
        lines.append(f"{len(a)} {len(b)}")
        lines.append(" ".join(map(str, a)))
        lines.append(" ".join(map(str, b)))
        result = convolve(a, b)
        outputs.append(" ".join(map(str, [len(result), *result])))

    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
