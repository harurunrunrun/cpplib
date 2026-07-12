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
    n = 37
    persistent = [[0] * n]
    rollback = [[0] * n]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(1700):
        kind = rng.randrange(8)
        if kind < 2:
            base = rng.randrange(len(persistent))
            index = rng.randrange(n)
            value = rng.randrange(-10**9, 10**9 + 1)
            state = persistent[base][:]
            state[index] = value
            persistent.append(state)
            commands.append(f"PSET {base} {index} {value}")
            outputs.append(str(len(persistent) - 1))
        elif kind == 2:
            base = rng.randrange(len(persistent))
            persistent.append(persistent[base][:])
            commands.append(f"PFORK {base}")
            outputs.append(str(len(persistent) - 1))
        elif kind == 3:
            version = rng.randrange(len(persistent))
            index = rng.randrange(n)
            commands.append(f"PGET {version} {index}")
            outputs.append(str(persistent[version][index]))
        elif kind == 4:
            commands.append("PVERSIONS")
            outputs.append(str(len(persistent)))
        elif kind == 5 and len(rollback) < 1900:
            index = rng.randrange(n)
            value = rng.randrange(-10**9, 10**9 + 1)
            state = rollback[-1][:]
            state[index] = value
            rollback.append(state)
            commands.append(f"RSET {index} {value}")
        elif kind == 6:
            index = rng.randrange(n)
            commands.append(f"RGET {index}")
            outputs.append(str(rollback[-1][index]))
        else:
            if len(rollback) > 1 and rng.randrange(2):
                rollback.pop()
                commands.append("RUNDO")
            else:
                snapshot = rng.randrange(len(rollback))
                rollback = rollback[: snapshot + 1]
                commands.append(f"RROLLBACK {snapshot}")
            commands.append("RSNAPSHOT")
            outputs.append(str(len(rollback) - 1))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
