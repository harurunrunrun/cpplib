#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MOD = 998244353


def compose(outer: list[int], inner: list[int], size: int) -> list[int]:
    result = [0] * size
    power = [1] + [0] * (size - 1)
    for coefficient in outer[:size]:
        for degree in range(size):
            result[degree] = (result[degree] + coefficient * power[degree]) % MOD
        next_power = [0] * size
        for left, left_value in enumerate(power):
            for right, right_value in enumerate(inner[: size - left]):
                if left + right < size:
                    next_power[left + right] = (
                        next_power[left + right] + left_value * right_value
                    ) % MOD
        power = next_power
    return result


def reversion(function: list[int], size: int) -> list[int]:
    result = [0] * size
    if size <= 1:
        return result
    inverse_linear = pow(function[1], MOD - 2, MOD)
    for degree in range(1, size):
        result[degree] = 0
        base = compose(function, result, degree + 1)[degree]
        target = 1 if degree == 1 else 0
        result[degree] = (target - base) * inverse_linear % MOD
    return result


def write_case(path: Path, cases: list[tuple[int, list[int], int, list[int]]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for kind, function, size, inner in cases:
        input_lines.append(f"{kind} {len(function)} {size}")
        input_lines.append(" ".join(map(str, function)))
        if kind == 0:
            input_lines.append(str(len(inner)))
            input_lines.append(" ".join(map(str, inner)))
            answer = compose(function, inner, size)
        else:
            answer = reversion(function, size)
        output_lines.append(" ".join(map(str, answer)))
    (path / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (path / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, list[int], int, list[int]]] = [
        (0, [1, 2, 3], 6, [4, 5]),
        (0, [], 4, [1, 1]),
        (1, [0, 1], 8, []),
        (1, [0, 2, 3, 4], 12, []),
    ]
    for _ in range(50):
        size = rng.randint(1, 24)
        outer = [rng.randrange(MOD) for _ in range(rng.randint(0, size + 3))]
        inner = [rng.randrange(MOD) for _ in range(rng.randint(0, size + 2))]
        cases.append((0, outer, size, inner))
    for _ in range(40):
        size = rng.randint(1, 24)
        function = [0, rng.randrange(1, MOD)]
        function.extend(rng.randrange(MOD) for _ in range(rng.randint(0, size + 2)))
        cases.append((1, function, size, []))
    write_case(args.out_dir, cases)


if __name__ == "__main__":
    main()
