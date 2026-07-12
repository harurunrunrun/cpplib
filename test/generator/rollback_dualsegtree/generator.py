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

    rng = random.Random(2026071307)
    n = 45
    initial = [rng.randrange(20) for _ in range(n)]
    states = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(650):
        typ = rng.randrange(8)
        if typ < 2:
            k = rng.randrange(n)
            value = rng.randrange(-50, 51)
            next_values = states[-1][:]
            next_values[k] = value
            states.append(next_values)
            commands.append(f"SET {k} {value}")
            outputs.append(str(len(states) - 1))
        elif typ < 4:
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            a = rng.choice([-1, 0, 1])
            b = rng.randrange(-10, 11)
            next_values = states[-1][:]
            for k in range(left, right):
                next_values[k] = a * next_values[k] + b
            states.append(next_values)
            commands.append(f"APPLY {left} {right} {a} {b}")
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