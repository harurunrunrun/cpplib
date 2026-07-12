#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(a: list[int], queries: list[tuple[int, int]]) -> str:
    out: list[str] = []
    for l, r in queries:
        out.append(str(len(set(a[l:r]))))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, a: list[int], queries: list[tuple[int, int]]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{len(a)} {len(queries)}", " ".join(map(str, a))]
    lines += [f"{l} {r}" for l, r in queries]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(a, queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [1, 2, 1, 3, 2], [(0, 3), (1, 5), (2, 2)])
    rng = random.Random(20260910)
    a = [rng.randrange(16) for _ in range(80)]
    queries = []
    for _ in range(120):
        l = rng.randrange(len(a) + 1)
        r = rng.randrange(len(a) + 1)
        if l > r:
            l, r = r, l
        queries.append((l, r))
    write_case(out_dir, 1, a, queries)


if __name__ == "__main__":
    main()
