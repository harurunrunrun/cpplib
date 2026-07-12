#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def max_right(a: list[int], left: int, limit: int) -> int:
    total = 0
    r = left
    while r < len(a) and total + a[r] <= limit:
        total += a[r]
        r += 1
    return r


def min_left(a: list[int], right: int, limit: int) -> int:
    total = 0
    left = right
    while left > 0 and total + a[left - 1] <= limit:
        left -= 1
        total += a[left]
    return left


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20261117)
    n = 80
    a = [rng.randrange(20) for _ in range(n)]
    initial = a[:]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(400):
        typ = rng.randrange(6)
        if typ < 2:
            k = rng.randrange(n)
            x = rng.randrange(20)
            a[k] = x
            commands.append(f"SET {k} {x}")
        elif typ == 2:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(a[k]))
        elif typ == 3:
            l = rng.randrange(n + 1)
            r = rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            commands.append(f"PROD {l} {r}")
            outputs.append(str(sum(a[l:r])))
        elif typ == 4:
            l = rng.randrange(n + 1)
            limit = rng.randrange(sum(a[l:]) + 1 if l < n else 1)
            commands.append(f"MR {l} {limit}")
            outputs.append(str(max_right(a, l, limit)))
        else:
            r = rng.randrange(n + 1)
            limit = rng.randrange(sum(a[:r]) + 1 if r > 0 else 1)
            commands.append(f"ML {r} {limit}")
            outputs.append(str(min_left(a, r, limit)))
    commands.append("ALL")
    outputs.append(str(sum(a)))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
