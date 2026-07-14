#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def span(values):
    result = {0}
    for value in values:
        result |= {current ^ value for current in result}
    return sorted(result)


def write_case(out_dir, index, values, left, right):
    ordered = span(values)
    lines = [f"{len(values)} {left} {right}", " ".join(map(str, values))]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    expected = ordered[left - 1:right]
    (out_dir / f"case_{index:02d}.out").write_text(" ".join(map(str, expected)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([0], 1, 1), ([1], 1, 2), ([1, 2, 3], 1, 4),
             ([7, 7, 7], 1, 2), ([1 << 59, (1 << 59) - 1], 2, 4)]
    rng = random.Random(28307)
    for n in range(2, 11):
        values = [rng.randrange(1 << 14) for _ in range(n)]
        size = len(span(values))
        left = rng.randint(1, size)
        right = rng.randint(left, size)
        cases.append((values, left, right))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
