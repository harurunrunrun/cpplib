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

    rng = random.Random(314159265)
    n = 241
    values = [rng.randrange(-500, 501) for _ in range(n)]
    weights = [rng.randrange(-1_000_000, 1_000_001) for _ in range(n)]
    values[:7] = [-500, -1, 0, 0, 1, 499, 500]
    weights[:7] = [10**12, -10**12, 7, -11, 13, 17, -19]
    commands: list[str] = []
    outputs: list[str] = []
    kinds = [
        "ACCESS", "RANK", "KTH", "KLARG", "FREQ", "RANGE",
        "SUM", "SUMLESS", "SUMRANGE", "PREV", "NEXT",
    ]

    for _ in range(1800):
        kind = rng.choice(kinds)
        if kind == "ACCESS":
            k = rng.randrange(n)
            commands.append(f"ACCESS {k}")
            outputs.append(str(values[k]))
        elif kind == "RANK":
            value = rng.randrange(-650, 651)
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"RANK {value} {l} {r}")
            outputs.append(str(values[l:r].count(value)))

        elif kind in {"KTH", "KLARG"}:
            l = rng.randrange(n)
            r = rng.randrange(l + 1, n + 1)
            ordered = sorted(values[l:r])
            k = rng.randrange(len(ordered))
            commands.append(f"{kind} {l} {r} {k}")
            outputs.append(str(ordered[k] if kind == "KTH" else ordered[-1 - k]))
        elif kind == "SUM":
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"SUM {l} {r}")
            outputs.append(str(sum(weights[l:r])))
        elif kind in {"FREQ", "SUMLESS"}:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            upper = rng.randrange(-650, 651)
            commands.append(f"{kind} {l} {r} {upper}")
            if kind == "FREQ":
                outputs.append(str(sum(item < upper for item in values[l:r])))
            else:
                outputs.append(str(sum(weights[i] for i in range(l, r) if values[i] < upper)))
        elif kind in {"RANGE", "SUMRANGE"}:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-650, 651), rng.randrange(-650, 651)))
            commands.append(f"{kind} {l} {r} {lower} {upper}")
            if kind == "RANGE":
                outputs.append(str(sum(lower <= item < upper for item in values[l:r])))
            else:
                outputs.append(str(sum(weights[i] for i in range(l, r) if lower <= values[i] < upper)))
        else:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            value = rng.randrange(-650, 651)
            commands.append(f"{kind} {l} {r} {value}")
            candidates = [item for item in values[l:r] if item < value] if kind == "PREV" else [item for item in values[l:r] if item >= value]
            answer = max(candidates) if kind == "PREV" and candidates else min(candidates) if candidates else None
            outputs.append(optional(answer))

    lines = [
        f"{n} {len(commands)}",
        " ".join(map(str, values)),
        " ".join(map(str, weights)),
        *commands,
    ]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()