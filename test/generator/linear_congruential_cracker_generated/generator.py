#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


MASK64 = (1 << 64) - 1
PRIME_MODULUS = 18446744073709551557
MULTIPLIER = 6364136223846793005
INCREMENT = 1442695040888963407


def next_value(state: int, multiplier: int, increment: int,
               modulus: int) -> int:
    value = state * multiplier + increment
    return value & MASK64 if modulus == 0 else value % modulus


def sequence(seed: int, multiplier: int, increment: int,
             modulus: int, count: int) -> list[int]:
    state = seed if modulus == 0 else seed % modulus
    result: list[int] = []
    for _ in range(count):
        state = next_value(state, multiplier, increment, modulus)
        result.append(state)
    return result


def write_case(directory: Path, name: str,
               input_text: str, output_text: str) -> None:
    (directory / f"{name}.in").write_text(input_text, encoding="utf-8")
    (directory / f"{name}.out").write_text(output_text, encoding="utf-8")


def add_known_case(directory: Path, name: str, mode: str,
                   seed: int, skip: int, count: int) -> None:
    modulus = PRIME_MODULUS if mode == "prime" else 0
    state = seed if modulus == 0 else seed % modulus
    for _ in range(skip):
        state = next_value(state, MULTIPLIER, INCREMENT, modulus)
    recovered = state
    state = next_value(state, MULTIPLIER, INCREMENT, modulus)
    output = [f"{recovered} {MULTIPLIER} {INCREMENT}"]
    for _ in range(count):
        state = next_value(state, MULTIPLIER, INCREMENT, modulus)
        output.append(str(state))
    write_case(
        directory,
        name,
        f"{mode}\n{seed} {skip} {count}\n",
        "\n".join(output) + "\n",
    )


def add_parameter_cases(directory: Path) -> None:
    observations = sequence(
        0x123456789ABCDEF0,
        MULTIPLIER,
        INCREMENT,
        PRIME_MODULUS,
        8,
    )
    encoded = " ".join(map(str, observations))
    write_case(
        directory,
        "prime_parameters_valid",
        f"prime_parameters\n{encoded}\n",
        f"{MULTIPLIER} {INCREMENT}\n",
    )

    corrupt = observations.copy()
    corrupt[-1] ^= 1
    write_case(
        directory,
        "prime_parameters_corrupt",
        f"prime_parameters\n{' '.join(map(str, corrupt))}\n",
        "none\n",
    )

    out_of_range = observations.copy()
    out_of_range[4] = PRIME_MODULUS
    write_case(
        directory,
        "prime_parameters_out_of_range",
        f"prime_parameters\n{' '.join(map(str, out_of_range))}\n",
        "none\n",
    )


def invariant_gcd(outputs: list[int]) -> int:
    differences = [
        outputs[index + 1] - outputs[index]
        for index in range(len(outputs) - 1)
    ]
    result = 0
    for index in range(len(differences) - 2):
        value = abs(
            differences[index] * differences[index + 2]
            - differences[index + 1] ** 2
        )
        result = math.gcd(result, value)
    return result


def add_unknown_valid(directory: Path, name: str, modulus: int,
                      multiplier: int, increment: int, seed: int) -> None:
    outputs = sequence(seed, multiplier, increment, modulus, 32)
    assert invariant_gcd(outputs) == modulus
    assert math.gcd(
        (outputs[1] - outputs[0]) % modulus,
        modulus,
    ) == 1
    following = next_value(outputs[-1], multiplier, increment, modulus)
    write_case(
        directory,
        name,
        f"unknown\n{len(outputs)} {' '.join(map(str, outputs))}\n",
        f"{modulus} {multiplier} {increment}\n{following}\n",
    )


def add_unknown_cases(directory: Path) -> None:
    candidates = [
        (1000000007, 48271, 1234567, 7654321),
        (4294967291, 279470273, 1013904223, 3141592653),
        (
            PRIME_MODULUS,
            MULTIPLIER,
            INCREMENT,
            0xD1B54A32D192ED03,
        ),
    ]
    for index, values in enumerate(candidates):
        add_unknown_valid(directory, f"unknown_valid_{index}", *values)

    invalid_sequences = [
        [1, 1, 1, 1, 1, 1],
        [1, 2, 3],
        [1, 2, 4, 8, 17, 34],
        [MASK64, 0, MASK64, 0, MASK64 - 1],
    ]
    for index, outputs in enumerate(invalid_sequences):
        write_case(
            directory,
            f"unknown_none_{index}",
            f"unknown\n{len(outputs)} {' '.join(map(str, outputs))}\n",
            "none\n",
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    fixed_seeds = [
        0,
        1,
        2,
        PRIME_MODULUS - 1,
        MASK64,
        0x8000000000000000,
    ]
    for index, seed in enumerate(fixed_seeds):
        add_known_case(
            args.out_dir,
            f"prime_fixed_{index}",
            "prime",
            seed,
            index * 17,
            8,
        )
        add_known_case(
            args.out_dir,
            f"wrap_fixed_{index}",
            "wrap",
            seed,
            index * 19,
            8,
        )

    rng = random.Random(0x4C43475F43524143)
    for index in range(24):
        add_known_case(
            args.out_dir,
            f"prime_random_{index:02d}",
            "prime",
            rng.randrange(1 << 64),
            rng.randrange(512),
            rng.randrange(1, 9),
        )
        add_known_case(
            args.out_dir,
            f"wrap_random_{index:02d}",
            "wrap",
            rng.randrange(1 << 64),
            rng.randrange(512),
            rng.randrange(1, 9),
        )

    for index, latest in enumerate((0, 1, 5, 11)):
        state = latest
        expected = ["none"]
        for _ in range(8):
            state = next_value(state, 6, 5, 12)
            expected.append(str(state))
        write_case(
            args.out_dir,
            f"noninvertible_{index}",
            f"noninvertible\n{latest} 8\n",
            "\n".join(expected) + "\n",
        )

    for index, invalid in enumerate(
        (PRIME_MODULUS, PRIME_MODULUS + 1, MASK64)
    ):
        write_case(
            args.out_dir,
            f"invalid_cracker_{index}",
            f"invalid_cracker\n{invalid}\n",
            "invalid\n",
        )

    add_parameter_cases(args.out_dir)
    add_unknown_cases(args.out_dir)


if __name__ == "__main__":
    main()
