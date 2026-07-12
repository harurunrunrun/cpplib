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

    rng = random.Random(918273645)
    n = 257
    values = [rng.randrange(-1000, 1001) for _ in range(n)]
    values[:8] = [-1000, -1, 0, 0, 1, 999, 1000, -1000]
    commands: list[str] = []
    outputs: list[str] = []
    kinds = ["ACCESS", "RANK", "SELECT", "KTH", "KLARG", "FREQ", "RANGE", "PREV", "NEXT"]

    for _ in range(1600):
        kind = rng.choice(kinds)
        if kind == "ACCESS":
            k = rng.randrange(n)
            commands.append(f"ACCESS {k}")
            outputs.append(str(values[k]))
        elif kind == "RANK":
            value = rng.randrange(-1200, 1201)
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"RANK {value} {l} {r}")
            outputs.append(str(values[l:r].count(value)))
        elif kind == "SELECT":
            value = rng.randrange(-1200, 1201)
            occurrence = rng.randrange(8)
            commands.append(f"SELECT {value} {occurrence}")
            positions = [i for i, item in enumerate(values) if item == value]
            outputs.append(str(positions[occurrence] if occurrence < len(positions) else n))
        elif kind in {"KTH", "KLARG"}:
            l = rng.randrange(n)
            r = rng.randrange(l + 1, n + 1)
            ordered = sorted(values[l:r])
            k = rng.randrange(len(ordered))
            commands.append(f"{kind} {l} {r} {k}")
            outputs.append(str(ordered[k] if kind == "KTH" else ordered[-1 - k]))
        elif kind == "FREQ":
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            upper = rng.randrange(-1200, 1201)
            commands.append(f"FREQ {l} {r} {upper}")
            outputs.append(str(sum(item < upper for item in values[l:r])))
        elif kind == "RANGE":
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-1200, 1201), rng.randrange(-1200, 1201)))
            commands.append(f"RANGE {l} {r} {lower} {upper}")
            outputs.append(str(sum(lower <= item < upper for item in values[l:r])))
        else:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            value = rng.randrange(-1200, 1201)
            commands.append(f"{kind} {l} {r} {value}")
            if kind == "PREV":
                candidates = [item for item in values[l:r] if item < value]
            else:
                candidates = [item for item in values[l:r] if item >= value]
            answer = max(candidates) if kind == "PREV" and candidates else min(candidates) if candidates else None
            outputs.append(optional(answer))

    lines = [f"{n} {len(commands)}", " ".join(map(str, values)), *commands]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()