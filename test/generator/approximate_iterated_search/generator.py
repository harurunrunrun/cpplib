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
    rng = random.Random(2026071315)
    cases = [(0, 0, -1, 1), (-50, 50, -50, 50)]
    for _ in range(40):
        lower = rng.randint(-300, 0)
        upper = rng.randint(0, 300)
        cases.append((
            rng.randint(lower, upper),
            rng.randint(lower, upper),
            lower,
            upper,
        ))
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
