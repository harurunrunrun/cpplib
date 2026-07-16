#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def bit_count_inclusive(value: int, bit: int) -> int:
    length = value + 1
    half = 1 << bit
    cycle = half << 1
    return length // cycle * half + max(0, length % cycle - half)


def total_inclusive(value: int) -> int:
    return sum(bit_count_inclusive(value, bit) for bit in range(64))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071610)
    commands: list[str] = []
    output: list[str] = []
    values = [0, 1, 2, 3, (1 << 63) - 1, 1 << 63, (1 << 64) - 1]
    for value in values:
        commands.append(f"TOTAL_INC {value}")
        output.append(str(total_inclusive(value)))
        commands.append(f"TOTAL_HALF {value}")
        output.append(str(0 if value == 0 else total_inclusive(value - 1)))
        for bit in (0, 1, 31, 32, 63):
            commands.append(f"BIT_INC {value} {bit}")
            output.append(str(bit_count_inclusive(value, bit)))
            commands.append(f"BIT_HALF {value} {bit}")
            output.append(str(0 if value == 0 else bit_count_inclusive(value - 1, bit)))
    for _ in range(10000):
        value = rng.randrange(1 << 64)
        operation = rng.randrange(4)
        if operation == 0:
            commands.append(f"TOTAL_INC {value}")
            output.append(str(total_inclusive(value)))
        elif operation == 1:
            commands.append(f"TOTAL_HALF {value}")
            output.append(str(0 if value == 0 else total_inclusive(value - 1)))
        else:
            bit = rng.randrange(64)
            if operation == 2:
                commands.append(f"BIT_INC {value} {bit}")
                output.append(str(bit_count_inclusive(value, bit)))
            else:
                commands.append(f"BIT_HALF {value} {bit}")
                output.append(str(0 if value == 0 else bit_count_inclusive(value - 1, bit)))

    (directory / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (directory / "case_00.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()

