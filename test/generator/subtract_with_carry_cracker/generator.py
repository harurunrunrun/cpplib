#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


PARAMETERS = {
    "ranlux24_min": (24, 10, 24, 25),
    "ranlux24_extra": (24, 10, 24, 40),
    "ranlux48_min": (48, 5, 12, 13),
    "ranlux48_extra": (48, 5, 12, 29),
}


class SubtractWithCarry:
    def __init__(
        self,
        seed: int,
        word_size: int,
        short_lag: int,
        long_lag: int,
    ) -> None:
        self.word_size = word_size
        self.short_lag = short_lag
        self.long_lag = long_lag
        self.modulus = 1 << word_size

        lcg_modulus = 2147483563
        lcg_state = 19780503 if seed == 0 else seed % lcg_modulus
        if lcg_state == 0:
            lcg_state = 1

        pieces = (word_size + 31) // 32
        self.state: list[int] = []
        for _ in range(long_lag):
            value = 0
            for piece in range(pieces):
                lcg_state = lcg_state * 40014 % lcg_modulus
                value += lcg_state << (32 * piece)
            self.state.append(value % self.modulus)

        self.carry = self.state[-1] == 0
        self.head = 0

    def next(self) -> int:
        short_index = (
            self.head + self.long_lag - self.short_lag
        ) % self.long_lag
        short_value = self.state[short_index]
        long_value = self.state[self.head]
        carry_value = int(self.carry)
        value = (
            short_value - long_value - carry_value
        ) % self.modulus
        self.carry = short_value < long_value + carry_value
        self.state[self.head] = value
        self.head = (self.head + 1) % self.long_lag
        return value


Case = tuple[str, int, int, int]


def expected(case: Case) -> str:
    mode, seed, skip, prediction_count = case
    word_size, short_lag, long_lag, observation_count = PARAMETERS[mode]
    engine = SubtractWithCarry(seed, word_size, short_lag, long_lag)
    for _ in range(skip + observation_count):
        engine.next()
    return " ".join(str(engine.next()) for _ in range(prediction_count))


def write_case(output_directory: Path, name: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    input_lines.extend(
        f"{mode} {seed} {skip} {prediction_count}"
        for mode, seed, skip, prediction_count in cases
    )
    output_lines = [expected(case) for case in cases]
    (output_directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (output_directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    minimum_and_carry: list[Case] = [
        ("ranlux24_min", 0, 0, 64),
        ("ranlux24_min", 1, 19, 64),
        ("ranlux24_min", 123456, 257, 64),
        ("ranlux48_min", 321, 0, 64),
        ("ranlux48_min", 654, 11, 64),
        ("ranlux48_min", 2147483563, 127, 64),
    ]
    write_case(
        output_directory,
        "case_00_minimum_and_carry",
        minimum_and_carry,
    )

    extra_observations: list[Case] = [
        ("ranlux24_extra", 0, 0, 128),
        ("ranlux24_extra", 987654, 73, 128),
        ("ranlux24_extra", 4294967295, 1024, 128),
        ("ranlux48_extra", 0, 0, 128),
        ("ranlux48_extra", 123456789, 41, 128),
        ("ranlux48_extra", (1 << 64) - 1, 1024, 128),
    ]
    write_case(
        output_directory,
        "case_01_extra_observations",
        extra_observations,
    )

    rng = random.Random(20260714)
    random_cases: list[Case] = []
    modes = list(PARAMETERS)
    for index in range(512):
        mode = modes[index % len(modes)]
        seed_bits = 32 if mode.startswith("ranlux24") else 64
        random_cases.append(
            (
                mode,
                rng.getrandbits(seed_bits),
                rng.randrange(0, 4096),
                rng.randrange(1, 97),
            )
        )
    write_case(output_directory, "case_02_random", random_cases)


if __name__ == "__main__":
    main()
