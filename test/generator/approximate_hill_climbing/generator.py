#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071311)
    cases = [(-20, 20, -20, 20), (0, 0, -1, 1), (99, -99, -100, 100)]
    for _ in range(40):
        lower = rng.randint(-500, 0)
        upper = rng.randint(0, 500)
        target = rng.randint(lower, upper)
        initial = rng.randint(lower, upper)
        cases.append((initial, target, lower, upper))
    (args.out_dir / "cases.in").write_text(
        f"{len(cases)}\n" + "".join(
            f"{initial} {target} {lower} {upper}\n"
            for initial, target, lower, upper in cases
        ),
        encoding="utf-8",
    )
    (args.out_dir / "cases.out").write_text(
        "0 0 0 0 0\n" * len(cases), encoding="utf-8"
    )


if __name__ == "__main__":
    main()
