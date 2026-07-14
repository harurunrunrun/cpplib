#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, a, b):
    n = len(a)
    answer = sum(a[i] >= a[j] and b[i] <= b[j] for i in range(n) for j in range(n))
    lines = [str(n), " ".join(map(str, a)), " ".join(map(str, b))]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{answer}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([1], [1]), ([1, 1, 1, 1], [2, 2, 2, 2]),
             ([1, 2, 3, 4], [4, 3, 2, 1]), ([1, 2, 3, 4], [1, 2, 3, 4])]
    rng = random.Random(23106)
    for n in range(2, 22, 3):
        cases.append(([rng.randint(1, 11) for _ in range(n)],
                      [rng.randint(1, 11) for _ in range(n)]))
    for index, (a, b) in enumerate(cases):
        write_case(out_dir, index, a, b)


if __name__ == "__main__":
    main()
