#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

MAX_VERSION = 8


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    persistent = [[0, 0]]
    partial = [[0, 0]]
    rollback = [[0, 0]]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(300):
        kind = rng.randrange(9)
        if kind == 0:
            version = rng.randrange(-2, len(persistent) + 2)
            index = rng.randrange(-2, 4)
            commands.append(f"PGET {version} {index}")
            if 0 <= version < len(persistent) and 0 <= index < 2:
                outputs.append(str(persistent[version][index]))
            else:
                outputs.append("THROW")
        elif kind == 1:
            version = rng.randrange(-2, len(persistent) + 2)
            index = rng.randrange(-2, 4)
            value = rng.randrange(-100, 101)
            commands.append(f"PSET {version} {index} {value}")
            if 0 <= version < len(persistent) and 0 <= index < 2 and len(persistent) <= MAX_VERSION:
                state = persistent[version][:]
                state[index] = value
                persistent.append(state)
                outputs.append(str(len(persistent) - 1))
            else:
                outputs.append("THROW")
        elif kind == 2:
            version = rng.randrange(-2, len(persistent) + 2)
            commands.append(f"PFORK {version}")
            if 0 <= version < len(persistent) and len(persistent) <= MAX_VERSION:
                persistent.append(persistent[version][:])
                outputs.append(str(len(persistent) - 1))
            else:
                outputs.append("THROW")
        elif kind == 3:
            version = rng.randrange(-2, len(partial) + 2)
            index = rng.randrange(-2, 4)
            commands.append(f"TGET {version} {index}")
            if 0 <= version < len(partial) and 0 <= index < 2:
                outputs.append(str(partial[version][index]))
            else:
                outputs.append("THROW")
        elif kind == 4:
            index = rng.randrange(-2, 4)
            value = rng.randrange(-100, 101)
            commands.append(f"TSET {index} {value}")
            if 0 <= index < 2 and len(partial) <= MAX_VERSION:
                state = partial[-1][:]
                state[index] = value
                partial.append(state)
                outputs.append(str(len(partial) - 1))
            else:
                outputs.append("THROW")
        elif kind == 5:
            index = rng.randrange(-2, 4)
            commands.append(f"RGET {index}")
            outputs.append(str(rollback[-1][index]) if 0 <= index < 2 else "THROW")
        elif kind == 6:
            index = rng.randrange(-2, 4)
            value = rng.randrange(-100, 101)
            commands.append(f"RSET {index} {value}")
            if 0 <= index < 2 and len(rollback) - 1 < MAX_VERSION:
                state = rollback[-1][:]
                state[index] = value
                rollback.append(state)
                outputs.append("OK")
            else:
                outputs.append("THROW")
        elif kind == 7:
            commands.append("RUNDO")
            if len(rollback) > 1:
                rollback.pop()
                outputs.append("OK")
            else:
                outputs.append("THROW")
        else:
            snapshot = rng.randrange(-2, len(rollback) + 2)
            commands.append(f"RROLLBACK {snapshot}")
            if 0 <= snapshot < len(rollback):
                rollback = rollback[: snapshot + 1]
                outputs.append("OK")
            else:
                outputs.append("THROW")

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
