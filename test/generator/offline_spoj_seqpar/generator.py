#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import itertools
import random
from pathlib import Path


def solve(values, partition_count):
    best = None
    for cuts in itertools.combinations(range(1, len(values)), partition_count - 1):
        boundaries = (0,) + cuts + (len(values),)
        candidate = max(sum(values[boundaries[i]:boundaries[i + 1]])
                        for i in range(partition_count))
        if best is None or candidate < best:
            best = candidate
    return best


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for values, partition_count in groups:
        lines.append(f"{len(values)} {partition_count}")
        lines.append(" ".join(map(str, values)))
        output.append(str(solve(values, partition_count)))
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[([7], 1)], [([1, 2, 3, 4], 4), ([-5, 10, -5], 2),
                           ([-8, -2, -7, -1], 2)]]
    rng = random.Random(730014)
    for _ in range(12):
        groups = []
        for _ in range(rng.randint(1, 4)):
            n = rng.randint(1, 12)
            groups.append(([rng.randint(-20, 20) for _ in range(n)],
                           rng.randint(1, n)))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
