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

    rng = random.Random(20261117)
    n = 80
    a = [rng.randrange(-50, 51) for _ in range(n)]
    initial = a[:]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(400):
        typ = rng.randrange(5)
        if typ == 0:
            k = rng.randrange(n)
            x = rng.randrange(-100, 101)
            a[k] = x
            commands.append(f"SET {k} {x}")
        elif typ < 4:
            l = rng.randrange(n + 1)
            r = rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            x = rng.randrange(-50, 51)
            for i in range(l, r):
                a[i] += x
            commands.append(f"APPLY {l} {r} {x}")
        else:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(a[k]))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
