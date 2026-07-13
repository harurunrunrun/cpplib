#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071342)
    cases = [
        ([2.0], [-7.0], [1.0]),
        ([1.0, -3.0], [8.0, 9.0], [1.0, 4.0]),
        ([0.0, 2.0, -4.0], [7.0, -5.0, 3.0], [0.5, 2.0, 5.0]),
    ]
    for dimension in range(1, 7):
        target = [rng.uniform(-5, 5) for _ in range(dimension)]
        initial = [target[index] + rng.uniform(-8, 8) for index in range(dimension)]
        weight = [rng.uniform(0.5, 5) for _ in range(dimension)]
        cases.append((target, initial, weight))
    lines = [str(len(cases))]
    for target, initial, weight in cases:
        lines.append(str(len(target)))
        lines.append(" ".join(f"{value:.15g}" for value in target))
        lines.append(" ".join(f"{value:.15g}" for value in initial))
        lines.append(" ".join(f"{value:.15g}" for value in weight))
    (args.out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
