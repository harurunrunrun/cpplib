#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(4242424201)
    values: dict[int, int] = {}
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(2200):
        kind = rng.randrange(11)
        key = rng.randrange(-100, 401)
        if kind == 0:
            value = rng.randrange(-10**9, 10**9 + 1)
            commands.append(f"INSERT {key} {value}")
            inserted = key not in values
            outputs.append(str(int(inserted)))
            if inserted:
                values[key] = value
        elif kind == 1:
            commands.append(f"ERASE {key}")
            erased = key in values
            outputs.append(str(int(erased)))
            values.pop(key, None)
        elif kind == 2:
            value = rng.randrange(-10**9, 10**9 + 1)
            commands.append(f"SET {key} {value}")
            exists = key in values
            outputs.append(str(int(exists)))
            if exists:
                values[key] = value
        elif kind in {3, 4}:
            l, r = sorted((rng.randrange(-120, 421), rng.randrange(-120, 421)))
            delta = rng.randrange(-10**6, 10**6 + 1)
            commands.append(f"APPLY {l} {r} {delta}")
            for current in list(values):
                if l <= current < r:
                    values[current] += delta
        elif kind == 5:
            commands.append(f"GET {key}")
            outputs.append(optional(values.get(key)))
        elif kind == 6:
            commands.append(f"CONTAINS {key}")
            outputs.append(str(int(key in values)))
        elif kind == 7:
            l, r = sorted((rng.randrange(-120, 421), rng.randrange(-120, 421)))
            commands.append(f"PROD {l} {r}")
            outputs.append(str(sum(value for current, value in values.items() if l <= current < r)))
        elif kind == 8:
            k = rng.randrange(-2, len(values) + 3)
            commands.append(f"KTH {k}")
            keys = sorted(values)
            outputs.append(optional(keys[k] if 0 <= k < len(keys) else None))
        elif kind == 9:
            commands.append(f"ORDER {key}")
            outputs.append(str(sum(current < key for current in values)))
        else:
            command = "SIZE" if rng.randrange(2) == 0 else "ALL"
            commands.append(command)
            outputs.append(str(len(values) if command == "SIZE" else sum(values.values())))

    lines = [str(len(commands)), *commands]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()