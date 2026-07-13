#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


MASK32 = (1 << 32) - 1
MASK64 = (1 << 64) - 1


def write_case(out_dir: Path, index: int, data: str, output: list[str]) -> None:
    (out_dir / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(
        "".join(f"{line}\n" for line in output),
        encoding="utf-8",
    )


def xorshift32(value: int) -> int:
    value ^= value << 13 & MASK32
    value ^= value >> 17
    value ^= value << 5 & MASK32
    return value & MASK32


def xorshift64(value: int) -> int:
    value ^= value << 13 & MASK64
    value ^= value >> 7
    value ^= value << 17 & MASK64
    return value & MASK64


def xorshift128(state: list[int]) -> int:
    temporary = (state[0] ^ (state[0] << 11 & MASK32)) & MASK32
    state[0], state[1], state[2] = state[1], state[2], state[3]
    state[3] = (
        state[3] ^ (state[3] >> 19) ^ temporary ^ (temporary >> 8)
    ) & MASK32
    return state[3]


def add_xorshift_case(
    out_dir: Path,
    index: int,
    seed32: int,
    seed64: int,
    seed128: tuple[int, int, int, int],
    count: int,
) -> None:
    state32 = seed32
    state64 = seed64
    state128 = list(seed128)
    state32 = xorshift32(state32)
    state64 = xorshift64(state64)
    for _ in range(4):
        xorshift128(state128)
    output = [
        str(seed32),
        str(seed64),
        " ".join(map(str, seed128)),
    ]
    for _ in range(count):
        state32 = xorshift32(state32)
        state64 = xorshift64(state64)
        value128 = xorshift128(state128)
        output.append(f"{state32} {state64} {value128}")
    data = (
        "xorshift\n"
        f"{seed32} {seed64} {' '.join(map(str, seed128))} {count}\n"
    )
    write_case(out_dir, index, data, output)


def lcg_sequence(seed: int, multiplier: int, increment: int,
                 modulus: int, count: int) -> list[int]:
    state = seed if modulus == 0 else seed % modulus
    if increment == 0 and state == 0:
        state = 1
    mask = MASK64 if modulus == 0 else None
    result: list[int] = []
    for _ in range(count):
        state = state * multiplier + increment
        state = state & mask if mask is not None else state % modulus
        result.append(state)
    return result


def add_lcg_case(
    out_dir: Path,
    index: int,
    seed32: int,
    seed64: int,
    count: int,
) -> None:
    minstd = lcg_sequence(seed32, 16807, 0, 2147483647, count + 1)
    wrap = lcg_sequence(
        seed64,
        6364136223846793005,
        1442695040888963407,
        0,
        count + 1,
    )
    output = [
        str(seed32 % 2147483647 or 1),
        str(seed64),
        "16807 0",
    ]
    output.extend(
        f"{minstd[position]} {wrap[position]}"
        for position in range(1, count + 1)
    )
    write_case(
        out_dir,
        index,
        f"lcg\n{seed32} {seed64} {count}\n",
        output,
    )


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
        self.n = state_size
        self.m = shift_size
        self.r = mask_bits
        self.a = xor_mask
        self.u = temper_u
        self.d = temper_d
        self.s = temper_s
        self.b = temper_b
        self.t = temper_t
        self.c = temper_c
        self.l = temper_l
        self.state = [seed & self.mask]
        for index in range(1, self.n):
            previous = self.state[-1]
            self.state.append(
                (
                    initialization_multiplier
                    * (previous ^ (previous >> (word_size - 2)))
                    + index
                )
                & self.mask
            )
        self.index = self.n

    def twist(self) -> None:
        lower_mask = (1 << self.r) - 1
        upper_mask = self.mask ^ lower_mask
        for index in range(self.n):
            combined = (
                (self.state[index] & upper_mask)
                | (self.state[(index + 1) % self.n] & lower_mask)
            )
            value = self.state[(index + self.m) % self.n] ^ (combined >> 1)
            if combined & 1:
                value ^= self.a
            self.state[index] = value & self.mask
        self.index = 0

    def next(self) -> int:
        if self.index == self.n:
            self.twist()
        value = self.state[self.index]
        self.index += 1
        value ^= (value >> self.u) & self.d
        value ^= (value << self.s) & self.b
        value ^= (value << self.t) & self.c
        value ^= value >> self.l
        return value & self.mask


def mt32(seed: int) -> MersenneTwister:
    return MersenneTwister(
        seed, 32, 624, 397, 31, 0x9908B0DF,
        11, 0xFFFFFFFF, 7, 0x9D2C5680,
        15, 0xEFC60000, 18, 1812433253,
    )


def mt64(seed: int) -> MersenneTwister:
    return MersenneTwister(
        seed, 64, 312, 156, 31, 0xB5026F5AA96619E9,
        29, 0x5555555555555555, 17, 0x71D67FFFEDA60000,
        37, 0xFFF7EEE000000000, 43, 6364136223846793005,
    )


def add_mt_case(
    out_dir: Path,
    index: int,
    mode: str,
    seed: int,
    skip: int,
    count: int,
) -> None:
    engine = mt32(seed) if mode == "mt32" else mt64(seed)
    observed_count = 640 if mode == "mt32" else 320
    for _ in range(skip + observed_count):
        engine.next()
    output = [str(engine.next()) for _ in range(count)]
    write_case(
        out_dir,
        index,
        f"{mode}\n{seed} {skip} {count}\n",
        output,
    )


class SubtractWithCarry:
    def __init__(self, seed: int, word_size: int,
                 short_lag: int, long_lag: int) -> None:
        self.w = word_size
        self.s = short_lag
        self.r = long_lag
        self.modulus = 1 << word_size
        lcg_modulus = 2147483563
        lcg_state = (
            19780503 if seed == 0 else seed % lcg_modulus
        )
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
        short_value = self.state[(self.head + self.r - self.s) % self.r]
        long_value = self.state[self.head]
        carry = 1 if self.carry else 0
        value = (short_value - long_value - carry) % self.modulus
        self.carry = short_value < long_value + carry
        self.state[self.head] = value
        self.head = (self.head + 1) % self.r
        return value


def add_swc_case(
    out_dir: Path,
    index: int,
    mode: str,
    seed: int,
    skip: int,
    count: int,
) -> None:
    if mode == "swc24":
        engine = SubtractWithCarry(seed, 24, 10, 24)
        observed_count = 25
    else:
        engine = SubtractWithCarry(seed, 48, 5, 12)
        observed_count = 13
    for _ in range(skip + observed_count):
        engine.next()
    output = [str(seed)]
    output.extend(str(engine.next()) for _ in range(count))
    first = seed - 20
    last = seed + 21
    write_case(
        out_dir,
        index,
        f"{mode}\n{seed} {skip} {first} {last} {count}\n",
        output,
    )


class Philox:
    def __init__(self, seed: int, word_size: int,
                 multipliers: tuple[int, int],
                 round_constants: tuple[int, int]) -> None:
        self.w = word_size
        self.mask = (1 << word_size) - 1
        self.multipliers = multipliers
        self.round_constants = round_constants
        self.key = [seed & self.mask, 0]
        self.counter = [0, 0, 0, 0]
        self.buffer = [0, 0, 0, 0]
        self.index = 3

    def block(self) -> list[int]:
        current = self.counter.copy()
        for round_index in range(10):
            permuted = [current[2], current[1], current[0], current[3]]
            following = [0, 0, 0, 0]
            for index in range(2):
                product = (
                    permuted[2 * index] * self.multipliers[index]
                )
                low = product & self.mask
                high = product >> self.w & self.mask
                round_key = (
                    self.key[index]
                    + round_index * self.round_constants[index]
                ) & self.mask
                following[2 * index] = (
                    high ^ round_key ^ permuted[2 * index + 1]
                ) & self.mask
                following[2 * index + 1] = low
            current = following
        return current

    def increment(self) -> None:
        carry = 1
        for index in range(4):
            total = self.counter[index] + carry
            self.counter[index] = total & self.mask
            carry = total >> self.w

    def next(self) -> int:
        self.index += 1
        if self.index == 4:
            self.buffer = self.block()
            self.increment()
            self.index = 0
        return self.buffer[self.index]


def philox32(seed: int) -> Philox:
    return Philox(
        seed,
        32,
        (0xCD9E8D57, 0xD2511F53),
        (0x9E3779B9, 0xBB67AE85),
    )


def philox64(seed: int) -> Philox:
    return Philox(
        seed,
        64,
        (0xCA5A826395121157, 0xD2E7470EE14C6C93),
        (0x9E3779B97F4A7C15, 0xBB67AE8584CAA73B),
    )


def add_philox_case(
    out_dir: Path,
    index: int,
    mode: str,
    seed: int,
    skip: int,
    count: int,
) -> None:
    engine = philox32(seed) if mode == "philox32" else philox64(seed)
    for _ in range(skip + 8):
        engine.next()
    output = ["1", str(seed)]
    output.extend(str(engine.next()) for _ in range(count))
    first = seed - 20
    last = seed + 21
    write_case(
        out_dir,
        index,
        f"{mode}\n{seed} {skip} {first} {last} {count}\n",
        output,
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    case = 0
    add_xorshift_case(
        args.out_dir, case,
        2463534242, 88172645463325252,
        (123456789, 362436069, 521288629, 88675123), 16,
    )
    case += 1
    add_xorshift_case(
        args.out_dir, case,
        1, MASK64,
        (1, 0, MASK32, 0x80000000), 16,
    )
    case += 1

    add_lcg_case(args.out_dir, case, 1, 123456789, 20)
    case += 1
    add_lcg_case(args.out_dir, case, 2147483646, MASK64, 20)
    case += 1

    for mode, state_size, skips in (
        ("mt32", 624, (0, 1, 623, 624, 625, 10007)),
        ("mt64", 312, (0, 1, 311, 312, 313, 10007)),
    ):
        for offset, skip in enumerate(skips):
            seed = (5489 + 1000003 * offset) & (
                MASK32 if mode == "mt32" else MASK64
            )
            add_mt_case(args.out_dir, case, mode, seed, skip, 24)
            case += 1

    for mode, seeds_and_skips in (
        ("swc24", ((123, 0), (456, 23), (789, 1011))),
        ("swc48", ((321, 0), (654, 11), (987, 1009))),
    ):
        for seed, skip in seeds_and_skips:
            add_swc_case(args.out_dir, case, mode, seed, skip, 32)
            case += 1

    for mode, seeds_and_skips in (
        ("philox32", ((123, 0), (456, 3), (789, 10001))),
        ("philox64", ((321, 0), (654, 5), (987, 10003))),
    ):
        for seed, skip in seeds_and_skips:
            add_philox_case(args.out_dir, case, mode, seed, skip, 32)
            case += 1


if __name__ == "__main__":
    main()
