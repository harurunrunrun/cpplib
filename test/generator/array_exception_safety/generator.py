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
    persistent = [1]
    rollback = [1]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(220):
        kind = rng.randrange(7)
        if kind == 0 and len(persistent) < 60:
            base = rng.randrange(len(persistent))
            value = rng.randrange(-1000, 1001)
            budget = rng.choice([-1, 0, 1])
            commands.append(f"PSET {base} {value} {budget}")
            if budget == 0:
                outputs.append("THROW")
            else:
                persistent.append(value)
                outputs.append(str(len(persistent) - 1))
        elif kind == 1 and len(rollback) < 60:
            value = rng.randrange(-1000, 1001)
            budget = rng.choice([-1, 0, 1])
            commands.append(f"RSET {value} {budget}")
            if budget == 0:
                outputs.append("THROW")
            else:
                rollback.append(value)
                outputs.append("OK")
        elif kind == 2:
            version = rng.randrange(len(persistent))
            commands.append(f"PGET {version}")
            outputs.append(str(persistent[version]))
        elif kind == 3:
            commands.append("RGET")
            outputs.append(str(rollback[-1]))
        elif kind == 4:
            commands.append("PVERSIONS")
            outputs.append(str(len(persistent)))
        elif kind == 5:
            commands.append("RHISTORY")
            outputs.append(str(len(rollback) - 1))
        else:
            snapshot = rng.randrange(len(rollback))
            rollback = rollback[: snapshot + 1]
            commands.append(f"RROLLBACK {snapshot}")
            outputs.append("OK")

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
