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
        typ = xs[0]
        version = int(xs[1])
        if typ == "INS":
            p, x = int(xs[2]), int(xs[3])
            b = versions[version][:]
            b.insert(p, x)
            versions.append(b)
            out.append(str(len(versions) - 1))
        elif typ == "ERASE":
            p = int(xs[2])
            b = versions[version][:]
            b.pop(p)
            versions.append(b)
            out.append(str(len(versions) - 1))
        elif typ == "SET":
            p, x = int(xs[2]), int(xs[3])
            b = versions[version][:]
            b[p] = x
            versions.append(b)
            out.append(str(len(versions) - 1))
        elif typ == "ADD":
            l, r, x = int(xs[2]), int(xs[3]), int(xs[4])
            b = versions[version][:]
            for i in range(l, r):
                b[i] += x
            versions.append(b)
            out.append(str(len(versions) - 1))
        elif typ == "REV":
            l, r = int(xs[2]), int(xs[3])
            b = versions[version][:]
            b[l:r] = reversed(b[l:r])
            versions.append(b)
            out.append(str(len(versions) - 1))
        elif typ == "GET":
            out.append(str(versions[version][int(xs[2])]))
        else:
            l, r = int(xs[2]), int(xs[3])
            out.append(str(sum(versions[version][l:r])))
    return "\n".join(out) + ("\n" if out else "")


def write_case(out_dir: Path, idx: int, a: list[int], queries: list[str]) -> None:
    name = f"case_{idx:02d}"
    lines = [f"{len(a)} {len(queries)}", " ".join(map(str, a)), *queries]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(a, queries), encoding="utf-8")


def random_queries(a: list[int], count: int, seed: int) -> list[str]:
    rng = random.Random(seed)
    versions = [a[:]]
    queries: list[str] = []
    for _ in range(count):
        version = rng.randrange(len(versions))
        current = versions[version]
        typ = rng.randrange(7)
        if typ == 0 or not current:
            p = rng.randrange(len(current) + 1)
            x = rng.randrange(-100, 101)
            queries.append(f"INS {version} {p} {x}")
            b = current[:]
            b.insert(p, x)
            versions.append(b)
        elif typ == 1:
            p = rng.randrange(len(current))
            queries.append(f"ERASE {version} {p}")
            b = current[:]
            b.pop(p)
            versions.append(b)
        elif typ == 2:
            p = rng.randrange(len(current))
            x = rng.randrange(-100, 101)
            queries.append(f"SET {version} {p} {x}")
            b = current[:]
            b[p] = x
            versions.append(b)
        elif typ == 3:
            l = rng.randrange(len(current) + 1)
            r = rng.randrange(len(current) + 1)
            if l > r:
                l, r = r, l
            x = rng.randrange(-20, 21)
            queries.append(f"ADD {version} {l} {r} {x}")
            b = current[:]
            for i in range(l, r):
                b[i] += x
            versions.append(b)
        elif typ == 4:
            l = rng.randrange(len(current) + 1)
            r = rng.randrange(len(current) + 1)
            if l > r:
                l, r = r, l
            queries.append(f"REV {version} {l} {r}")
            b = current[:]
            b[l:r] = reversed(b[l:r])
            versions.append(b)
        elif typ == 5:
            queries.append(f"GET {version} {rng.randrange(len(current))}")
        else:
            l = rng.randrange(len(current) + 1)
            r = rng.randrange(len(current) + 1)
            if l > r:
                l, r = r, l
            queries.append(f"SUM {version} {l} {r}")
    return queries


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        0,
        [1, 2, 3, 4],
        [
            "ADD 0 1 3 10",
            "REV 1 0 4",
            "INS 2 2 -5",
            "SET 0 0 20",
            "ERASE 3 1",
            "GET 5 2",
            "SUM 2 0 4",
            "SUM 0 0 4",
        ],
    )
    write_case(
        out_dir,
        1,
        [],
        ["INS 0 0 5", "ADD 1 0 1 3", "REV 2 0 1", "GET 3 0", "ERASE 3 0", "SUM 4 0 0"],
    )
    rng = random.Random(20260919)
    a = [rng.randrange(-20, 21) for _ in range(24)]
    write_case(out_dir, 2, a, random_queries(a, 240, 20261004))
    (out_dir / "case_03.in").write_text("-1 0\n", encoding="utf-8")
    (out_dir / "case_03.out").write_text("", encoding="utf-8")


    monotone_queries = [f"INS {i} {0 if i % 2 else i} {i - 800}" for i in range(1600)]
    monotone_queries += ["GET 1600 0", "GET 1600 1599", "SUM 1600 0 1600"]
    write_case(out_dir, 4, [], monotone_queries)


if __name__ == "__main__":
    main()