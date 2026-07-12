#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def convolve(kind: str, a: list[int], b: list[int]) -> list[int]:
    n = len(a)
    result = [0] * n
    mask = n - 1
    for i, left in enumerate(a):
        for j, right in enumerate(b):
            if kind == "OR":
                index = i | j
            elif kind == "AND":
                index = i & j
            elif kind == "XOR":
                index = i ^ j
            else:
                index = mask ^ (i ^ j)
            result[index] += left * right
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[str, int, list[int], list[int]]] = []
    for lg in range(8):
        n = 1 << lg
        for kind in ["OR", "AND", "XOR", "XNOR"]:
            a = [rng.randrange(-100, 101) for _ in range(n)]
            b = [rng.randrange(-100, 101) for _ in range(n)]
            cases.append((kind, lg, a, b))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for kind, lg, a, b in cases:
        lines.append(f"{kind} {lg}")
        lines.append(" ".join(map(str, a)))
        lines.append(" ".join(map(str, b)))
        outputs.append(" ".join(map(str, convolve(kind, a, b))))

    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
