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
    n = 193
    values = [rng.randrange(-500, 501) * rng.randrange(1, 4) for _ in range(n)]
    commands = ["ACCESS 0", f"RANK {values[0]} 0 {n}", f"SELECT {values[0]} 0", "FREQ 0 0 0", "PREV 0 0 0", "NEXT 0 0 0"]
    outputs = [str(values[0]), str(values.count(values[0])), "0", "0", "NONE", "NONE"]
    kinds = ["ACCESS", "RANK", "SELECT", "KTH", "KLARG", "FREQ", "RANGE", "MIN", "MAX", "MFLOOR", "MCEIL", "PREV", "NEXT"]

    for _ in range(1000):
        kind = rng.choice(kinds)
        if kind == "ACCESS":
            k = rng.randrange(n)
            commands.append(f"ACCESS {k}")
            outputs.append(str(values[k]))
        elif kind == "RANK":
            value = rng.randrange(-1600, 1601)
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"RANK {value} {l} {r}")
            outputs.append(str(values[l:r].count(value)))
        elif kind == "SELECT":
            value = rng.randrange(-1600, 1601)
            occurrence = rng.randrange(8)
            commands.append(f"SELECT {value} {occurrence}")
            positions = [index for index, item in enumerate(values) if item == value]
            outputs.append(str(positions[occurrence] if occurrence < len(positions) else n))
        elif kind in {"KTH", "KLARG", "MIN", "MAX", "MFLOOR", "MCEIL"}:
            l, r = sorted((rng.randrange(n), rng.randrange(1, n + 1)))
            if l >= r:
                l, r = min(l, n - 1), max(r, min(l, n - 1) + 1)
            ordered = sorted(values[l:r])
            if kind in {"KTH", "KLARG"}:
                k = rng.randrange(len(ordered))
                commands.append(f"{kind} {l} {r} {k}")
                outputs.append(str(ordered[k] if kind == "KTH" else ordered[-1 - k]))
            else:
                commands.append(f"{kind} {l} {r}")
                answers = {
                    "MIN": ordered[0],
                    "MAX": ordered[-1],
                    "MFLOOR": ordered[(len(ordered) - 1) // 2],
                    "MCEIL": ordered[len(ordered) // 2],
                }
                outputs.append(str(answers[kind]))
        elif kind == "FREQ":
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            upper = rng.randrange(-1600, 1601)
            commands.append(f"FREQ {l} {r} {upper}")
            outputs.append(str(sum(value < upper for value in values[l:r])))
        elif kind == "RANGE":
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-1600, 1601), rng.randrange(-1600, 1601)))
            commands.append(f"RANGE {l} {r} {lower} {upper}")
            outputs.append(str(sum(lower <= value < upper for value in values[l:r])))
        else:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            value = rng.randrange(-1600, 1601)
            commands.append(f"{kind} {l} {r} {value}")
            if kind == "PREV":
                candidates = [item for item in values[l:r] if item < value]
                outputs.append(optional(max(candidates) if candidates else None))
            else:
                candidates = [item for item in values[l:r] if item >= value]
                outputs.append(optional(min(candidates) if candidates else None))

    input_text = (
        f"{n} {len(commands)}\n"
        + " ".join(map(str, values))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")
    (out_dir / "case_01.in").write_text("-1 0\n", encoding="utf-8")
    (out_dir / "case_01.out").write_text(
        "32769 32768\n0 32767\n2\n32769\nTHROW\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
