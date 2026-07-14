#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def max_subarray(values):
    best = values[0]
    current = values[0]
    for value in values[1:]:
        current = max(value, current + value)
        best = max(best, current)
    return best


def write_case(out_dir, index, values, operation_count, rng):
    current = values[:]
    operations = []
    answers = []
    for step in range(operation_count):
        if step % 3 == 0:
            position = rng.randrange(len(values))
            value = rng.randrange(-100, 101)
            operations.append(f"0 {position + 1} {value}")
            current[position] = value
        else:
            left = rng.randrange(len(values))
            right = rng.randrange(left, len(values))
            operations.append(f"1 {left + 1} {right + 1}")
            answers.append(max_subarray(current[left:right + 1]))
    lines = [str(len(values)), " ".join(map(str, values)), str(operation_count)] + operations
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76022)
    write_case(out_dir, 0, [-1], 20, rng)
    write_case(out_dir, 1, [rng.randrange(-100, 101) for _ in range(67)], 180, rng)


if __name__ == "__main__":
    main()
