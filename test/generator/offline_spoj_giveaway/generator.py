#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, operation_count, rng):
    current = values[:]
    operations = []
    answers = []
    for step in range(operation_count):
        if step % 3 == 0:
            position = rng.randrange(len(values))
            value = rng.randrange(-30, 31)
            operations.append(f"1 {position + 1} {value}")
            current[position] = value
        else:
            left = rng.randrange(len(values))
            right = rng.randrange(left, len(values))
            threshold = rng.randrange(-32, 33)
            operations.append(f"0 {left + 1} {right + 1} {threshold}")
            answers.append(sum(value >= threshold for value in current[left:right + 1]))
    lines = [str(len(values)), " ".join(map(str, values)), str(len(operations))] + operations
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76017)
    write_case(out_dir, 0, [0], 20, rng)
    write_case(out_dir, 1, [rng.randrange(-30, 31) for _ in range(47)], 180, rng)


if __name__ == "__main__":
    main()
