#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, window):
    answer = [max(values[left:left + window]) for left in range(len(values) - window + 1)]
    input_text = f"{len(values)}\n{' '.join(map(str, values))}\n{window}\n"
    (out_dir / f"case_{index:02d}.in").write_text(input_text)
    (out_dir / f"case_{index:02d}.out").write_text(" ".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, 0, [-7], 1)
    write_case(out_dir, 1, [5, 5, 4, 9, -2, 9], 6)
    rng = random.Random(76004)
    values = [rng.randrange(-100, 101) for _ in range(101)]
    write_case(out_dir, 2, values, 17)


if __name__ == "__main__":
    main()
