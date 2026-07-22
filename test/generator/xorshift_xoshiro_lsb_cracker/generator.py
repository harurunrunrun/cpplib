#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MASK64 = (1 << 64) - 1


def rotate_left(value: int, shift: int) -> int:
    return ((value << shift) | (value >> (64 - shift))) & MASK64


def xorshift_observe(first: int, second: int) -> list[int]:
    observations: list[int] = []
    for _ in range(128):
        observations.append((first + second) & 1)
        old_first = first
        first = second
        old_first ^= (old_first << 23) & MASK64
        second = (
            old_first ^ second ^ (old_first >> 18) ^ (second >> 5)
        ) & MASK64
    return observations


def xoshiro_observe(words: list[int]) -> list[int]:
    state = list(words)
    observations: list[int] = []
    for _ in range(256):
        observations.append((state[0] + state[3]) & 1)
        shifted = (state[1] << 17) & MASK64
        state[2] ^= state[0]
        state[3] ^= state[1]
        state[1] ^= state[2]
        state[0] ^= state[3]
        state[2] ^= shifted
        state[3] = rotate_left(state[3], 45)
        state = [word & MASK64 for word in state]
    return observations


Case = tuple[str, list[int], str]


def xorshift_case(first: int, second: int) -> Case:
    return (
        "xorshift128plus",
        xorshift_observe(first, second),
        f"{first} {second}",
    )


def xoshiro_case(words: list[int]) -> Case:
    return (
        "xoshiro256plus",
        xoshiro_observe(words),
        " ".join(map(str, words)),
    )


def write_cases(output_directory: Path, name: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for engine, observations, expected in cases:
        input_lines.append(
            f"{engine} {len(observations)} "
            + " ".join(map(str, observations))
        )
        output_lines.append(expected)
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed: list[Case] = [
        xorshift_case(0, 0),
        xorshift_case(1, 0),
        xorshift_case(0, 1),
        xorshift_case(MASK64, MASK64),
        xoshiro_case([0, 0, 0, 0]),
        xoshiro_case([1, 0, 0, 0]),
        xoshiro_case([0, 1, 0, 0]),
        xoshiro_case([MASK64] * 4),
        ("xorshift128plus", [0] * 127, "invalid"),
        ("xoshiro256plus", [0] * 257, "invalid"),
        ("xorshift128plus", [2] + [0] * 127, "invalid"),
        ("unknown", [], "invalid"),
    ]
    write_cases(args.out_dir, "case_00_fixed_and_invalid", fixed)

    basis = [
        xorshift_case(1 << bit, 0) if bit < 64
        else xorshift_case(0, 1 << (bit - 64))
        for bit in range(128)
    ]
    basis.extend(
        xoshiro_case([
            (1 << (bit % 64)) if word == bit // 64 else 0
            for word in range(4)
        ])
        for bit in range(256)
    )
    write_cases(args.out_dir, "case_01_basis", basis)

    rng = random.Random(2026072202)
    random_cases: list[Case] = []
    for _ in range(512):
        random_cases.append(
            xorshift_case(rng.getrandbits(64), rng.getrandbits(64))
        )
    for _ in range(384):
        random_cases.append(
            xoshiro_case([rng.getrandbits(64) for _ in range(4)])
        )
    write_cases(args.out_dir, "case_02_random", random_cases)


if __name__ == "__main__":
    main()
