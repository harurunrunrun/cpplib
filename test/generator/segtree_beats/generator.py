#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071309)
    n = 79
    initial = [rng.randrange(-200, 201) for _ in range(n)]
    values = initial[:]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(900):
        typ = rng.randrange(10)
        if typ < 3:
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            value = rng.randrange(-250, 251)
            if typ == 0:
                for k in range(left, right):
                    values[k] = min(values[k], value)
                commands.append(f"CHMIN {left} {right} {value}")
            elif typ == 1:
                for k in range(left, right):
                    values[k] = max(values[k], value)
                commands.append(f"CHMAX {left} {right} {value}")
            else:
                for k in range(left, right):
                    values[k] += value
                commands.append(f"ADD {left} {right} {value}")
        elif typ == 3:
            k = rng.randrange(n)
            value = rng.randrange(-250, 251)
            values[k] = value
            commands.append(f"SET {k} {value}")
        elif typ == 4:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(values[k]))
        elif typ == 5:
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            commands.append(f"SUM {left} {right}")
            outputs.append(str(sum(values[left:right])))
        elif typ == 6:
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            commands.append(f"MIN {left} {right}")
            outputs.append(str(min(values[left:right])))
        elif typ == 7:
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            commands.append(f"MAX {left} {right}")
            outputs.append(str(max(values[left:right])))
        else:
            commands.append("ALL")
            outputs.append(f"{sum(values)} {min(values)} {max(values)}")

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial))
        + "\n"
        + "\n".join(commands)
        + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()