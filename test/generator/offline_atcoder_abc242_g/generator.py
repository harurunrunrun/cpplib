#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def write_case(out_dir, index, values, queries):
    lines = [str(len(values)), " ".join(map(str, values)), str(len(queries))]
    lines += [f"{left + 1} {right}" for left, right in queries]
    answer = []
    for left, right in queries:
        counts = collections.Counter(values[left:right])
        answer.append(sum(count // 2 for count in counts.values()))
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([1], [(0, 1)]), ([1, 1, 1, 1], [(0, 4), (1, 3), (2, 4)]),
             (list(range(1, 9)), [(0, 8), (3, 4)])]
    rng = random.Random(24207)
    for n in range(2, 28, 4):
        values = [rng.randint(1, n) for _ in range(n)]
        queries = []
        for _ in range(20):
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            queries.append((left, right))
        cases.append((values, queries))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
