#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

LIMIT = 1 << 20


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    values: list[int] = []
    commands = ["SIZE", "MIN 0", "MAX 1048575", "KTH 0 0", "LESS 0 0"]
    outputs = ["0", "NONE", "NONE", "NONE", "0"]

    for _ in range(1400):
        kind = rng.randrange(10)
        if kind < 3:
            value = rng.randrange(LIMIT)
            commands.append(f"INSERT {value}")
            values.append(value)
        elif kind == 3:
            value = rng.randrange(LIMIT)
            commands.append(f"ERASE {value}")
            present = value in values
            outputs.append(str(int(present)))
            if present:
                values.remove(value)
        elif kind == 4:
            value = rng.randrange(LIMIT)
            commands.append(f"COUNT {value}")
            outputs.append(str(values.count(value)))
        elif kind == 5:
            value = rng.randrange(LIMIT)
            commands.append(f"CONTAINS {value}")
            outputs.append(str(int(value in values)))
        elif kind == 6:
            xor_value = rng.randrange(LIMIT)
            k = rng.randrange(-5, len(values) + 6)
            commands.append(f"KTH {k} {xor_value}")
            ordered = sorted(values, key=lambda value: value ^ xor_value)
            answer = ordered[k] if 0 <= k < len(ordered) else None
            outputs.append(optional(answer))
        elif kind == 7:
            xor_value = rng.randrange(LIMIT)
            commands.append(f"MIN {xor_value}")
            answer = min(values, key=lambda value: value ^ xor_value) if values else None
            outputs.append(optional(answer))
        elif kind == 8:
            xor_value = rng.randrange(LIMIT)
            commands.append(f"MAX {xor_value}")
            answer = max(values, key=lambda value: value ^ xor_value) if values else None
            outputs.append(optional(answer))
        else:
            value = rng.randrange(LIMIT + 1)
            xor_value = rng.randrange(LIMIT)
            commands.append(f"LESS {value} {xor_value}")
            outputs.append(str(sum((item ^ xor_value) < value for item in values)))
        if rng.randrange(20) == 0:
            commands.append("SIZE")
            outputs.append(str(len(values)))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
