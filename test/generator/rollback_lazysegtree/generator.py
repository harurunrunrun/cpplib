#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def max_right(values: list[int], left: int, limit: int) -> int:
    total = 0
    right = left
    while right < len(values) and total + values[right] <= limit:
        total += values[right]
        right += 1
    return right


def min_left(values: list[int], right: int, limit: int) -> int:
    total = 0
    left = right
    while left > 0 and total + values[left - 1] <= limit:
        left -= 1
        total += values[left]
    return left


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071308)
    n = 53
    initial = [rng.randrange(15) for _ in range(n)]
    states = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(650):
        typ = rng.randrange(12)
        if typ == 0:
            k = rng.randrange(n)
            value = rng.randrange(40)
            next_values = states[-1][:]
            next_values[k] = value
            states.append(next_values)
            commands.append(f"SET {k} {value}")
            outputs.append(str(len(states) - 1))
        elif typ == 1:
            k = rng.randrange(n)
            value = rng.randrange(8)
            next_values = states[-1][:]
            next_values[k] += value
            states.append(next_values)
            commands.append(f"ADD1 {k} {value}")
            outputs.append(str(len(states) - 1))
        elif typ < 4:
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            value = rng.randrange(8)
            next_values = states[-1][:]
            for k in range(left, right):
                next_values[k] += value
            states.append(next_values)
            commands.append(f"ADD {left} {right} {value}")
            outputs.append(str(len(states) - 1))
        elif typ == 4 and len(states) > 1:
            states.pop()
            commands.append("UNDO")
            outputs.append(str(len(states) - 1))
        elif typ == 5:
            snapshot = rng.randrange(len(states))
            states = states[: snapshot + 1]
            commands.append(f"ROLLBACK {snapshot}")
            outputs.append(str(snapshot))
        elif typ == 6:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(states[-1][k]))
        elif typ == 7:
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            commands.append(f"SUM {left} {right}")
            outputs.append(str(sum(states[-1][left:right])))
        elif typ == 8:
            commands.append("ALL")
            outputs.append(str(sum(states[-1])))
        elif typ == 9:
            left = rng.randrange(n + 1)
            limit = rng.randrange(sum(states[-1][left:]) + 10)
            commands.append(f"MR {left} {limit}")
            outputs.append(str(max_right(states[-1], left, limit)))
        elif typ == 10:
            right = rng.randrange(n + 1)
            limit = rng.randrange(sum(states[-1][:right]) + 10)
            commands.append(f"ML {right} {limit}")
            outputs.append(str(min_left(states[-1], right, limit)))
        else:
            commands.append("SNAP")
            outputs.append(str(len(states) - 1))

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