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
    n = 211
    values = list(range(-500, -500 + n * 5, 5))
    rng.shuffle(values)
    weights = [rng.randrange(-1000, 1001) for _ in range(n)]
    commands: list[str] = []
    outputs: list[str] = []
    kinds = ["FSUM", "WSUM", "FRANGE", "WRANGE", "FREQ", "FKSMALL", "FKLARGE", "WKSMALL", "WKLARGE", "MFLOOR", "MCEIL"]

    for _ in range(1300):
        kind = rng.choice(kinds)
        if kind in {"MFLOOR", "MCEIL"}:
            l = rng.randrange(n)
            r = rng.randrange(l + 1, n + 1)
        else:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
        if kind == "FSUM":
            commands.append(f"FSUM {l} {r}")
            outputs.append(str(sum(values[l:r])))
        elif kind == "WSUM":
            commands.append(f"WSUM {l} {r}")
            outputs.append(str(sum(weights[l:r])))
        elif kind in {"FRANGE", "WRANGE", "FREQ"}:
            lower, upper = sorted((rng.randrange(-600, 701), rng.randrange(-600, 701)))
            commands.append(f"{kind} {l} {r} {lower} {upper}")
            indices = [i for i in range(l, r) if lower <= values[i] < upper]
            answer = sum(values[i] for i in indices) if kind == "FRANGE" else sum(weights[i] for i in indices) if kind == "WRANGE" else len(indices)
            outputs.append(str(answer))
        elif kind in {"FKSMALL", "FKLARGE", "WKSMALL", "WKLARGE"}:
            order = sorted(range(l, r), key=lambda i: values[i])
            k = rng.randrange(len(order) + 1)
            commands.append(f"{kind} {l} {r} {k}")
            chosen = order[:k] if kind in {"FKSMALL", "WKSMALL"} else order[len(order) - k :]
            outputs.append(str(sum(values[i] for i in chosen) if kind.startswith("F") else sum(weights[i] for i in chosen)))
        else:
            ordered = sorted(values[l:r])
            commands.append(f"{kind} {l} {r}")
            outputs.append(str(ordered[(len(ordered) - 1) // 2] if kind == "MFLOOR" else ordered[len(ordered) // 2]))

    input_text = f"{n} {len(commands)}\n" + " ".join(map(str, values)) + "\n" + " ".join(map(str, weights)) + "\n" + "\n".join(commands) + "\n"
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
