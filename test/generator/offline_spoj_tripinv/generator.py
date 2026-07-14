#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values):
    n = len(values)
    answer = sum(values[i] > values[j] > values[k]
                 for i in range(n) for j in range(i + 1, n)
                 for k in range(j + 1, n))
    lines = [str(n), " ".join(map(str, values))]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{answer}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[1], list(range(1, 11)), list(range(10, 0, -1))]
    rng = random.Random(730019)
    for n in range(2, 32):
        values = list(range(1, n + 1))
        rng.shuffle(values)
        cases.append(values)
    for index, values in enumerate(cases):
        write_case(out_dir, index, values)


if __name__ == "__main__":
    main()
