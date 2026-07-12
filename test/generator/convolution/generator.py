#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def convolve(a: list[int], b: list[int], mod: int) -> list[int]:
    if not a or not b:
        return []
    result = [0] * (len(a) + len(b) - 1)
    for i, x in enumerate(a):
        for j, y in enumerate(b):
            result[i + j] = (result[i + j] + x * y) % mod
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases: list[tuple[str, list[int], list[int]]] = [
        ("NTT", [], [1]),
        ("NTT", [1, 2, 3], [4, 5]),
        ("ARB", [1, -2, 3], [-4, 5]),
        ("ARB", [], []),
    ]
    for _ in range(80):
        kind = rng.choice(["NTT", "ARB"])
        n = rng.randrange(1, 65)
        m = rng.randrange(1, 65)
        a = [rng.randrange(-10**9, 10**9 + 1) for _ in range(n)]
        b = [rng.randrange(-10**9, 10**9 + 1) for _ in range(m)]
        cases.append((kind, a, b))

    lines = [str(len(cases))]
    outputs: list[str] = []
    for kind, a, b in cases:
        lines.append(f"{kind} {len(a)} {len(b)}")
        lines.append(" ".join(map(str, a)))
        lines.append(" ".join(map(str, b)))
        mod = 998244353 if kind == "NTT" else 1000000007
        result = convolve(a, b, mod)
        outputs.append(" ".join(map(str, [len(result), *result])))

    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
