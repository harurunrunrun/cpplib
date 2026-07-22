#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def make_case(path: Path, seed: int, initial_size: int, query_count: int) -> None:
    rng = random.Random(seed)
    xs = [rng.randrange(-1_000_000, 1_000_001) for _ in range(initial_size)]
    ys = [rng.randrange(-1_000_000, 1_000_001) for _ in range(initial_size)]
    weights = [rng.randrange(-1_000_000, 1_000_001) for _ in range(initial_size)]
    initial = list(zip(xs, ys, weights))
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(query_count):
        if not xs:
            kind = "INSERT"
        else:
            kind = rng.choices(
                [
                    "INSERT", "ERASE", "SET", "SETX", "SETP", "SETW",
                    "GET", "COUNT", "SUM", "KTH", "PREV", "NEXT",
                ],
                [8, 8, 7, 6, 7, 5, 5, 12, 12, 8, 6, 6],
            )[0]
        if len(xs) >= 7000 and kind == "INSERT":
            kind = "ERASE"

        if kind == "INSERT":
            k = rng.randrange(len(xs) + 1)
            x = rng.randrange(-1_200_000, 1_200_001)
            y = rng.randrange(-1_200_000, 1_200_001)
            weight = rng.randrange(-1_200_000, 1_200_001)
            commands.append(f"INSERT {k} {x} {y} {weight}")
            xs.insert(k, x)
            ys.insert(k, y)
            weights.insert(k, weight)
        elif kind == "ERASE":
            k = rng.randrange(len(xs))
            commands.append(f"ERASE {k}")
            del xs[k]
            del ys[k]
            del weights[k]
        elif kind == "SET":
            k = rng.randrange(len(xs))
            y = rng.randrange(-1_200_000, 1_200_001)
            weight = rng.randrange(-1_200_000, 1_200_001)
            commands.append(f"SET {k} {y} {weight}")
            ys[k], weights[k] = y, weight
        elif kind == "SETX":
            k = rng.randrange(len(xs))
            x = rng.randrange(-1_200_000, 1_200_001)
            commands.append(f"SETX {k} {x}")
            xs[k] = x
        elif kind == "SETP":
            k = rng.randrange(len(xs))
            x = rng.randrange(-1_200_000, 1_200_001)
            y = rng.randrange(-1_200_000, 1_200_001)
            weight = rng.randrange(-1_200_000, 1_200_001)
            commands.append(f"SETP {k} {x} {y} {weight}")
            xs[k], ys[k], weights[k] = x, y, weight
        elif kind == "SETW":
            k = rng.randrange(len(xs))
            weight = rng.randrange(-1_200_000, 1_200_001)
            commands.append(f"SETW {k} {weight}")
            weights[k] = weight
        elif kind == "GET":
            k = rng.randrange(len(xs))
            commands.append(f"GET {k}")
            outputs.append(f"{xs[k]} {ys[k]} {weights[k]}")
        elif kind in {"COUNT", "SUM"}:
            xl, xr = sorted((rng.randrange(-1_300_000, 1_300_001),
                             rng.randrange(-1_300_000, 1_300_001)))
            yl, yr = sorted((rng.randrange(-1_300_000, 1_300_001),
                             rng.randrange(-1_300_000, 1_300_001)))
            commands.append(f"{kind} {xl} {xr} {yl} {yr}")
            selected = [
                k for k in range(len(xs))
                if xl <= xs[k] < xr and yl <= ys[k] < yr
            ]
            outputs.append(str(len(selected) if kind == "COUNT"
                               else sum(weights[k] for k in selected)))
        elif kind == "KTH":
            if rng.randrange(3) == 0:
                xl, xr = -1_300_000, 1_300_001
            else:
                chosen = rng.randrange(len(xs))
                xl = xs[chosen]
                xr = xl + rng.randrange(1, 2_600_002)
            candidates = sorted(ys[k] for k in range(len(xs)) if xl <= xs[k] < xr)
            if not candidates:
                xl, xr = -1_300_000, 1_300_001
                candidates = sorted(ys)
            k = rng.randrange(len(candidates))
            commands.append(f"KTH {xl} {xr} {k}")
            outputs.append(str(candidates[k]))
        else:
            xl, xr = sorted((rng.randrange(-1_300_000, 1_300_001),
                             rng.randrange(-1_300_000, 1_300_001)))
            value = rng.randrange(-1_300_000, 1_300_001)
            commands.append(f"{kind} {xl} {xr} {value}")
            candidates = [
                ys[k] for k in range(len(xs))
                if xl <= xs[k] < xr
                and (ys[k] < value if kind == "PREV" else ys[k] >= value)
            ]
            answer = max(candidates) if kind == "PREV" and candidates \
                else min(candidates) if candidates else None
            outputs.append(optional(answer))

    lines = [f"{initial_size} {len(commands)}"]
    lines.extend(f"{x} {y} {weight}" for x, y, weight in initial)
    lines.extend(commands)
    path.with_suffix(".in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    make_case(out_dir / "case_00", 20260713, 173, 2000)
    make_case(out_dir / "case_01_performance", 20260722, 2000, 12000)


if __name__ == "__main__":
    main()
