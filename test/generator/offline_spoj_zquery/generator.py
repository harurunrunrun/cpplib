#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def longest_zero_sum(values, left, right):
    answer = 0
    for begin in range(left, right):
        total = 0
        for end in range(begin, right):
            total += values[end]
            if total == 0:
                answer = max(answer, end - begin + 1)
    return answer


def write_case(out_dir, index, values, queries):
    answer = [longest_zero_sum(values, left, right) for left, right in queries]
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines += [f"{left + 1} {right}" for left, right in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([1], [(0, 1)]), ([1, -1, 1, -1], [(0, 4), (1, 3), (0, 1)])]
    rng = random.Random(730023)
    for n in range(2, 28):
        values = [rng.choice([-1, 1]) for _ in range(n)]
        queries = []
        for _ in range(30):
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            queries.append((left, right))
        cases.append((values, queries))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
