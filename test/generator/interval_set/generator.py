#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(queries: list[tuple[str, int, int]]) -> str:
    used = [False] * 80
    out: list[str] = []
    for typ, l, r in queries:
        if typ == "ADD":
            for i in range(l, r):
                used[i] = True
        elif typ == "ERASE":
            for i in range(l, r):
                used[i] = False
        elif typ == "CONTAINS":
            out.append(str(int(used[l])))
        elif typ == "COVERED":
            out.append(str(int(all(used[i] for i in range(l, r)))))
        elif typ == "MEX":
            x = l
            while x < len(used) and used[x]:
                x += 1
            out.append(str(x))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, queries: list[tuple[str, int, int]]) -> None:
    name = f"case_{idx:02d}"
    data = "\n".join([str(len(queries)), *[f"{typ} {l} {r}" for typ, l, r in queries]]) + "\n"
    (out_dir / f"{name}.in").write_text(data, encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ("ADD", 1, 4),
        ("ADD", 4, 7),
        ("COVERED", 1, 7),
        ("ERASE", 3, 5),
        ("CONTAINS", 2, 0),
        ("CONTAINS", 3, 0),
        ("MEX", 1, 0),
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260908)
    queries: list[tuple[str, int, int]] = []
    for _ in range(200):
        typ = rng.choice(["ADD", "ERASE", "CONTAINS", "COVERED", "MEX"])
        l = rng.randrange(0, 64)
        r = rng.randrange(0, 64)
        if l > r:
            l, r = r, l
        if typ in {"CONTAINS", "MEX"}:
            r = 0
        queries.append((typ, l, r))
    write_case(out_dir, 1, queries)


if __name__ == "__main__":
    main()
