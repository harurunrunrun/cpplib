#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from functools import lru_cache
from pathlib import Path


def solve(digits: str) -> int:
    prefix = [0]
    for digit in digits:
        prefix.append(prefix[-1] + int(digit))

    @lru_cache(maxsize=None)
    def dfs(position: int, previous_sum: int) -> int:
        if position == len(digits):
            return 1
        answer = 0
        for end in range(position + 1, len(digits) + 1):
            segment_sum = prefix[end] - prefix[position]
            if previous_sum <= segment_sum:
                answer += dfs(end, segment_sum)
        return answer

    return dfs(0, 0)


def encode(value: str) -> str:
    return value if value else "-"


def write_case(
    out_dir: Path,
    index: int,
    cases: list[tuple[str, int]],
) -> None:
    stem = f"case_{index:02d}"
    input_lines = [str(len(cases))]
    input_lines.extend(encode(digits) for digits, _ in cases)
    output_lines = [str(answer) for _, answer in cases]
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed_inputs = [
        "",
        "0",
        "111",
        "123",
        "90",
        "0000000000000000000000000000000000000000000000000000000000000000",
        "9876543210",
    ]
    fixed = [(digits, solve(digits)) for digits in fixed_inputs]
    write_case(out_dir, 0, fixed)

    rng = random.Random(20260717)
    random_cases = []
    for _ in range(350):
        digits = "".join(
            rng.choice("0123456789") for _ in range(rng.randrange(19))
        )
        random_cases.append((digits, solve(digits)))
    write_case(out_dir, 1, random_cases)

    performance = "9" + "0" * 649
    write_case(out_dir, 2, [(performance, 1)])


if __name__ == "__main__":
    main()
