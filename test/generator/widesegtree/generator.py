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

    rng = random.Random(20261115)
    n = 1024
    a = [0] * n
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(500):
        typ = rng.randrange(5)
        if typ == 0:
            i = rng.randrange(n)
            x = rng.randrange(-1000, 1001)
            a[i] = x
            commands.append(f"SET {i} {x}")
        elif typ == 1:
            i = rng.randrange(n)
            x = rng.randrange(-100, 101)
            a[i] += x
            commands.append(f"ADD {i} {x}")
        elif typ == 2:
            i = rng.randrange(n)
            commands.append(f"GET {i}")
            outputs.append(str(a[i]))
        elif typ == 3:
            l = rng.randrange(n + 1)
            r = rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            commands.append(f"PROD {l} {r}")
            outputs.append(str(sum(a[l:r])))
        else:
            commands.append("ALL")
            outputs.append(str(sum(a)))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
