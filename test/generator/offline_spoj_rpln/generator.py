#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for test, (values, queries) in enumerate(groups, 1):
        lines.append(f"{len(values)} {len(queries)}")
        lines.append(" ".join(map(str, values)))
        lines += [f"{left + 1} {right}" for left, right in queries]
        output.append(f"Scenario #{test}:")
        output += [str(min(values[left:right])) for left, right in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[([5], [(0, 1)])],
             [([3, 1, 4, 1, 5], [(0, 5), (1, 2), (2, 4)]),
              ([-7, -3], [(0, 1), (0, 2)])]]
    rng = random.Random(730010)
    for _ in range(8):
        groups = []
        for _ in range(rng.randint(1, 4)):
            n = rng.randint(1, 24)
            values = [rng.randint(-100, 100) for _ in range(n)]
            queries = []
            for _ in range(20):
                left = rng.randrange(n)
                right = rng.randrange(left + 1, n + 1)
                queries.append((left, right))
            groups.append((values, queries))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
