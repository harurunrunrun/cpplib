#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, initial, operations):
    values = initial[:]
    output = []
    for operation in operations:
        if operation[0] == "M":
            values[operation[1]] = operation[2]
        else:
            _, left, right, threshold = operation
            output.append(sum(value <= threshold for value in values[left:right]))
    lines = [f"{len(initial)} {len(operations)}", " ".join(map(str, initial))]
    for operation in operations:
        if operation[0] == "M":
            lines.append(f"M {operation[1] + 1} {operation[2]}")
        else:
            lines.append(f"C {operation[1] + 1} {operation[2]} {operation[3]}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, output)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([5], [("C", 0, 1, 4), ("C", 0, 1, 5),
                     ("M", 0, -2), ("C", 0, 1, -2)])]
    rng = random.Random(730008)
    for n in range(2, 19):
        initial = [rng.randint(-20, 20) for _ in range(n)]
        operations = []
        for _ in range(50):
            if rng.randrange(3) == 0:
                operations.append(("M", rng.randrange(n), rng.randint(-20, 20)))
            else:
                left = rng.randrange(n)
                right = rng.randrange(left + 1, n + 1)
                operations.append(("C", left, right, rng.randint(-25, 25)))
        cases.append((initial, operations))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
