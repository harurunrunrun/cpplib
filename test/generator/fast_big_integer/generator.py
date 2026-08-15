#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path


if hasattr(sys, "set_int_max_str_digits"):
    sys.set_int_max_str_digits(0)


def trunc_divmod(left: int, right: int) -> tuple[int, int]:
    quotient = abs(left) // abs(right)
    if (left < 0) != (right < 0):
        quotient = -quotient
    return quotient, left - quotient * right


def random_integer(rng: random.Random, bits: int) -> int:
    value = rng.getrandbits(bits)
    return -value if rng.randrange(2) else value


def append_binary(
    commands: list[str],
    output: list[str],
    operation: str,
    left: int,
    right: int,
) -> None:
    commands.append(f"{operation} {left} {right}")
    if operation == "ADD":
        output.append(str(left + right))
    elif operation == "SUB":
        output.append(str(left - right))
    elif operation == "MUL":
        output.append(str(left * right))
    elif operation == "CMP":
        output.append(str(-1 if left < right else int(left > right)))
    else:
        quotient, remainder = trunc_divmod(left, right)
        if operation == "DIV":
            output.append(str(quotient))
        elif operation == "MOD":
            output.append(str(remainder))
        else:
            output.append(f"{quotient} {remainder}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026081519)
    commands: list[str] = []
    output: list[str] = []

    for text in ["0", "+000", "-0", "000001", "-000001"]:
        commands.append(f"ECHO {text}")
        output.append(str(int(text)))

    edges = [
        0,
        1,
        -1,
        10**9 - 1,
        10**9,
        -(10**9),
        (1 << 63) - 1,
        -(1 << 63),
        10**72 - 1,
    ]
    for left in edges:
        for right in edges:
            for operation in ("ADD", "SUB", "MUL", "CMP"):
                append_binary(commands, output, operation, left, right)
            if right != 0:
                for operation in ("DIV", "MOD", "DIVMOD"):
                    append_binary(commands, output, operation, left, right)

    # Exercise both sides of the 64-limb division and 128-limb NTT cutovers.
    for limb_count in (63, 64, 65, 127, 128, 129, 257, 300):
        digits = limb_count * 9
        left = 10 ** (digits - 1) + rng.randrange(10**18)
        right = 10 ** (digits - 2) + rng.randrange(10**18)
        append_binary(commands, output, "MUL", left, right)

        divisor = 10 ** (digits - 1) + rng.randrange(10**18)
        quotient = 10 ** (digits - 1) + rng.randrange(10**18)
        for remainder in (0, 1, divisor - 1):
            dividend = divisor * quotient + remainder
            append_binary(commands, output, "DIVMOD", dividend, divisor)
            append_binary(commands, output, "DIVMOD", -dividend, divisor)
            append_binary(commands, output, "DIVMOD", dividend, -divisor)

    for _ in range(600):
        bits = rng.randrange(1, 4097)
        left = random_integer(rng, bits)
        right = random_integer(rng, rng.randrange(1, bits + 1))
        operation = rng.choice(("ADD", "SUB", "MUL", "DIV", "MOD", "DIVMOD", "CMP"))
        if operation in ("DIV", "MOD", "DIVMOD") and right == 0:
            right = 1
        append_binary(commands, output, operation, left, right)

    (directory / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (directory / "case_00.out").write_text(
        "\n".join(output) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
