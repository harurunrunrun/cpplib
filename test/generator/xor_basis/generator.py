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

    rng = random.Random(20260713)
    span = {0}
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(1200):
        kind = rng.randrange(100)
        if kind < 25:
            value = rng.randrange(1 << 16)
            independent = value not in span
            commands.append(f"INSERT {value}")
            outputs.append(str(int(independent)))
            if independent:
                span |= {current ^ value for current in list(span)}
        elif kind < 48:
            value = rng.randrange(1 << 16)
            commands.append(f"CONTAINS {value}")
            outputs.append(str(int(value in span)))
        elif kind < 65:
            seed = rng.randrange(1 << 16)
            commands.append(f"MAX {seed}")
            outputs.append(str(max(seed ^ value for value in span)))
        elif kind < 82:
            seed = rng.randrange(1 << 16)
            commands.append(f"MIN {seed}")
            outputs.append(str(min(seed ^ value for value in span)))
        elif kind < 94:
            k = rng.randrange(len(span))
            commands.append(f"KTH {k}")
            outputs.append(str(sorted(span)[k]))
        elif kind < 99:
            commands.append("RANK")
            outputs.append(str((len(span) - 1).bit_length()))
        else:
            commands.append("CLEAR")
            span = {0}

    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
