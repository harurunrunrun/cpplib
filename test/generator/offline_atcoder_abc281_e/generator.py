#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, window, k):
    answer = [sum(sorted(values[left:left + window])[:k])
              for left in range(len(values) - window + 1)]
    lines = [f"{len(values)} {window} {k}", " ".join(map(str, values))]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(" ".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([5], 1, 1), ([4, 1, 3, 2], 4, 2),
             ([2, 2, 2, 2, 2], 3, 2), ([5, 7, 2, 9], 2, 2)]
    rng = random.Random(28105)
    for n in range(2, 25, 3):
        values = [rng.randint(0, 40) for _ in range(n)]
        window = rng.randint(1, n)
        k = rng.randint(1, window)
        cases.append((values, window, k))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
