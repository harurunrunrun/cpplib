#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def ucb1_choice(counts: list[int], sums: list[int]) -> int:
    for arm, count in enumerate(counts):
        if count == 0:
            return arm
    total = sum(counts)
    scores = [
        sums[arm] / counts[arm]
        + math.sqrt(2.0 * math.log(total) / counts[arm])
        for arm in range(len(counts))
    ]
    return max(range(len(counts)), key=lambda arm: (scores[arm], -arm))


def tuned_choice(
    counts: list[int],
    sums: list[int],
    squares: list[int],
) -> int:
    for arm, count in enumerate(counts):
        if count == 0:
            return arm
    total = sum(counts)
    logarithm = math.log(total)
    scores = []
    for arm, count in enumerate(counts):
        mean = sums[arm] / count
        variance = max(0.0, squares[arm] / count - mean * mean)
        adjusted = min(
            0.25, variance + math.sqrt(2.0 * logarithm / count)
        )
        scores.append(mean + math.sqrt(logarithm * adjusted / count))
    return max(range(len(counts)), key=lambda arm: (scores[arm], -arm))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026072902)

    cases: list[tuple[int, list[tuple[int, int]]]] = [
        (1, []),
        (3, [(0, 1)]),
        (2, [(0, 1), (1, 0)]),
        (3, [(0, 1), (1, 1), (2, 0), (0, 0), (1, 1), (2, 1)]),
    ]
    for _ in range(40):
        arms = rng.randrange(1, 9)
        updates = [
            (rng.randrange(arms), rng.randrange(2))
            for _ in range(rng.randrange(0, 80))
        ]
        cases.append((arms, updates))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for arms, updates in cases:
        input_lines.append(f"{arms} {len(updates)}")
        input_lines.extend(f"{arm} {reward}" for arm, reward in updates)
        counts = [0] * arms
        sums = [0] * arms
        squares = [0] * arms
        success = [0] * arms
        failure = [0] * arms
        for arm, reward in updates:
            counts[arm] += 1
            sums[arm] += reward
            squares[arm] += reward * reward
            if reward:
                success[arm] += 1
            else:
                failure[arm] += 1
        answer = [str(ucb1_choice(counts, sums))]
        answer.append(str(tuned_choice(counts, sums, squares)))
        for arm in range(arms):
            answer.extend(
                [str(success[arm] + 1), str(failure[arm] + 1)]
            )
        output_lines.append(" ".join(answer))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
