#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MASK32 = (1 << 32) - 1
MASK64 = (1 << 64) - 1


def rotate_left(value: int, shift: int) -> int:
    return ((value << shift) | (value >> (64 - shift))) & MASK64


def xoroshiro_observe(first: int, second: int) -> list[int]:
    result: list[int] = []
    for _ in range(128):
        result.append((first + second) & 1)
        second ^= first
        first = (rotate_left(first, 55) ^ second ^ (second << 14)) & MASK64
        second = rotate_left(second, 36)
    return result


def well_observe(words: list[int]) -> list[int]:
    words = list(words)
    index = 0
    result: list[int] = []
    for _ in range(512):
        first = words[index]
        third = words[(index + 13) & 15]
        second = (
            first ^ third ^ (first << 16) ^ (third << 15)
        ) & MASK32
        third = words[(index + 9) & 15]
        third = (third ^ (third >> 11)) & MASK32
        first = (second ^ third) & MASK32
        words[index] = first
        fourth = (first ^ ((first << 5) & 0xDA442D24)) & MASK32
        index = (index + 15) & 15
        first = words[index]
        words[index] = (
            first ^ second ^ fourth ^ (first << 2)
            ^ (second << 18) ^ (third << 28)
        ) & MASK32
        result.append(words[index] & 1)
    return result


Case = tuple[str, list[int], str]


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


def xoroshiro_case(first: int, second: int) -> Case:
    return (
        "xoroshiro128plus",
        xoroshiro_observe(first, second),
        f"{first} {second}",
    )


def well_case(words: list[int]) -> Case:
    return (
        "well512a",
        well_observe(words),
        " ".join(map(str, words)) + " 0",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed: list[Case] = [
        xoroshiro_case(0, 0),
        xoroshiro_case(1, 0),
        xoroshiro_case(0, 1),
        xoroshiro_case(MASK64, MASK64),
        well_case([0] * 16),
        well_case([1 << (index % 32) for index in range(16)]),
        well_case([MASK32] * 16),
        ("xoroshiro128plus", [0] * 127, "invalid"),
        ("well512a", [0] * 513, "invalid"),
        ("xoroshiro128plus", [2] + [0] * 127, "invalid"),
        ("unknown", [], "invalid"),
    ]
    write_cases(args.out_dir, "case_00_fixed_and_invalid", fixed)

    basis = [
        xoroshiro_case(1 << bit, 0) if bit < 64
        else xoroshiro_case(0, 1 << (bit - 64))
        for bit in range(128)
    ]
    basis.extend(
        well_case([
            (1 << (bit % 32)) if index == bit // 32 else 0
            for index in range(16)
        ])
        for bit in range(512)
    )
    write_cases(args.out_dir, "case_01_basis", basis)

    rng = random.Random(2026072201)
    random_cases: list[Case] = []
    for _ in range(512):
        random_cases.append(
            xoroshiro_case(rng.getrandbits(64), rng.getrandbits(64))
        )
    for _ in range(256):
        random_cases.append(well_case([rng.getrandbits(32) for _ in range(16)]))
    write_cases(args.out_dir, "case_02_random", random_cases)


if __name__ == "__main__":
    main()
