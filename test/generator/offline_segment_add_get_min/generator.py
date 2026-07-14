#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, initial, operations):
    segments = list(initial)
    answers = []
    for operation in operations:
        if operation[0] == 0:
            segments.append(operation[1:])
        else:
            x = operation[1]
            values = [
                slope * x + intercept
                for left, right, slope, intercept in segments
                if left <= x < right
            ]
            answers.append(str(min(values)) if values else "INFINITY")

    lines = [f"{len(initial)} {len(operations)}"]
    lines.extend(" ".join(map(str, segment)) for segment in initial)
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(answers) + ("\n" if answers else ""), encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([], [(1, 0), (0, 0, 1, 2, 3), (1, 0), (1, 1)]),
        (
            [(-5, 5, 1, 0), (-2, 3, -2, 4), (7, 7, 100, 100)],
            [(1, -5), (1, -2), (1, 3), (1, 5), (0, 3, 6, 0, -9), (1, 3)],
        ),
    ]
    rng = random.Random(6100101)
    for _ in range(28):
        initial = []
        for _ in range(rng.randrange(9)):
            left = rng.randrange(-12, 13)
            right = rng.randrange(left, 14)
            initial.append((left, right, rng.randrange(-8, 9), rng.randrange(-30, 31)))
        operations = []
        for _ in range(90):
            if rng.randrange(5) < 2:
                left = rng.randrange(-12, 13)
                right = rng.randrange(left, 14)
                operations.append((
                    0, left, right, rng.randrange(-8, 9), rng.randrange(-30, 31)
                ))
            else:
                operations.append((1, rng.randrange(-14, 15)))
        cases.append((initial, operations))

    for index, (initial, operations) in enumerate(cases):
        write_case(out_dir, index, initial, operations)


if __name__ == "__main__":
    main()
