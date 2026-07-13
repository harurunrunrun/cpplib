#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

LIMIT = 10**6


def rectangle_sum(values: dict[tuple[int, int], int], rb: int, cb: int,
                  re: int, ce: int) -> int:
    return sum(value for (row, col), value in values.items()
               if rb <= row < re and cb <= col < ce)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071303)
    history: list[dict[tuple[int, int], int]] = [{}]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(170):
        kind = rng.randrange(100)
        if kind < 32:
            row, col = rng.randrange(LIMIT), rng.randrange(LIMIT)
            value = rng.randrange(-1000, 1001)
            commands.append(f"SET {row} {col} {value}")
            next_values = history[-1].copy()
            next_values[row, col] = value
            history.append(next_values)
        elif kind < 52:
            row, col = rng.randrange(LIMIT), rng.randrange(LIMIT)
            value = rng.randrange(-100, 101)
            commands.append(f"ADD {row} {col} {value}")
            next_values = history[-1].copy()
            next_values[row, col] = next_values.get((row, col), 0) + value
            history.append(next_values)
        elif kind < 58 and len(history) > 1:
            commands.append("UNDO")
            history.pop()
        elif kind < 64 and len(history) > 1:
            target = rng.randrange(len(history))
            commands.append(f"ROLLBACK {target}")
            history = history[:target + 1]
        elif kind < 72:
            commands.append("SNAPSHOT")
            outputs.append(str(len(history) - 1))
        elif kind < 82:
            row, col = rng.randrange(LIMIT), rng.randrange(LIMIT)
            commands.append(f"GET {row} {col}")
            outputs.append(str(history[-1].get((row, col), 0)))
        elif kind < 95:
            rb, re = sorted((rng.randrange(LIMIT + 1), rng.randrange(LIMIT + 1)))
            cb, ce = sorted((rng.randrange(LIMIT + 1), rng.randrange(LIMIT + 1)))
            commands.append(f"PROD {rb} {cb} {re} {ce}")
            outputs.append(str(rectangle_sum(history[-1], rb, cb, re, ce)))
        else:
            commands.append("ALL")
            outputs.append(str(sum(history[-1].values())))
    commands.extend([
        "SET 1000000000038 1000000000006 17",
        "GET 1000000000038 1000000000006",
        "UNDO",
        "GET 1000000000038 1000000000006",
    ])
    outputs.extend(["17", str(history[-1].get((1000000000038, 1000000000006), 0))])
    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
