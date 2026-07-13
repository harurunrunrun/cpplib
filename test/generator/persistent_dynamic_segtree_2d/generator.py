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
    rng = random.Random(2026071301)
    versions: list[dict[tuple[int, int], int]] = [{}]
    commands: list[str] = []
    outputs: list[str] = []
    for turn in range(130):
        kind = rng.randrange(100)
        version = rng.randrange(len(versions))
        if kind < 30:
            row, col = rng.randrange(LIMIT), rng.randrange(LIMIT)
            value = rng.randrange(-1000, 1001)
            commands.append(f"SET {version} {row} {col} {value}")
            next_values = versions[version].copy()
            next_values[row, col] = value
            versions.append(next_values)
            outputs.append(str(len(versions) - 1))
        elif kind < 50:
            row, col = rng.randrange(LIMIT), rng.randrange(LIMIT)
            value = rng.randrange(-100, 101)
            commands.append(f"ADD {version} {row} {col} {value}")
            next_values = versions[version].copy()
            next_values[row, col] = next_values.get((row, col), 0) + value
            versions.append(next_values)
            outputs.append(str(len(versions) - 1))
        elif kind < 58:
            commands.append(f"FORK {version}")
            versions.append(versions[version].copy())
            outputs.append(str(len(versions) - 1))
        elif kind < 70:
            row, col = rng.randrange(LIMIT), rng.randrange(LIMIT)
            commands.append(f"GET {version} {row} {col}")
            outputs.append(str(versions[version].get((row, col), 0)))
        elif kind < 92:
            rb, re = sorted((rng.randrange(LIMIT + 1), rng.randrange(LIMIT + 1)))
            cb, ce = sorted((rng.randrange(LIMIT + 1), rng.randrange(LIMIT + 1)))
            commands.append(f"PROD {version} {rb} {cb} {re} {ce}")
            outputs.append(str(rectangle_sum(versions[version], rb, cb, re, ce)))
        else:
            commands.append(f"ALL {version}")
            outputs.append(str(sum(versions[version].values())))
    commands.extend([
        "SET 0 1000000000038 1000000000006 17",
        f"GET {len(versions)} 1000000000038 1000000000006",
    ])
    outputs.extend([str(len(versions)), "17"])
    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
