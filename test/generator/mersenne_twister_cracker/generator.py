#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


class MersenneTwister:
    def __init__(
        self,
        seed: int,
        word_size: int,
        state_size: int,
        shift_size: int,
        mask_bits: int,
        xor_mask: int,
        temper_u: int,
        temper_d: int,
        temper_s: int,
        temper_b: int,
        temper_t: int,
        temper_c: int,
        temper_l: int,
        initialization_multiplier: int,
    ) -> None:
        self.word_size = word_size
        self.mask = (1 << word_size) - 1
        self.state_size = state_size
        self.shift_size = shift_size
        self.mask_bits = mask_bits
        self.xor_mask = xor_mask
        self.temper_u = temper_u
        self.temper_d = temper_d
        self.temper_s = temper_s
        self.temper_b = temper_b
        self.temper_t = temper_t
        self.temper_c = temper_c
        self.temper_l = temper_l
        self.state = [seed & self.mask]
        for index in range(1, state_size):
            previous = self.state[-1]
            self.state.append(
                (
                    initialization_multiplier
                    * (previous ^ (previous >> (word_size - 2)))
                    + index
                )
                & self.mask
            )
        self.index = state_size

    def twist(self) -> None:
        low_mask = (1 << self.mask_bits) - 1
        high_mask = self.mask ^ low_mask
        for index in range(self.state_size):
            combined = (
                (self.state[index] & high_mask)
                | (self.state[(index + 1) % self.state_size] & low_mask)
            )
            value = (
                self.state[(index + self.shift_size) % self.state_size]
                ^ (combined >> 1)
            )
            if combined & 1:
                value ^= self.xor_mask
            self.state[index] = value & self.mask
        self.index = 0

    def next(self) -> int:
        if self.index == self.state_size:
            self.twist()
        value = self.state[self.index]
        self.index += 1
        value ^= (value >> self.temper_u) & self.temper_d
        value ^= (value << self.temper_s) & self.temper_b
        value ^= (value << self.temper_t) & self.temper_c
        value ^= value >> self.temper_l
        return value & self.mask


def mt32(seed: int) -> MersenneTwister:
    return MersenneTwister(
        seed,
        32,
        624,
        397,
        31,
        0x9908B0DF,
        11,
        0xFFFFFFFF,
        7,
        0x9D2C5680,
        15,
        0xEFC60000,
        18,
        1812433253,
    )


def mt64(seed: int) -> MersenneTwister:
    return MersenneTwister(
        seed,
        64,
        312,
        156,
        31,
        0xB5026F5AA96619E9,
        29,
        0x5555555555555555,
        17,
        0x71D67FFFEDA60000,
        37,
        0xFFF7EEE000000000,
        43,
        6364136223846793005,
    )


def write_case(
    output_directory: Path,
    index: int,
    mode: str,
    seed: int,
    skip: int,
    observation_count: int,
    *,
    corrupt: bool = False,
) -> None:
    engine = mt32(seed) if mode.startswith("mt32") else mt64(seed)
    for _ in range(skip):
        engine.next()
    observations = [engine.next() for _ in range(observation_count)]
    prediction_count = 24
    if corrupt:
        observations[-1] ^= 1
        expected = "invalid\n"
    else:
        expected = "".join(
            f"{engine.next()}\n" for _ in range(prediction_count)
        )
    input_text = (
        f"{mode}\n"
        + " ".join(map(str, observations))
        + f"\n{prediction_count}\n"
    )
    (output_directory / f"case_{index:02d}.in").write_text(
        input_text,
        encoding="utf-8",
    )
    (output_directory / f"case_{index:02d}.out").write_text(
        expected,
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    output_directory = parser.parse_args().out_dir
    output_directory.mkdir(parents=True, exist_ok=True)

    cases = [
        ("mt32_min", 0, 0, 624, False),
        ("mt32_min", 0xFFFFFFFF, 1, 624, False),
        ("mt32_min", 5489, 623, 624, False),
        ("mt32_min", 123456789, 624, 624, False),
        ("mt32_extra", 987654321, 625, 640, False),
        ("mt32_extra", 17, 10007, 640, False),
        ("mt32_corrupt", 29, 0, 625, True),
        ("mt32_corrupt", 31, 10007, 625, True),
        ("mt64_min", 0, 0, 312, False),
        ("mt64_min", 0xFFFFFFFFFFFFFFFF, 1, 312, False),
        ("mt64_min", 5489, 311, 312, False),
        ("mt64_min", 1234567890123456789, 312, 312, False),
        ("mt64_extra", 987654321, 313, 320, False),
        ("mt64_extra", 43, 10007, 320, False),
        ("mt64_corrupt", 47, 0, 313, True),
        ("mt64_corrupt", 53, 10007, 313, True),
    ]
    for index, (mode, seed, skip, count, corrupt) in enumerate(cases):
        write_case(
            output_directory,
            index,
            mode,
            seed,
            skip,
            count,
            corrupt=corrupt,
        )


if __name__ == "__main__":
    main()
