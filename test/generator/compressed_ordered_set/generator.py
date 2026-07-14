#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260715)
    universe = sorted(rng.sample(range(-500, 501), 180))
    initial = rng.sample(universe, 60)
    state = sorted(set(initial))
    operations: list[str] = []
    answers: list[str] = []
    query_types = ["I", "E", "K", "O", "C", "P", "S", "Z"]
    for _ in range(1600):
        kind = rng.choice(query_types)
        if kind in ("I", "E"):
            value = rng.choice(universe)
            index = bisect.bisect_left(state, value)
            if kind == "I" and (index == len(state) or state[index] != value):
                state.insert(index, value)
            if kind == "E" and index < len(state) and state[index] == value:
                state.pop(index)
        elif kind == "K":
            value = rng.randrange(0, len(state) + 4)
            answers.append(str(state[value - 1]) if 1 <= value <= len(state) else "NA")
        else:
            value = rng.randrange(-700, 701)
            if kind == "O":
                answers.append(str(bisect.bisect_left(state, value)))
            elif kind == "C":
                answers.append(str(bisect.bisect_right(state, value)))
            elif kind == "P":
                index = bisect.bisect_right(state, value) - 1
                answers.append(str(state[index]) if index >= 0 else "NA")
            elif kind == "S":
                index = bisect.bisect_left(state, value)
                answers.append(str(state[index]) if index < len(state) else "NA")
            else:
                answers.append(str(len(state)))
        operations.append(f"{kind} {value}")

    lines = [f"{len(universe)} {len(initial)} {len(operations)}"]
    lines.append(" ".join(map(str, universe)))
    lines.append(" ".join(map(str, initial)))
    lines.extend(operations)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
