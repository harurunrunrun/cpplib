#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def trunc_divmod(left: int, right: int) -> tuple[int, int]:
    quotient = abs(left) // abs(right)
    if (left < 0) != (right < 0):
        quotient = -quotient
    return quotient, left - quotient * right


def random_integer(rng: random.Random, bits: int) -> int:
    value = rng.getrandbits(bits)
    if rng.randrange(2):
        value = -value
    return value


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071616)
    commands: list[str] = []
    output: list[str] = []

    for text in ["0", "+000", "-0", "000001", "-000001",
                 str(1 << 4096), str(-(1 << 4096))]:
        commands.append(f"ECHO {text}")
        output.append(str(int(text)))

    edges = [0, 1, -1, (1 << 63) - 1, -(1 << 63),
             1 << 255, -(1 << 255), (1 << 1024) - 1]
    for left in edges:
        for right in edges:
            commands.append(f"ADD {left} {right}")
            output.append(str(left + right))
            commands.append(f"SUB {left} {right}")
            output.append(str(left - right))
            commands.append(f"MUL {left} {right}")
            output.append(str(left * right))
            commands.append(f"CMP {left} {right}")
            output.append(str(-1 if left < right else int(left > right)))
            if right != 0:
                quotient, remainder = trunc_divmod(left, right)
                commands.append(f"DIV {left} {right}")
                output.append(str(quotient))
                commands.append(f"MOD {left} {right}")
                output.append(str(remainder))

    for _ in range(500):
        bits = rng.randrange(1, 2049)
        left = random_integer(rng, bits)
        right = random_integer(rng, rng.randrange(1, bits + 1))
        operation = rng.randrange(8)
        if operation == 0:
            commands.append(f"ADD {left} {right}")
            output.append(str(left + right))
        elif operation == 1:
            commands.append(f"SUB {left} {right}")
            output.append(str(left - right))
        elif operation == 2:
            commands.append(f"MUL {left} {right}")
            output.append(str(left * right))
        elif operation == 3:
            commands.append(f"CMP {left} {right}")
            output.append(str(-1 if left < right else int(left > right)))
        elif operation in (4, 5):
            if right == 0:
                right = 1
            quotient, remainder = trunc_divmod(left, right)
            if operation == 4:
                commands.append(f"DIV {left} {right}")
                output.append(str(quotient))
            else:
                commands.append(f"MOD {left} {right}")
                output.append(str(remainder))
        else:
            shift = rng.randrange(0, 2049)
            if operation == 6:
                commands.append(f"SHL {left} {shift}")
                output.append(str(left << shift))
            else:
                commands.append(f"SHR {left} {shift}")
                output.append(str(left >> shift))

    (directory / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (directory / "case_00.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()

