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
    n = 223
    values = [rng.randrange(-300, 301) for _ in range(n)]
    weights = [rng.randrange(-1000, 1001) for _ in range(n)]
    initial_values, initial_weights = values[:], weights[:]
    commands: list[str] = []
    outputs: list[str] = []
    kinds = ["SET", "SETV", "SETW", "GET", "SUM", "FREQ", "RSUM", "KTH", "KLARG", "KSMALL", "KLARGE", "PREV", "NEXT"]

    for _ in range(1500):
        kind = rng.choice(kinds)
        if kind == "SET":
            k, value, weight = rng.randrange(n), rng.randrange(-350, 351), rng.randrange(-1200, 1201)
            commands.append(f"SET {k} {value} {weight}")
            values[k], weights[k] = value, weight
        elif kind == "SETV":
            k, value = rng.randrange(n), rng.randrange(-350, 351)
            commands.append(f"SETV {k} {value}")
            values[k] = value
        elif kind == "SETW":
            k, weight = rng.randrange(n), rng.randrange(-1200, 1201)
            commands.append(f"SETW {k} {weight}")
            weights[k] = weight
        elif kind == "GET":
            k = rng.randrange(n)
            commands.append(f"GET {k}")
            outputs.append(f"{values[k]} {weights[k]}")
        elif kind == "SUM":
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"SUM {l} {r}")
            outputs.append(str(sum(weights[l:r])))
        elif kind in {"FREQ", "RSUM"}:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-400, 401), rng.randrange(-400, 401)))
            commands.append(f"{kind} {l} {r} {lower} {upper}")
            indices = [i for i in range(l, r) if lower <= values[i] < upper]
            outputs.append(str(len(indices) if kind == "FREQ" else sum(weights[i] for i in indices)))
        elif kind in {"KTH", "KLARG", "KSMALL", "KLARGE"}:
            l = rng.randrange(n)
            r = rng.randrange(l + 1, n + 1)
            order = sorted(range(l, r), key=lambda i: values[i])
            if kind in {"KTH", "KLARG"}:
                k = rng.randrange(len(order))
                commands.append(f"{kind} {l} {r} {k}")
                outputs.append(str(values[order[k]] if kind == "KTH" else values[order[-1 - k]]))
            else:
                k = rng.randrange(len(order) + 1)
                commands.append(f"{kind} {l} {r} {k}")
                chosen = order[:k] if kind == "KSMALL" else order[len(order) - k :]
                outputs.append(str(sum(weights[i] for i in chosen)))
        else:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            value = rng.randrange(-400, 401)
            commands.append(f"{kind} {l} {r} {value}")
            candidates = [item for item in values[l:r] if item < value] if kind == "PREV" else [item for item in values[l:r] if item >= value]
            answer = max(candidates) if kind == "PREV" and candidates else min(candidates) if candidates else None
            outputs.append(optional(answer))

    input_text = f"{n} {len(commands)}\n" + " ".join(map(str, initial_values)) + "\n" + " ".join(map(str, initial_weights)) + "\n" + "\n".join(commands) + "\n"
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
