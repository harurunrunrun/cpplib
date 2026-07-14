#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, initial, operations):
    values = initial[:]
    output = []
    for operation, argument in operations:
        if operation == 1:
            values[argument] += 1
        elif operation == 2:
            output.append(sum(value >= argument for value in values))
        else:
            values = [value - 1 if value >= argument else value for value in values]
    lines = [f"{len(initial)} {len(operations)}", " ".join(map(str, initial))]
    for operation, argument in operations:
        lines.append(f"{operation} {argument + 1 if operation == 1 else argument}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, output)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([0], [(2, 0), (1, 0), (2, 1), (3, 1), (2, 1)]),
             ([2, 2, 5], [(1, 0), (1, 1), (2, 3), (3, 3), (2, 2)])]
    rng = random.Random(730015)
    for n in range(2, 22):
        initial = [rng.randint(0, 20) for _ in range(n)]
        operations = []
        for _ in range(55):
            kind = rng.randint(1, 3)
            argument = rng.randrange(n) if kind == 1 else rng.randint(0, 25)
            operations.append((kind, argument))
        cases.append((initial, operations))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
