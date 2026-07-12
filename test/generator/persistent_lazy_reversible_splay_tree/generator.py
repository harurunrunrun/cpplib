#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(a: list[int], queries: list[str]) -> str:
    versions = [a[:]]
    out: list[str] = []
    for query in queries:
        xs = query.split()
        typ, v, l, r = xs[0], int(xs[1]), int(xs[2]), int(xs[3])
        if typ == "ADD":
            x = int(xs[4])
            b = versions[v][:]
            for i in range(l, r):
                b[i] += x
            versions.append(b)
            out.append(str(len(versions) - 1))
        elif typ == "REV":
            b = versions[v][:]
            b[l:r] = reversed(b[l:r])
            versions.append(b)
            out.append(str(len(versions) - 1))
        else:
            out.append(str(sum(versions[v][l:r])))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, a: list[int], queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text("\n".join([f"{len(a)} {len(queries)}", " ".join(map(str, a)), *queries]) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(a, queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [1, 2, 3, 4], ["ADD 0 1 3 10", "SUM 1 0 4", "REV 1 0 4", "SUM 2 1 3"])
    rng = random.Random(20260919)
    a = [rng.randrange(-20, 21) for _ in range(20)]
    queries: list[str] = []
    version_count = 1
    for _ in range(100):
        v = rng.randrange(version_count)
        l = rng.randrange(len(a) + 1)
        r = rng.randrange(len(a) + 1)
        if l > r:
            l, r = r, l
        typ = rng.randrange(3)
        if typ == 0:
            queries.append(f"ADD {v} {l} {r} {rng.randrange(-10, 11)}")
            version_count += 1
        elif typ == 1:
            queries.append(f"REV {v} {l} {r}")
            version_count += 1
        else:
            queries.append(f"SUM {v} {l} {r}")
    write_case(out_dir, 1, a, queries)


if __name__ == "__main__":
    main()
