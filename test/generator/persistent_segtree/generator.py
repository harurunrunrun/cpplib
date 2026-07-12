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

    rng = random.Random(2026071302)
    n = 57
    initial = [rng.randrange(25) for _ in range(n)]
    versions = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(500):
        typ = rng.randrange(9)
        if typ < 2:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            value = rng.randrange(40)
            next_values = versions[base][:]
            next_values[k] = value
            versions.append(next_values)
            commands.append(f"SET {base} {k} {value}")
            outputs.append(str(len(versions) - 1))
        elif typ == 2:
            base = rng.randrange(len(versions))
            versions.append(versions[base][:])
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        elif typ == 3:
            version = rng.randrange(len(versions))
            k = rng.randrange(n)
            commands.append(f"GET {version} {k}")
            outputs.append(str(versions[version][k]))
        elif typ == 4:
            version = rng.randrange(len(versions))
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            commands.append(f"SUM {version} {left} {right}")
            outputs.append(str(sum(versions[version][left:right])))
        elif typ == 5:
            version = rng.randrange(len(versions))
            commands.append(f"ALL {version}")
            outputs.append(str(sum(versions[version])))
        elif typ == 6:
            version = rng.randrange(len(versions))
            left = rng.randrange(n + 1)
            limit = rng.randrange(sum(versions[version][left:]) + 15)
            commands.append(f"MR {version} {left} {limit}")
            outputs.append(str(max_right(versions[version], left, limit)))
        else:
            version = rng.randrange(len(versions))
            right = rng.randrange(n + 1)
            limit = rng.randrange(sum(versions[version][:right]) + 15)
            commands.append(f"ML {version} {right} {limit}")
            outputs.append(str(min_left(versions[version], right, limit)))

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