#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

MOD = 998244353


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    commands: list[str] = []
    outputs: list[str] = []
    for n, k in [(0, -1), (0, 0), (5, 2), (5, 5), (200, 250)]:
        for kind in ["COMB", "PREFIX"]:
            commands.append(f"{kind} {n} {k}")
            if kind == "COMB":
                outputs.append(str(math.comb(n, k) % MOD if 0 <= k <= n else 0))
            else:
                outputs.append(str(sum(math.comb(n, i) for i in range(max(0, min(k, n) + 1))) % MOD))

    for _ in range(700):
        n = rng.randrange(201)
        k = rng.randrange(-20, 251)
        kind = rng.choice(["COMB", "PREFIX"])
        commands.append(f"{kind} {n} {k}")
        if kind == "COMB":
            outputs.append(str(math.comb(n, k) % MOD if 0 <= k <= n else 0))
        elif k < 0:
            outputs.append("0")
        else:
            outputs.append(str(sum(math.comb(n, i) for i in range(min(k, n) + 1)) % MOD))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
