#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def exact(weights: list[int], target: int) -> int:
    answer = sum(weights)
    for mask in range(1 << len(weights)):
        value = sum(
            weight for index, weight in enumerate(weights)
            if mask >> index & 1
        )
        if value >= target:
            answer = min(answer, value)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)

    random_engine = random.Random(0xC070FF)
    batches: list[list[tuple[list[int], int]]] = [
        [([1], 1), ([2, 3, 5, 7], 8), ([4, 4, 9], 7)],
    ]
    for _ in range(5):
        cases: list[tuple[list[int], int]] = []
        for _ in range(5):
            n = random_engine.randrange(2, 9)
            weights = [random_engine.randrange(1, 16) for _ in range(n)]
            target = random_engine.randrange(1, sum(weights) + 1)
            cases.append((weights, target))
        batches.append(cases)

    for batch_index, cases in enumerate(batches):
        input_lines = [str(len(cases))]
        output_lines: list[str] = []
        for weights, target in cases:
            input_lines.append(f"{len(weights)} {target}")
            input_lines.append(" ".join(map(str, weights)))
            output_lines.append(str(exact(weights, target)))
        (output / f"{batch_index}.in").write_text(
            "\n".join(input_lines) + "\n", encoding="utf-8"
        )
        (output / f"{batch_index}.out").write_text(
            "\n".join(output_lines) + "\n", encoding="utf-8"
        )


if __name__ == "__main__":
    main()
