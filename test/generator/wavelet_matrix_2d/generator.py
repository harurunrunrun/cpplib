#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(271828182)
    n = 233
    xs = [rng.randrange(-300, 301) for _ in range(n)]
    ys = [rng.randrange(-300, 301) for _ in range(n)]
    weights = [rng.randrange(-1_000_000, 1_000_001) for _ in range(n)]
    xs[:6] = [-300, -300, 0, 0, 300, 300]
    ys[:6] = [-300, 300, -1, 1, -300, 300]
    weights[:6] = [10**12, -10**12, 7, -11, 13, -17]
    commands: list[str] = []
    outputs: list[str] = []
    kinds = ["COUNT", "SUM", "KTH", "PREV", "NEXT"]

    for _ in range(1500):
        kind = rng.choice(kinds)
        if kind in {"COUNT", "SUM"}:
            xl, xr = sorted((rng.randrange(-350, 351), rng.randrange(-350, 351)))
            yl, yr = sorted((rng.randrange(-350, 351), rng.randrange(-350, 351)))
            commands.append(f"{kind} {xl} {xr} {yl} {yr}")
            indices = [
                i for i in range(n)
                if xl <= xs[i] < xr and yl <= ys[i] < yr
            ]
            outputs.append(str(len(indices) if kind == "COUNT" else sum(weights[i] for i in indices)))
        elif kind == "KTH":
            while True:
                xl, xr = sorted((rng.randrange(-350, 351), rng.randrange(-350, 351)))
                candidates = sorted(ys[i] for i in range(n) if xl <= xs[i] < xr)
                if candidates:
                    break
            k = rng.randrange(len(candidates))
            commands.append(f"KTH {xl} {xr} {k}")
            outputs.append(str(candidates[k]))
        else:
            xl, xr = sorted((rng.randrange(-350, 351), rng.randrange(-350, 351)))
            value = rng.randrange(-350, 351)
            commands.append(f"{kind} {xl} {xr} {value}")
            candidates = [
                ys[i] for i in range(n)
                if xl <= xs[i] < xr and (ys[i] < value if kind == "PREV" else ys[i] >= value)
            ]
            answer = max(candidates) if kind == "PREV" and candidates else min(candidates) if candidates else None
            outputs.append(optional(answer))

    lines = [f"{n} {len(commands)}"]
    lines.extend(f"{x} {y} {weight}" for x, y, weight in zip(xs, ys, weights))
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()