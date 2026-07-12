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

    rng = random.Random(20260713)
    n = 173
    xs = [rng.randrange(-200, 201) for _ in range(n)]
    ys = [rng.randrange(-200, 201) for _ in range(n)]
    weights = [rng.randrange(-1000, 1001) for _ in range(n)]
    initial = list(zip(xs, ys, weights))
    commands: list[str] = []
    outputs: list[str] = []
    kinds = ["SET", "SETW", "GET", "COUNT", "SUM", "KTH", "PREV", "NEXT"]

    for _ in range(1200):
        kind = rng.choice(kinds)
        if kind == "SET":
            k = rng.randrange(n)
            y = rng.randrange(-250, 251)
            weight = rng.randrange(-1200, 1201)
            commands.append(f"SET {k} {y} {weight}")
            ys[k], weights[k] = y, weight
        elif kind == "SETW":
            k = rng.randrange(n)
            weight = rng.randrange(-1200, 1201)
            commands.append(f"SETW {k} {weight}")
            weights[k] = weight
        elif kind == "GET":
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(f"{xs[k]} {ys[k]} {weights[k]}")
        elif kind in {"COUNT", "SUM"}:
            xl, xr = sorted((rng.randrange(-260, 261), rng.randrange(-260, 261)))
            yl, yr = sorted((rng.randrange(-260, 261), rng.randrange(-260, 261)))
            commands.append(f"{kind} {xl} {xr} {yl} {yr}")
            indices = [k for k in range(n) if xl <= xs[k] < xr and yl <= ys[k] < yr]
            outputs.append(str(len(indices) if kind == "COUNT" else sum(weights[k] for k in indices)))
        elif kind == "KTH":
            xl, xr = -260, 261
            candidates = sorted(ys[k] for k in range(n) if xl <= xs[k] < xr)
            k = rng.randrange(len(candidates))
            commands.append(f"KTH {xl} {xr} {k}")
            outputs.append(str(candidates[k]))
        else:
            xl, xr = sorted((rng.randrange(-260, 261), rng.randrange(-260, 261)))
            value = rng.randrange(-260, 261)
            commands.append(f"{kind} {xl} {xr} {value}")
            candidates = [ys[k] for k in range(n) if xl <= xs[k] < xr and (ys[k] < value if kind == "PREV" else ys[k] >= value)]
            answer = max(candidates) if kind == "PREV" and candidates else min(candidates) if candidates else None
            outputs.append(optional(answer))

    lines = [f"{n} {len(commands)}"]
    lines.extend(f"{x} {y} {weight}" for x, y, weight in initial)
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
