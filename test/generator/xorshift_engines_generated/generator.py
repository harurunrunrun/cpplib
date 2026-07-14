#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MASK32 = (1 << 32) - 1
MASK64 = (1 << 64) - 1


def next32(value: int) -> int:
    value ^= value << 13 & MASK32
    value ^= value >> 17
    value ^= value << 5 & MASK32
    return value & MASK32


def next64(value: int) -> int:
    value ^= value << 13 & MASK64
    value ^= value >> 7
    value ^= value << 17 & MASK64
    return value & MASK64


def next128(state: list[int]) -> int:
    temporary = (state[0] ^ (state[0] << 11 & MASK32)) & MASK32
    state[0], state[1], state[2] = state[1], state[2], state[3]
    state[3] = (
        state[3] ^ (state[3] >> 19) ^ temporary ^ (temporary >> 8)
    ) & MASK32
    return state[3]


Case = tuple[int, int, int, int, tuple[int, int, int, int], int, int]


def encode(cases: list[Case]) -> tuple[str, str]:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for seed32, skip32, seed64, skip64, seed128, skip128, count in cases:
        input_lines.append(
            f"{seed32} {skip32} {seed64} {skip64} "
            f"{' '.join(map(str, seed128))} {skip128} {count}"
        )

        state32 = seed32
        state64 = seed64
        state128 = list(seed128)
        for _ in range(skip32):
            state32 = next32(state32)
        for _ in range(skip64):
            state64 = next64(state64)
        for _ in range(skip128):
            next128(state128)

        recovered32 = state32
        recovered64 = state64
        recovered128 = tuple(state128)
        state32 = next32(state32)
        state64 = next64(state64)
        for _ in range(4):
            next128(state128)
        output_lines.append(
            f"{recovered32} {recovered64} "
            f"{' '.join(map(str, recovered128))}"
        )
        for _ in range(count):
            state32 = next32(state32)
            state64 = next64(state64)
            value128 = next128(state128)
            output_lines.append(f"{state32} {state64} {value128}")
    return "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n"


def write_case(directory: Path, name: str, cases: list[Case]) -> None:
    input_text, output_text = encode(cases)
    (directory / f"{name}.in").write_text(input_text, encoding="utf-8")
    (directory / f"{name}.out").write_text(output_text, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed: list[Case] = [
        (
            1,
            0,
            1,
            0,
            (1, 0, 0, 0),
            0,
            8,
        ),
        (
            MASK32,
            1,
            MASK64,
            1,
            (0, 0, 0, 1),
            1,
            8,
        ),
        (
            1 << 31,
            31,
            1 << 63,
            63,
            (MASK32, 0, MASK32, 0),
            127,
            8,
        ),
        (
            2463534242,
            257,
            88172645463325252,
            255,
            (123456789, 362436069, 521288629, 88675123),
            256,
            8,
        ),
    ]
    for bit in range(32):
        fixed.append(
            (
                1 << bit,
                bit,
                1 << (2 * bit % 64),
                2 * bit,
                (1 << bit, 1 << ((bit + 7) % 32), 0, MASK32),
                3 * bit,
                3,
            )
        )
    write_case(args.out_dir, "fixed", fixed)

    rng = random.Random(0x584F525348494654)
    for group in range(4):
        cases: list[Case] = []
        for _ in range(128):
            seed32 = rng.randrange(1, 1 << 32)
            seed64 = rng.randrange(1, 1 << 64)
            seed128 = tuple(rng.randrange(1 << 32) for _ in range(4))
            if not any(seed128):
                seed128 = (1, 0, 0, 0)
            cases.append(
                (
                    seed32,
                    rng.randrange(258),
                    seed64,
                    rng.randrange(258),
                    seed128,
                    rng.randrange(258),
                    rng.randrange(1, 9),
                )
            )
        write_case(args.out_dir, f"random_{group}", cases)


if __name__ == "__main__":
    main()
