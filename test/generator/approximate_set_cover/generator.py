#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def normalize(sets: list[list[int]]) -> list[list[int]]:
    return [sorted(set(values)) for values in sets]


def greedy_cover(universe: int, sets: list[list[int]], weights: list[int] | None) -> tuple[list[int], list[int]]:
    sets = normalize(sets)
    covered = [False] * universe
    selected: list[int] = []
    while True:
        gains = [sum(not covered[x] for x in values) for values in sets]
        candidates = [index for index, gain in enumerate(gains) if gain]
        if not candidates:
            break
        if weights is None:
            chosen = min(candidates, key=lambda index: (-gains[index], index))
        else:
            chosen = candidates[0]
            for index in candidates[1:]:
                left = weights[index] * gains[chosen]
                right = weights[chosen] * gains[index]
                if left < right or (left == right and index < chosen):
                    chosen = index
        selected.append(chosen)
        for element in sets[chosen]:
            covered[element] = True
    return selected, [index for index, value in enumerate(covered) if not value]


def max_coverage(universe: int, sets: list[list[int]], budget: int) -> tuple[list[int], list[int], list[int]]:
    sets = normalize(sets)
    covered = [False] * universe
    selected: list[int] = []
    while len(selected) < budget:
        gains = [sum(not covered[x] for x in values) for values in sets]
        if not gains:
            break
        gain, negated = max((value, -index) for index, value in enumerate(gains))
        if gain == 0:
            break
        chosen = -negated
        selected.append(chosen)
        for element in sets[chosen]:
            covered[element] = True
    return (
        selected,
        [index for index, value in enumerate(covered) if value],
        [index for index, value in enumerate(covered) if not value],
    )


def encode(values: list[int]) -> str:
    return str(len(values)) + "".join(f" {value}" for value in values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071311)
    cases: list[tuple[int, list[list[int]], list[int], int]] = [
        (0, [], [], 0),
        (4, [], [], 5),
        (5, [[0, 0, 1], [1, 2], [3], [4, 3]], [3, 1, 0, 4], 2),
        (6, [[0, 2], [1, 3], [2, 4]], [2, 2, 1], 10),
    ]
    for _ in range(45):
        universe = rng.randrange(0, 36)
        set_count = rng.randrange(0, 45)
        sets: list[list[int]] = []
        for _ in range(set_count):
            size = rng.randrange(0, universe + 1) if universe else 0
            values = [rng.randrange(universe) for _ in range(size)] if universe else []
            if values and rng.randrange(3) == 0:
                values.append(values[0])
            sets.append(values)
        weights = [rng.randrange(0, 10**12) for _ in range(set_count)]
        cases.append((universe, sets, weights, rng.randrange(0, set_count + 5)))
    universe = 500
    sets = [[(17 * index + 31 * offset) % universe for offset in range(40)] for index in range(700)]
    cases.append((universe, sets, [10**18 - index for index in range(700)], 80))

    inputs = [str(len(cases))]
    outputs: list[str] = []
    for universe, sets, weights, budget in cases:
        inputs.append(f"{universe} {len(sets)} {budget}")
        inputs.append(" ".join(map(str, weights)))
        inputs.extend(str(len(values)) + "".join(f" {x}" for x in values) for values in sets)
        plain, plain_missing = greedy_cover(universe, sets, None)
        weighted, weighted_missing = greedy_cover(universe, sets, weights)
        chosen, covered, missing = max_coverage(universe, sets, budget)
        outputs.append(
            f"{encode(plain)} | {encode(plain_missing)} | {encode(weighted)} | "
            f"{encode(weighted_missing)} | {encode(chosen)} | {encode(covered)} | {encode(missing)}"
        )
    (out_dir / "case_00.in").write_text("\n".join(inputs) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
