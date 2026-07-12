#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_VERSION = 30000


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071317)
    n = 32768
    initial_values = [rng.randrange(-1000, 1001) for _ in range(n)]
    initial_weights = [rng.randrange(-100000, 100001) for _ in range(n)]
    changes: list[tuple[int, int, int] | None] = [None]
    commands: list[str] = []
    outputs: list[str] = []

    for version in range(1, MAX_VERSION + 1):
        if version % 8 == 0:
            k = rng.randrange(n)
            value = rng.randrange(-1000, 1001)
            weight = rng.randrange(-100000, 100001)
            commands.append(f"SET 0 {k} {value} {weight}")
            outputs.append(
                f"{version} {value} {weight} "
                f"{initial_values[k]} {initial_weights[k]}"
            )
            changes.append((k, value, weight))
        else:
            base = rng.randrange(version)
            change = changes[base]
            probe = change[0] if change is not None and version % 2 == 0 else rng.randrange(n)
            commands.append(f"FORK {base} {probe}")
            if change is not None and change[0] == probe:
                value = change[1]
                weight = change[2]
            else:
                value = initial_values[probe]
                weight = initial_weights[probe]
            outputs.append(f"{version} {value} {weight}")
            changes.append(change)

    commands.append("VERSIONS")
    outputs.append(str(MAX_VERSION + 1))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial_values))
        + "\n"
        + " ".join(map(str, initial_weights))
        + "\n"
        + "\n".join(commands)
        + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
