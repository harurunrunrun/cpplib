#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def convolution(concave: list[int], arbitrary: list[int]) -> list[int]:
    if not concave or not arbitrary:
        return []
    answer: list[int] = []
    for output in range(len(concave) + len(arbitrary) - 1):
        left = max(0, output - len(arbitrary) + 1)
        right = min(len(concave) - 1, output)
        answer.append(min(concave[i] + arbitrary[output - i]
                          for i in range(left, right + 1)))
    return answer


def make_concave(rng: random.Random, size: int) -> list[int]:
    if size == 0:
        return []
    slopes = sorted((rng.randint(-30, 30) for _ in range(size - 1)),
                    reverse=True)
    values = [rng.randint(-100, 100)]
    for slope in slopes:
        values.append(values[-1] + slope)
    return values


def write_case(out_dir: Path, index: int,
               cases: list[tuple[list[int], list[int]]],
               answers: list[list[int]] | None = None) -> None:
    input_parts = [str(len(cases))]
    output_parts: list[str] = []
    for case_index, (concave, arbitrary) in enumerate(cases):
        input_parts.append(f"{len(concave)} {len(arbitrary)}")
        input_parts.append(" ".join(map(str, concave)))
        input_parts.append(" ".join(map(str, arbitrary)))
        answer = (answers[case_index] if answers is not None
                  else convolution(concave, arbitrary))
        output_parts.append(" ".join(map(str, answer)))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_parts) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_parts) + "\n", encoding="utf-8"
    )


def constant_arbitrary_answer(concave: list[int], size: int,
                              value: int) -> list[int]:
    answer: list[int] = []
    for output in range(len(concave) + size - 1):
        left = max(0, output - size + 1)
        right = min(len(concave) - 1, output)
        answer.append(min(concave[left], concave[right]) + value)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ([], [1, 2]), ([1], []), ([5], [7]),
        ([0, 3, 5, 6, 6], [4, -3, 8, 1]),
        ([10, 5, 0, -5], [2]),
        ([0, 0, 0, 0], [5, 4, 3, 2, 1]),
        ([0, 10, 0], [100, -100, 100]),
    ]
    write_case(args.out_dir, 0, fixed)

    rng = random.Random(2026071308)
    random_cases: list[tuple[list[int], list[int]]] = []
    for _ in range(350):
        n = rng.randrange(1, 45)
        m = rng.randrange(1, 45)
        random_cases.append((
            make_concave(rng, n),
            [rng.randint(-1000, 1000) for _ in range(m)],
        ))
    for offset in range(0, len(random_cases), 70):
        write_case(args.out_dir, 1 + offset // 70,
                   random_cases[offset:offset + 70])

    large_concave = make_concave(rng, 100000)
    constant = 37
    large_arbitrary = [constant] * 100000
    write_case(
        args.out_dir, 6, [(large_concave, large_arbitrary)],
        [constant_arbitrary_answer(large_concave, len(large_arbitrary), constant)],
    )


if __name__ == "__main__":
    main()
