#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MASK32 = (1 << 32) - 1


def next32(state: int) -> int:
    state ^= (state << 13) & MASK32
    state ^= state >> 17
    state ^= (state << 5) & MASK32
    return state & MASK32


def observe32(state: int) -> list[int]:
    result: list[int] = []
    for _ in range(32):
        state = next32(state)
        result.append(state & 1)
    return result


def next128(state: list[int]) -> int:
    temporary = state[0] ^ ((state[0] << 11) & MASK32)
    state[0], state[1], state[2] = state[1], state[2], state[3]
    state[3] = (
        state[3] ^ (state[3] >> 19)
        ^ temporary ^ (temporary >> 8)
    ) & MASK32
    return state[3]


def observe128(state: list[int]) -> list[int]:
    state = list(state)
    result: list[int] = []
    for _ in range(128):
        result.append(next128(state) & 1)
    return result


Case = tuple[str, list[int], str]


def case32(state: int) -> Case:
    return "xorshift32", observe32(state), str(state)


def case128(state: list[int]) -> Case:
    return "xorshift128", observe128(state), " ".join(map(str, state))


def write_cases(directory: Path, name: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for engine, observations, expected in cases:
        input_lines.append(
            f"{engine} {len(observations)} "
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
        case32(0),
        case32(1),
        case32(2463534242),
        case32(MASK32),
        case128([0, 0, 0, 0]),
        case128([123456789, 362436069, 521288629, 88675123]),
        case128([MASK32] * 4),
        ("xorshift32", [0] * 31, "invalid"),
        ("xorshift32", [0] * 33, "invalid"),
        ("xorshift128", [0] * 127, "invalid"),
        ("xorshift128", [0] * 129, "invalid"),
        ("xorshift32", [0] * 7 + [2] + [0] * 24, "invalid"),
        ("xorshift128", [0] * 91 + [255] + [0] * 36, "invalid"),
        ("unknown", [], "invalid"),
    ]
    write_cases(args.out_dir, "case_00_fixed_and_invalid", fixed)

    basis = [case32(1 << bit) for bit in range(32)]
    for bit in range(128):
        state = [0, 0, 0, 0]
        state[bit // 32] = 1 << (bit % 32)
        basis.append(case128(state))
    write_cases(args.out_dir, "case_01_basis", basis)

    rng = random.Random(2026072203)
    random_cases: list[Case] = []
    for iteration in range(2048):
        state32 = rng.getrandbits(32)
        for _ in range(iteration % 97):
            state32 = next32(state32)
        random_cases.append(case32(state32))

        state128 = [rng.getrandbits(32) for _ in range(4)]
        for _ in range(iteration % 131):
            next128(state128)
        random_cases.append(case128(state128))
    write_cases(args.out_dir, "case_02_random_and_skipped", random_cases)


if __name__ == "__main__":
    main()
