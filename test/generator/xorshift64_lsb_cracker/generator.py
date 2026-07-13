#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MASK = (1 << 64) - 1


def observe(seed: int) -> list[int]:
    state = seed
    result: list[int] = []
    for _ in range(64):
        state ^= (state << 13) & MASK
        state ^= state >> 7
        state ^= (state << 17) & MASK
        state &= MASK
        result.append(state & 1)
    return result


def advance(seed: int, count: int) -> int:
    state = seed
    for _ in range(count):
        state ^= (state << 13) & MASK
        state ^= state >> 7
        state ^= (state << 17) & MASK
        state &= MASK
    return state


def encode(cases: list[tuple[list[int], str]]) -> tuple[str, str]:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for observations, expected in cases:
        input_lines.append(
            f"{len(observations)} " + " ".join(map(str, observations))
        )
        output_lines.append(expected)
    return "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n"


def write_case(
    output_directory: Path,
    name: str,
    cases: list[tuple[list[int], str]],
) -> None:
    input_text, output_text = encode(cases)
    (output_directory / f"{name}.in").write_text(input_text, encoding="utf-8")
    (output_directory / f"{name}.out").write_text(output_text, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    fixed_seeds = [
        0,
        1,
        2,
        3,
        1 << 63,
        88172645463325252,
        MASK,
    ]
    fixed_cases = [(observe(seed), str(seed)) for seed in fixed_seeds]
    fixed_cases.extend(
        [
            ([0] * 63, "invalid"),
            ([0] * 65, "invalid"),
            ([2] + [0] * 63, "invalid"),
            ([0] * 31 + [255] + [0] * 32, "invalid"),
        ]
    )
    write_case(output_directory, "case_00_fixed_and_invalid", fixed_cases)

    basis_cases = [
        (observe(1 << bit), str(1 << bit))
        for bit in range(64)
    ]
    write_case(output_directory, "case_01_basis", basis_cases)

    rng = random.Random(20260714)
    random_seeds = [rng.getrandbits(64) for _ in range(4096)]
    random_cases = [(observe(seed), str(seed)) for seed in random_seeds]
    for index, seed in enumerate(random_seeds[:1024]):
        state_before_observation = advance(seed or 1, index % 257)
        random_cases.append(
            (observe(state_before_observation), str(state_before_observation))
        )
    write_case(output_directory, "case_02_random", random_cases)

    dense_seeds = [
        ((index * 0x9E3779B97F4A7C15) ^ (index << 37) ^ (index >> 3)) & MASK
        for index in range(20000)
    ]
    dense_cases = [(observe(seed), str(seed)) for seed in dense_seeds]
    write_case(output_directory, "case_03_dense", dense_cases)


if __name__ == "__main__":
    main()
