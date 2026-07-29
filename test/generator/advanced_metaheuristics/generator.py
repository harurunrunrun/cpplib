#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    random_engine = random.Random(0xA4D4A12)
    batches: list[list[tuple[int, int, int, int]]] = [
        [(0, 0, -1, 1), (-10, 10, -10, 10)],
    ]
    for _ in range(5):
        cases: list[tuple[int, int, int, int]] = []
        for _ in range(12):
            lower = random_engine.randint(-1000, 0)
            upper = random_engine.randint(0, 1000)
            initial = random_engine.randint(lower, upper)
            target = random_engine.randint(lower, upper)
            cases.append((initial, target, lower, upper))
        batches.append(cases)

    expected = " ".join(["0"] * 21)
    for batch_index, cases in enumerate(batches):
        input_text = f"{len(cases)}\n" + "".join(
            f"{initial} {target} {lower} {upper}\n"
            for initial, target, lower, upper in cases
        )
        output_text = "".join(f"{expected}\n" for _ in cases)
        (arguments.out_dir / f"{batch_index}.in").write_text(
            input_text, encoding="utf-8"
        )
        (arguments.out_dir / f"{batch_index}.out").write_text(
            output_text, encoding="utf-8"
        )


if __name__ == "__main__":
    main()
