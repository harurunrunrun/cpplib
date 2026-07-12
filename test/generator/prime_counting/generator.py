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

    limit = 500000
    prime = [True] * (limit + 1)
    prime[0] = prime[1] = False
    for p in range(2, int(limit**0.5) + 1):
        if prime[p]:
            prime[p * p : limit + 1 : p] = [False] * (((limit - p * p) // p) + 1)
    prefix = [0] * (limit + 1)
    for n in range(1, limit + 1):
        prefix[n] = prefix[n - 1] + int(prime[n])

    rng = random.Random(20260713)
    commands = ["IS 0", "IS 2", f"COUNT {limit}", "LESS 0", f"LESS {limit + 1}"]
    outputs = ["0", "1", str(prefix[limit]), "0", str(prefix[limit])]
    for _ in range(1800):
        kind = rng.choice(["IS", "COUNT", "LESS"])
        n = rng.randrange(limit + 1) if kind != "LESS" else rng.randrange(limit + 2)
        commands.append(f"{kind} {n}")
        outputs.append(str(int(prime[n])) if kind == "IS" else str(prefix[n]) if kind == "COUNT" else str(prefix[n - 1] if n > 0 else 0))

    (out_dir / "case_00.in").write_text(f"{limit} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
