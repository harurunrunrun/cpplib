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
    rng = random.Random(2026071313)
    cases = [(0, 0)]
    for _ in range(50):
        cases.append((rng.randint(-1000, 1000), rng.randint(-1000, 1000)))
    (args.out_dir / "cases.in").write_text(
        f"{len(cases)}\n" + "".join(
            f"{initial} {target} {abs(initial - target) + 2}\n"
            for initial, target in cases
        ),
        encoding="utf-8",
    )
    (args.out_dir / "cases.out").write_text(
        "0 0 0 0 0\n" * len(cases), encoding="utf-8"
    )


if __name__ == "__main__":
    main()
