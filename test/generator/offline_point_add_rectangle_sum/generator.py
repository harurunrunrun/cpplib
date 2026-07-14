#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, initial, operations):
    points = list(initial)
    answers = []
    for operation in operations:
        if operation[0] == 0:
            points.append(operation[1:])
        else:
            x1, y1, x2, y2 = operation[1:]
            answers.append(sum(
                value
                for x, y, value in points
                if x1 <= x < x2 and y1 <= y < y2
            ))

    lines = [f"{len(initial)} {len(operations)}"]
    lines.extend(" ".join(map(str, point)) for point in initial)
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "".join(f"{answer}\n" for answer in answers), encoding="utf-8"
    )


def random_point(rng):
    return rng.randrange(-10, 11), rng.randrange(-10, 11), rng.randrange(-20, 21)


def random_query(rng):
    x1, x2 = sorted((rng.randrange(-12, 13), rng.randrange(-12, 13)))
    y1, y2 = sorted((rng.randrange(-12, 13), rng.randrange(-12, 13)))
    return 1, x1, y1, x2, y2


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([], [(1, 0, 0, 0, 5), (0, 0, 0, 3), (1, 0, 0, 1, 1)]),
        (
            [(-1, -1, 4), (0, 0, 7), (1, 1, -2)],
            [(1, -1, -1, 1, 1), (0, 0, 0, -3), (1, -5, -5, 5, 5)],
        ),
    ]
    rng = random.Random(6100103)
    for _ in range(28):
        initial = [random_point(rng) for _ in range(rng.randrange(9))]
        operations = []
        for _ in range(90):
            if rng.randrange(5) < 2:
                operations.append((0, *random_point(rng)))
            else:
                operations.append(random_query(rng))
        cases.append((initial, operations))

    for index, (initial, operations) in enumerate(cases):
        write_case(out_dir, index, initial, operations)


if __name__ == "__main__":
    main()
