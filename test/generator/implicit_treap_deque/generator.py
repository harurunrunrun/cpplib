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

    rng = random.Random(2026071501)
    state: list[int] = []
    operations: list[str] = []
    answers: list[str] = []
    for _ in range(2500):
        if not state:
            kind = rng.choice(["PF", "PB"])
        else:
            kind = rng.choice(["PF", "PB", "OF", "OB", "G", "S"])
        if kind in ("PF", "PB"):
            value = rng.randrange(-10**12, 10**12)
            operations.append(f"{kind} {value}")
            if kind == "PF":
                state.insert(0, value)
            else:
                state.append(value)
        elif kind == "OF":
            operations.append(kind)
            state.pop(0)
        elif kind == "OB":
            operations.append(kind)
            state.pop()
        elif kind == "G":
            index = rng.randrange(len(state))
            operations.append(f"G {index}")
            answers.append(str(state[index]))
        else:
            operations.append(kind)
            answers.append(str(len(state)))

    lines = [str(len(operations)), *operations]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
