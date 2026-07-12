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
    n = 5
    versions = [[-1] * n]
    commands = ["GET 0 0", "VERSIONS"]
    outputs = ["-1", "1"]
    for _ in range(120):
        kind = rng.randrange(4)
        if kind == 0 and len(versions) < 120:
            base = rng.randrange(len(versions))
            index = rng.randrange(n)
            value = rng.randrange(-10**9, 10**9 + 1)
            next_version = versions[base][:]
            next_version[index] = value
            versions.append(next_version)
            commands.append(f"SET {base} {index} {value}")
            outputs.append(str(len(versions) - 1))
        elif kind == 1 and len(versions) < 120:
            base = rng.randrange(len(versions))
            versions.append(versions[base][:])
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        elif kind == 2:
            version = rng.randrange(len(versions))
            index = rng.randrange(n)
            commands.append(f"GET {version} {index}")
            outputs.append(str(versions[version][index]))
        else:
            commands.append("VERSIONS")
            outputs.append(str(len(versions)))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
