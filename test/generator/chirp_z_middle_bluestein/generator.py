#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def middle(values: list[int], kernel: list[int]) -> list[int]:
    return [
        sum(values[start + index] * kernel[index]
            for index in range(len(kernel))) % MOD
        for start in range(len(values) - len(kernel) + 1)
    ]


def chirp(values: list[int], initial: int, ratio: int, count: int) -> list[int]:
    result: list[int] = []
    point = initial
    for _ in range(count):
        power = 1
        value = 0
        for coefficient in values:
            value = (value + coefficient * power) % MOD
            power = power * point % MOD
        result.append(value)
        point = point * ratio % MOD
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, list[int], list[int], int, int, int]] = [
        (0, [1, 2, 3, 4], [5, 6], 0, 0, 0),
        (0, [7], [3], 0, 0, 0),
        (1, [1, 2, 3], [], 5, 4, 2),
        (1, [1, 2, 3], [], 4, 7, 0),
    ]
    for _ in range(60):
        kernel_size = rng.randint(1, 30)
        value_size = rng.randint(kernel_size, 60)
        values = [rng.randrange(MOD) for _ in range(value_size)]
        kernel = [rng.randrange(MOD) for _ in range(kernel_size)]
        cases.append((0, values, kernel, 0, 0, 0))
    for _ in range(60):
        values = [rng.randrange(MOD) for _ in range(rng.randint(0, 40))]
        cases.append((
            1, values, [], rng.randint(0, 50),
            rng.randrange(MOD), rng.randrange(MOD),
        ))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for kind, values, kernel, count, initial, ratio in cases:
        input_lines.append(f"{kind} {len(values)}")
        input_lines.append(" ".join(map(str, values)))
        if kind == 0:
            input_lines.append(str(len(kernel)))
            input_lines.append(" ".join(map(str, kernel)))
            answer = middle(values, kernel)
        else:
            input_lines.append(f"{count} {initial} {ratio}")
            answer = chirp(values, initial, ratio, count)
        output_lines.append(" ".join(map(str, answer)))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
