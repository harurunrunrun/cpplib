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

    rng = random.Random(20261116)
    n = 24
    initial = [rng.randrange(-50, 51) for _ in range(n)]
    states = [initial[:]]
    current = initial[:]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(350):
        typ = rng.randrange(8)
        if typ < 3:
            k = rng.randrange(n)
            x = rng.randrange(-1000, 1001)
            current[k] = x
            states.append(current[:])
            commands.append(f"SET {k} {x}")
        elif typ == 3 and len(states) > 1:
            commands.append("UNDO")
            states.pop()
            current = states[-1][:]
        elif typ == 4:
            snap = rng.randrange(len(states))
            commands.append(f"ROLLBACK {snap}")
            states = states[:snap + 1]
            current = states[-1][:]
        elif typ < 7:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(current[k]))
        else:
            commands.append("HISTORY")
            outputs.append(str(len(states) - 1))

    commands.append("SNAP")
    outputs.append(str(len(states) - 1))
    commands.append("CAN")
    outputs.append(str(int(len(states) > 1)))
    commands.append("SIZE")
    outputs.append(str(n))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
