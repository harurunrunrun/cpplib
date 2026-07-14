#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, length, intervals, queries):
    lines = [f"{len(intervals)} {len(queries)} {length}"]
    lines.extend(f"{left} {right}" for left, right in intervals)
    lines.extend(map(str, queries))
    answers = [sum(left <= point <= right for left, right in intervals) for point in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, 0, 1, [(0, 0)], [0])
    rng = random.Random(76002)
    length = 41
    intervals = []
    for _ in range(70):
        left = rng.randrange(length)
        right = rng.randrange(left, length)
        intervals.append((left, right))
    queries = [0, length - 1] + [rng.randrange(length) for _ in range(80)]
    write_case(out_dir, 1, length, intervals, queries)


if __name__ == "__main__":
    main()
