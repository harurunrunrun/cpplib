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
        typ, v, l, r = query.split()
        v, l, r = int(v), int(l), int(r)
        if typ == "REV":
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

    write_case(out_dir, 0, [1, 2, 3], ["SUM 0 0 3", "REV 0 0 3", "SUM 1 0 2", "SUM 0 0 2"])
    rng = random.Random(20260918)
    a = [rng.randrange(-20, 21) for _ in range(20)]
    queries: list[str] = []
    version_count = 1
    for _ in range(120):
        v = rng.randrange(version_count)
        l = rng.randrange(len(a) + 1)
        r = rng.randrange(len(a) + 1)
        if l > r:
            l, r = r, l
        if rng.randrange(2) == 0:
            queries.append(f"REV {v} {l} {r}")
            version_count += 1
        else:
            queries.append(f"SUM {v} {l} {r}")
    write_case(out_dir, 1, a, queries)


if __name__ == "__main__":
    main()
