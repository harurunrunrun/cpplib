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

    rng = random.Random(20261116)
    n = 24
    initial = [rng.randrange(-50, 51) for _ in range(n)]
    versions: list[list[int]] = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(350):
        typ = rng.randrange(8)
        if typ < 3:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            x = rng.randrange(-1000, 1001)
            nxt = versions[base][:]
            nxt[k] = x
            versions.append(nxt)
            commands.append(f"SET {base} {k} {x}")
            outputs.append(str(len(versions) - 1))
        elif typ == 3:
            base = rng.randrange(len(versions))
            versions.append(versions[base][:])
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        elif typ < 7:
            version = rng.randrange(len(versions))
            k = rng.randrange(n)
            commands.append(f"GET {version} {k}")
            outputs.append(str(versions[version][k]))
        elif typ == 7:
            commands.append("VERSIONS")
            outputs.append(str(len(versions)))

    commands.append("SIZE")
    outputs.append(str(n))
    commands.append("LATEST")
    outputs.append(str(len(versions) - 1))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
