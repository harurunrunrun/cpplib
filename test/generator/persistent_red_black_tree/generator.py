#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(queries: list[str]) -> str:
    versions: list[set[int]] = [set()]
    out: list[str] = []
    for query in queries:
        xs = query.split()
        typ, v, x = xs[0], int(xs[1]), int(xs[2])
        if typ == "INSERT":
            versions.append(set(versions[v]))
            versions[-1].add(x)
            out.append(str(len(versions) - 1))
        elif typ == "ERASE":
            versions.append(set(versions[v]))
            versions[-1].discard(x)
            out.append(str(len(versions) - 1))
        elif typ == "CONTAINS":
            out.append(str(int(x in versions[v])))
        elif typ == "KTH":
            values = sorted(versions[v])
            out.append(str(values[x] if 0 <= x < len(values) else -1))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text("\n".join([str(len(queries)), *queries]) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(queries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        "INSERT 0 5",
        "INSERT 1 2",
        "CONTAINS 1 2",
        "CONTAINS 2 2",
        "ERASE 2 5",
        "KTH 3 0",
    ]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260916)
    queries: list[str] = []
    version_count = 1
    for _ in range(200):
        typ = rng.randrange(4)
        if typ == 0:
            queries.append(f"INSERT {rng.randrange(version_count)} {rng.randrange(50)}")
            version_count += 1
        elif typ == 1:
            queries.append(f"ERASE {rng.randrange(version_count)} {rng.randrange(50)}")
            version_count += 1
        elif typ == 2:
            queries.append(f"CONTAINS {rng.randrange(version_count)} {rng.randrange(50)}")
        else:
            queries.append(f"KTH {rng.randrange(version_count)} {rng.randrange(60) - 5}")
    write_case(out_dir, 1, queries)


if __name__ == "__main__":
    main()
