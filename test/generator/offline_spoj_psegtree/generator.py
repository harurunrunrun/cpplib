#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, initial, operations):
    versions = [initial[:]]
    output = []
    for operation in operations:
        if operation[0] == 1:
            _, version, position, delta = operation
            values = versions[version][:]
            values[position] += delta
            versions.append(values)
        else:
            _, version, left, right = operation
            output.append(sum(versions[version][left:right]))
    lines = [str(len(initial)), " ".join(map(str, initial)), str(len(operations))]
    for operation in operations:
        if operation[0] == 1:
            _, version, position, delta = operation
            lines.append(f"1 {version} {position + 1} {delta}")
        else:
            _, version, left, right = operation
            lines.append(f"2 {version} {left + 1} {right}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, output)) + "\n")


def random_case(rng, n, count):
    initial = [rng.randint(-20, 20) for _ in range(n)]
    operations = []
    version_count = 1
    for _ in range(count):
        if rng.randrange(3) != 0:
            version = rng.randrange(version_count)
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            operations.append((2, version, left, right))
        else:
            operations.append((1, rng.randrange(version_count), rng.randrange(n),
                               rng.randint(-15, 15)))
            version_count += 1
    return initial, operations


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        ([5], [(2, 0, 0, 1), (1, 0, 0, 7), (2, 0, 0, 1), (2, 1, 0, 1)]),
        ([1, 2, 3, 4], [(1, 0, 1, 10), (1, 0, 2, -5),
                         (2, 1, 0, 4), (2, 2, 0, 4)]),
    ]
    rng = random.Random(730002)
    for n in range(2, 13):
        cases.append(random_case(rng, n, 35))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
