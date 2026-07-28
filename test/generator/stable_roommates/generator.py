#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def has_stable_matching(preferences: list[list[int]]) -> bool:
    count = len(preferences)
    if count % 2:
        return False
    if count == 0:
        return True
    rank = [
        {other: position for position, other in enumerate(row)}
        for row in preferences
    ]
    match = [-1] * count

    def search() -> bool:
        try:
            first = next(person for person in range(count) if match[person] < 0)
        except StopIteration:
            for left in range(count):
                for right in range(left + 1, count):
                    if match[left] == right:
                        continue
                    if (
                        rank[left][right] < rank[left][match[left]]
                        and rank[right][left] < rank[right][match[right]]
                    ):
                        return False
            return True
        for second in range(first + 1, count):
            if match[second] >= 0:
                continue
            match[first] = second
            match[second] = first
            if search():
                return True
            match[first] = -1
            match[second] = -1
        return False

    return search()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[list[list[int]]] = [[], [[1], [0]]]
    for count in [4, 6, 8, 10]:
        for _ in range(90 if count <= 8 else 30):
            preferences = []
            for person in range(count):
                row = [other for other in range(count) if other != person]
                rng.shuffle(row)
                preferences.append(row)
            cases.append(preferences)
    input_lines = [str(len(cases))]
    output_lines = []
    for preferences in cases:
        input_lines.append(str(len(preferences)))
        input_lines.extend(" ".join(map(str, row)) for row in preferences)
        output_lines.append(str(int(has_stable_matching(preferences))))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
