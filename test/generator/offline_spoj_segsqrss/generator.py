#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for test, (initial, operations) in enumerate(groups, 1):
        values = initial[:]
        lines.append(f"{len(values)} {len(operations)}")
        lines.append(" ".join(map(str, values)))
        output.append(f"Case {test}:")
        for operation in operations:
            kind, left, right = operation[:3]
            if kind == 0:
                value = operation[3]
                for position in range(left, right):
                    values[position] = value
                lines.append(f"0 {left + 1} {right} {value}")
            elif kind == 1:
                value = operation[3]
                for position in range(left, right):
                    values[position] += value
                lines.append(f"1 {left + 1} {right} {value}")
            else:
                output.append(str(sum(value * value for value in values[left:right])))
                lines.append(f"2 {left + 1} {right}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[([5], [(2, 0, 1), (1, 0, 1, -3), (2, 0, 1),
                      (0, 0, 1, -4), (2, 0, 1)])]]
    rng = random.Random(730013)
    for _ in range(10):
        groups = []
        for _ in range(rng.randint(1, 3)):
            n = rng.randint(1, 18)
            initial = [rng.randint(-15, 15) for _ in range(n)]
            operations = []
            for _ in range(45):
                kind = rng.randrange(3)
                left = rng.randrange(n)
                right = rng.randrange(left + 1, n + 1)
                if kind < 2:
                    operations.append((kind, left, right, rng.randint(-15, 15)))
                else:
                    operations.append((2, left, right))
            groups.append((initial, operations))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
