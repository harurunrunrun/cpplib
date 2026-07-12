#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path

INT_MIN = -(1 << 31)
INT_MAX = (1 << 31) - 1


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n = 173
    values = [rng.randrange(-100000, 100001) for _ in range(n)]
    commands = ["MIN 0 0", f"MAX {n} {n}", "GCD 0 0", "GET 0"]
    outputs = [str(INT_MAX), str(INT_MIN), "0", str(values[0])]

    for _ in range(650):
        kind = rng.randrange(4)
        if kind == 3:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(values[k]))
            continue
        l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
        if kind == 0:
            commands.append(f"MIN {l} {r}")
            outputs.append(str(min(values[l:r], default=INT_MAX)))
        elif kind == 1:
            commands.append(f"MAX {l} {r}")
            outputs.append(str(max(values[l:r], default=INT_MIN)))
        else:
            commands.append(f"GCD {l} {r}")
            g = 0
            for value in values[l:r]:
                g = math.gcd(g, value)
            outputs.append(str(g))

    input_text = (
        f"{n} {len(commands)}\n"
        + " ".join(map(str, values))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
