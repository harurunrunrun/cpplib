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
    n = 137
    values = [rng.randrange(-1000, 1001) for _ in range(n)]
    initial = values[:]
    commands = ["PREFIX 0", f"SUM 0 {n}", "GET 0"]
    outputs = ["0", str(sum(values)), str(values[0])]

    for _ in range(600):
        kind = rng.randrange(5)
        if kind == 0:
            k = rng.randrange(n)
            x = rng.randrange(-500, 501)
            commands.append(f"ADD {k} {x}")
            values[k] += x
        elif kind == 1:
            k = rng.randrange(n)
            x = rng.randrange(-2000, 2001)
            commands.append(f"SET {k} {x}")
            values[k] = x
        elif kind == 2:
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(str(values[k]))
        elif kind == 3:
            l = rng.randrange(n + 1)
            r = rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            commands.append(f"SUM {l} {r}")
            outputs.append(str(sum(values[l:r])))
        else:
            r = rng.randrange(n + 1)
            commands.append(f"PREFIX {r}")
            outputs.append(str(sum(values[:r])))

    input_text = (
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
