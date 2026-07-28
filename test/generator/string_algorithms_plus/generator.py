#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    cases = ["-"]
    for length in range(1, 10):
        cases.extend(map("".join, itertools.product("ab", repeat=length)))
    rng = random.Random(args.seed)
    for _ in range(300):
        cases.append(
            "".join(rng.choice("abc") for _ in range(rng.randint(1, 35)))
        )
    (args.out_dir / "cases.in").write_text(
        f"{len(cases)}\n" + "\n".join(cases) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
