#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(data: list[int]) -> str:
    zipped: list[tuple[int, int]] = []
    for x in data:
        if not zipped or zipped[-1][0] != x:
            zipped.append((x, 1))
        else:
            zipped[-1] = (x, zipped[-1][1] + 1)
    lines = [str(len(zipped))]
    lines += [f"{x} {c}" for x, c in zipped]
    lines.append(" ".join(map(str, data)))
    return "\n".join(lines) + "\n"


def write_case(out_dir: Path, idx: int, data: list[int]) -> None:
    name = f"case_{idx:02d}"
    (out_dir / f"{name}.in").write_text(str(len(data)) + "\n" + " ".join(map(str, data)) + "\n", encoding="utf-8")
    (out_dir / f"{name}.out").write_text(solve(data), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [])
    write_case(out_dir, 1, [1, 1, 1, 2, 2, 3, 1, 1])
    rng = random.Random(20260915)
    data = []
    for _ in range(100):
        if data and rng.randrange(3):
            data.append(data[-1])
        else:
            data.append(rng.randrange(-3, 4))
    write_case(out_dir, 2, data)


if __name__ == "__main__":
    main()
