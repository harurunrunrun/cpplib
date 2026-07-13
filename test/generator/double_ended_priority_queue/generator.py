#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    values = [rng.randrange(-100, 101) for _ in range(700)]
    values.extend([-7, -7, 0, 3, 3, 9])
    initial = list(values)
    values.sort()
    commands: list[str] = []
    output: list[str] = []

    for step in range(2500):
        kind = rng.randrange(12)
        if not values or kind < 4:
            value = rng.randrange(-100, 101)
            commands.append(f"PUSH {value}")
            bisect.insort(values, value)
        elif kind == 4:
            commands.append("MIN")
            output.append(str(values[0]))
        elif kind == 5:
            commands.append("MAX")
            output.append(str(values[-1]))
        elif kind == 6:
            commands.append("POPMIN")
            output.append(str(values.pop(0)))
        elif kind == 7:
            commands.append("POPMAX")
            output.append(str(values.pop()))
        elif kind == 8:
            commands.append("SIZE")
            output.append(str(len(values)))
        elif kind == 9:
            commands.append("EMPTY")
            output.append(str(int(not values)))
        elif kind == 10 and step > 100:
            commands.append("CLEAR")
            values.clear()
        else:
            commands.extend(["MIN", "MAX"])
            output.extend([str(values[0]), str(values[-1])])

    input_lines = [f"{len(initial)} {len(commands)}", " ".join(map(str, initial))]
    input_lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
