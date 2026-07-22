#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MASK64 = (1 << 64) - 1


def rotate_left(value: int, shift: int) -> int:
    return ((value << shift) | (value >> (64 - shift))) & MASK64


def xoshiro512_next(words: list[int]) -> int:
    result = (words[0] + words[2]) & MASK64
    shifted = (words[1] << 11) & MASK64
    words[2] ^= words[0]
    words[5] ^= words[1]
    words[1] ^= words[2]
    words[7] ^= words[3]
    words[3] ^= words[4]
    words[4] ^= words[5]
    words[0] ^= words[6]
    words[6] ^= words[7]
    words[6] ^= shifted
    words[7] = rotate_left(words[7], 21)
    for index in range(8):
        words[index] &= MASK64
    return result


def xoroshiro1024_next(words: list[int], index: int) -> tuple[int, int]:
    previous = index
    index = (index + 1) & 15
    first = words[index]
    last = words[previous]
    result = (first * 0x9E3779B97F4A7C13) & MASK64
    last ^= first
    words[previous] = (
        rotate_left(first, 25) ^ last ^ (last << 27)
    ) & MASK64
    words[index] = rotate_left(last, 36)
    return result, index


def xoshiro_observations(words: list[int]) -> list[int]:
    state = list(words)
    return [xoshiro512_next(state) & 1 for _ in range(512)]


def xoroshiro_observations(words: list[int], index: int) -> list[int]:
    state = list(words)
    observations: list[int] = []
    for _ in range(1024):
        value, index = xoroshiro1024_next(state, index)
        observations.append(value & 1)
    return observations


Case = tuple[str, int, list[int], str]


def xoshiro_case(words: list[int]) -> Case:
    return (
        "xoshiro512plus",
        0,
        xoshiro_observations(words),
        " ".join(map(str, words)),
    )


def xoroshiro_case(words: list[int], index: int) -> Case:
    return (
        "xoroshiro1024star",
        index,
        xoroshiro_observations(words, index),
        " ".join(map(str, words)) + f" {index}",
    )


def write_cases(directory: Path, name: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for engine, index, observations, expected in cases:
        input_lines.append(
            f"{engine} {index} {len(observations)} "
            + " ".join(map(str, observations))
        )
        output_lines.append(expected)
    (directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed: list[Case] = [
        xoshiro_case([0] * 8),
        xoshiro_case([MASK64] * 8),
        xoroshiro_case([0] * 16, 0),
        xoroshiro_case([MASK64] * 16, 15),
        ("xoshiro512plus", 0, [0] * 511, "invalid"),
        ("xoroshiro1024star", 0, [0] * 1025, "invalid"),
        ("xoroshiro1024star", 16, [0] * 1024, "invalid"),
        ("xoroshiro1024star", -1, [0] * 1024, "invalid"),
        ("xoshiro512plus", 0, [0] * 199 + [2] + [0] * 312, "invalid"),
        ("unknown", 0, [], "invalid"),
    ]
    write_cases(args.out_dir, "case_00_fixed_and_invalid", fixed)

    basis: list[Case] = []
    for word in range(8):
        for bit in (0, 1, 31, 63):
            state = [0] * 8
            state[word] = 1 << bit
            basis.append(xoshiro_case(state))
    for word in range(16):
        for bit in (0, 1, 31, 63):
            state = [0] * 16
            state[word] = 1 << bit
            basis.append(xoroshiro_case(state, (word * 7 + bit) & 15))
    write_cases(args.out_dir, "case_01_basis", basis)

    generator = random.Random(2026072202)
    random_cases: list[Case] = []
    for _ in range(32):
        random_cases.append(
            xoshiro_case([generator.getrandbits(64) for _ in range(8)])
        )
    for _ in range(32):
        random_cases.append(xoroshiro_case(
            [generator.getrandbits(64) for _ in range(16)],
            generator.randrange(16),
        ))
    write_cases(args.out_dir, "case_02_random", random_cases)


if __name__ == "__main__":
    main()
