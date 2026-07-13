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

    random_source = random.Random(2026071312)
    cases: list[tuple[list[float], list[float]]] = [
        ([0.0], [5.0]),
        ([3.0, -4.0], [-6.0, 7.0]),
        ([0.0] * 12, [float((-1) ** i * (i + 1)) for i in range(12)]),
    ]
    for dimension in range(1, 13):
        target = [random_source.uniform(-6.0, 6.0) for _ in range(dimension)]
        initial = [
            target[index] + random_source.uniform(-10.0, 10.0)
            for index in range(dimension)
        ]
        cases.append((target, initial))
    lines = [str(len(cases))]
    for target, initial in cases:
        lines.append(str(len(target)))
        lines.append(" ".join(f"{value:.15g}" for value in target))
        lines.append(" ".join(f"{value:.15g}" for value in initial))
    (args.out_dir / "cases.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
