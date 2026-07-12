#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

EMPTY = ((1 << 63) - 1) // 4
LONG_LONG_MIN = -(1 << 63)
LONG_LONG_MAX = (1 << 63) - 1


def clamp(value: int) -> int:
    return max(LONG_LONG_MIN, min(LONG_LONG_MAX, value))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    lines: list[tuple[int, int]] = []
    commands = ["QUERY 0"]
    outputs = [str(EMPTY)]
    add_count = 0
    for _ in range(1100):
        if add_count < 300 and (not lines or rng.randrange(10) < 3):
            a = rng.randrange(-10**15, 10**15 + 1)
            b = rng.randrange(-10**18, 10**18 + 1)
            commands.append(f"ADD {a} {b}")
            lines.append((a, b))
            add_count += 1
        else:
            x = rng.randrange(-1000000, 1000001)
            commands.append(f"QUERY {x}")
            outputs.append(str(clamp(min(a * x + b for a, b in lines))))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

    fixed_cases = [
        (["QUERY 0", "ADD 0 3000000000000000000", "QUERY 0"],
         [str(EMPTY), "3000000000000000000"]),
        (["ADD 9000000000000000000 1000000000000000000", "QUERY 1000000"],
         [str(LONG_LONG_MAX)]),
        (["ADD -9000000000000000000 -1000000000000000000", "QUERY 1000000"],
         [str(LONG_LONG_MIN)]),
    ]
    for index, (fixed_commands, fixed_outputs) in enumerate(fixed_cases, start=1):
        (out_dir / f"case_{index:02d}.in").write_text(
            str(len(fixed_commands)) + "\n" + "\n".join(fixed_commands) + "\n",
            encoding="utf-8")
        (out_dir / f"case_{index:02d}.out").write_text(
            "\n".join(fixed_outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
