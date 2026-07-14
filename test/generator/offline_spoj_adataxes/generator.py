#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path

MOD = 1_000_000_007


def write_case(out_dir, index, heights, queries):
    lines = [f"{len(heights)} {len(queries)}", " ".join(map(str, heights))]
    lines.extend(f"{left} {right} {limit}" for left, right, limit in queries)
    answers = []
    for left, right, limit in queries:
        product = 1
        for value in heights[left:right + 1]:
            if value <= limit:
                product = product * value % MOD
        answers.append(product)
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, 0, [0], [(0, 0, -1), (0, 0, 0), (0, 0, 1)])
    rng = random.Random(76003)
    heights = [rng.randrange(0, 31) for _ in range(43)]
    queries = []
    for _ in range(100):
        left = rng.randrange(len(heights))
        right = rng.randrange(left, len(heights))
        queries.append((left, right, rng.randrange(-1, 33)))
    write_case(out_dir, 1, heights, queries)


if __name__ == "__main__":
    main()
