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


def write_case(out_dir, index, values, query_count, rng):
    queries = []
    for _ in range(query_count):
        left = rng.randrange(len(values))
        right = rng.randrange(left, len(values))
        queries.append((left, right))
    lines = [str(len(values)), " ".join(map(str, values)), str(query_count)]
    lines.extend(f"{left + 1} {right + 1}" for left, right in queries)
    answers = [max_subarray(values[left:right + 1]) for left, right in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76020)
    write_case(out_dir, 0, [-10], 5, rng)
    write_case(out_dir, 1, [rng.randrange(-100, 101) for _ in range(83)], 180, rng)


if __name__ == "__main__":
    main()
