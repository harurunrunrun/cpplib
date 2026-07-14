#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def inversion_count(values):
    return sum(values[left] > values[right] for left in range(len(values)) for right in range(left + 1, len(values)))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(17170)
    cases = [[], [1], [5, 4, 3, 2, 1], [2, 2, 1, 1], list(range(20))]
    for n in (3, 12, 40):
        cases.append([rng.randint(-20, 20) for _ in range(n)])
    lines = [str(len(cases))]
    for values in cases:
        lines.append(str(len(values)))
        lines.extend(map(str, values))
    answers = [str(inversion_count(values)) for values in cases]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
