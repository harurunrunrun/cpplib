#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

if hasattr(sys, "set_int_max_str_digits"):
    sys.set_int_max_str_digits(0)

def hexadecimal(value: int) -> str:
    return ("-" if value < 0 else "") + format(abs(value), "x")


def truncating_divmod(value: int, divisor: int) -> tuple[int, int]:
    quotient = abs(value) // divisor
    if value < 0:
        quotient = -quotient
    return quotient, abs(value) % divisor


class Case:
    def __init__(self) -> None:
        self.commands: list[str] = []
        self.answers: list[str] = []

    def add(self, command: str, answer: object) -> None:
        self.commands.append(command)
        self.answers.append(str(answer))

    def meta(self, value: int) -> None:
        self.add(
            f"META {hexadecimal(value)}",
            f"{value} {abs(value).bit_length()} {int(value < 0)} {abs(value)}",
        )

    def binary(self, operation: str, left: int, right: int) -> None:
        if operation == "ADD":
            answer = left + right
        elif operation == "SUB":
            answer = left - right
        elif operation == "MUL":
            answer = left * right
        elif operation == "CMP":
            answer = (left > right) - (left < right)
        else:
            raise AssertionError(operation)
        self.add(
            f"{operation} {hexadecimal(left)} {hexadecimal(right)}",
            answer,
        )

    def shift(self, operation: str, value: int, amount: int) -> None:
        answer = value << amount if operation == "SHL" else value >> amount
        self.add(f"{operation} {hexadecimal(value)} {amount}", answer)

    def divide(self, value: int, divisor: int) -> None:
        quotient, remainder = truncating_divmod(value, divisor)
        self.add(
            f"DIV {hexadecimal(value)} {divisor}",
            f"{quotient} {remainder}",
        )

    def multiply_power(self, bits: int) -> None:
        modulus = 1_000_000_007
        remainder = (pow(2, bits * 2, modulus) - 1) % modulus
        self.add(
            f"MUL_POWER {bits}",
            f"{bits * 2} {remainder}",
        )

    def decimal(self, value: int) -> None:
        self.add(f"DEC {hexadecimal(value)}", value)

    def decimal_power(self, bits: int, offset: int, negative: bool) -> None:
        value = (1 << bits) + offset
        if negative:
            value = -value
        self.add(f"DEC_POWER {bits} {offset} {int(negative)}", value)


def write_case(out_dir: Path, case_id: int, case: Case) -> None:
    (out_dir / f"case_{case_id:02d}.in").write_text(
        f"{len(case.commands)}\n" + "\n".join(case.commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{case_id:02d}.out").write_text(
        "\n".join(case.answers) + "\n",
        encoding="utf-8",
    )


def boundary_case() -> Case:
    case = Case()
    values = {0, 1, -1}
    for bits in (1, 2, 31, 32, 33, 63, 64, 65, 127, 128, 129, 191, 192,
                 255, 256, 257, 511, 512, 1024):
        power = 1 << bits
        for delta in (-1, 0, 1):
            values.add(power + delta)
            values.add(-(power + delta))
    ordered = sorted(values)
    for value in ordered:
        case.meta(value)
        case.add(f"NEG {hexadecimal(value)}", -value)
        case.add(f"ABS {hexadecimal(value)}", abs(value))
    for index, left in enumerate(ordered):
        right = ordered[(index * 37 + 11) % len(ordered)]
        for operation in ("ADD", "SUB", "MUL", "CMP"):
            case.binary(operation, left, right)
        for shift in (0, 1, 31, 32, 33, 127, 513):
            case.shift("SHL" if shift <= 33 else "SHR", left, shift)
    for value in ordered[::5]:
        for divisor in (1, 2, 3, 2**32 - 1, 2**32, 2**63,
                        2**64 - 1):
            case.divide(value, divisor)
        increment = 2**64 - 1
        case.add(
            f"ADD_SMALL {hexadecimal(value)} {increment}",
            value + increment,
        )
    return case


def random_integer(rng: random.Random, maximum_bits: int) -> int:
    bits = rng.randrange(maximum_bits + 1)
    value = rng.getrandbits(bits)
    return -value if rng.randrange(2) else value


def random_case(seed: int, count: int, maximum_bits: int) -> Case:
    rng = random.Random(seed)
    case = Case()
    divisors = [1, 2, 3, 2**32 - 1, 2**32, 2**63,
                2**64 - 1]
    for index in range(count):
        left = random_integer(rng, maximum_bits)
        right = random_integer(rng, maximum_bits)
        operation = ("ADD", "SUB", "MUL", "CMP")[index % 4]
        case.binary(operation, left, right)
        case.shift(
            "SHL" if index % 2 == 0 else "SHR",
            left,
            rng.randrange(maximum_bits * 2 + 65),
        )
        case.divide(left, rng.choice(divisors))
        if index % 7 == 0:
            case.meta(left)
        if index % 11 == 0:
            increment = rng.randrange(2**64)
            case.add(
                f"ADD_SMALL {hexadecimal(left)} {increment}",
                left + increment,
            )
    return case


def large_case() -> Case:
    rng = random.Random(2026071603)
    case = Case()
    for bits in (2048, 4096, 8192):
        left = rng.getrandbits(bits) | (1 << (bits - 1))
        right = -(rng.getrandbits(bits // 2) | 1)
        case.binary("MUL", left, right)
        case.binary("ADD", left, right)
        case.shift("SHR", -left, bits - 1)
        case.divide(left, 2**64 - 1)
    for value in (1, -1, (1 << 127) - 1, -(1 << 127)):
        case.shift("SHL", value, 32768)
        case.shift("SHR", value, 32768)
    case.multiply_power(1_048_576)
    case.meta((1 << 33001) + (1 << 16401) + 1)
    return case


def conversion_case() -> Case:
    case = Case()
    types = {
        "I8": (-2**7, 2**7 - 1),
        "U8": (0, 2**8 - 1),
        "I16": (-2**15, 2**15 - 1),
        "U16": (0, 2**16 - 1),
        "I32": (-2**31, 2**31 - 1),
        "U32": (0, 2**32 - 1),
        "I64": (-2**63, 2**63 - 1),
        "U64": (0, 2**64 - 1),
        "I128": (-2**127, 2**127 - 1),
        "U128": (0, 2**128 - 1),
        "BOOL": (0, 1),
    }
    for name, (minimum, maximum) in types.items():
        values = {
            minimum - 1, minimum, minimum + 1,
            maximum - 1, maximum, maximum + 1,
            0, 1, -1,
        }
        for value in sorted(values):
            if minimum <= value <= maximum:
                answer = f"OK {value}"
            else:
                answer = "OVERFLOW"
            case.add(f"CONVERT {name} {hexadecimal(value)}", answer)
    return case


def decimal_conversion_case() -> Case:
    case = Case()
    values = {0, 1, -1}
    for digits in (1, 2, 8, 9, 10, 17, 18, 19, 31, 32, 33,
                   63, 64, 65, 127, 128, 129, 255, 256, 257, 999, 1000):
        power = 10**digits
        for delta in (-1, 0, 1):
            values.add(power + delta)
            values.add(-(power + delta))
    for value in sorted(values):
        case.decimal(value)
    return case


def large_decimal_conversion_case() -> Case:
    case = Case()
    # The dense binary value 2^332192-1 has exactly 100000 decimal digits.
    case.decimal_power(332_192, -1, True)
    return case


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        boundary_case(),
        random_case(2026071601, 320, 512),
        random_case(2026071602, 80, 2048),
        large_case(),
        conversion_case(),
        decimal_conversion_case(),
        large_decimal_conversion_case(),
    ]
    for case_id, case in enumerate(cases):
        write_case(args.out_dir, case_id, case)


if __name__ == "__main__":
    main()
