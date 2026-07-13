#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def lis_length(values: list[int], strict: bool) -> int:
    tails: list[int] = []
    for value in values:
        position = bisect.bisect_left(tails, value) if strict else bisect.bisect_right(tails, value)
        if position == len(tails):
            tails.append(value)
        else:
            tails[position] = value
    return len(tails)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    for case_id, strict in enumerate((True, False)):
        values = [rng.randrange(-300, 301) for _ in range(2000)]
        input_text = f"{len(values)} {int(strict)}\n" + " ".join(map(str, values)) + "\n"
        output_text = f"{lis_length(values, strict)}\n"
        (out_dir / f"case_{case_id:02d}.in").write_text(input_text, encoding="utf-8")
        (out_dir / f"case_{case_id:02d}.out").write_text(output_text, encoding="utf-8")


if __name__ == "__main__":
    main()
