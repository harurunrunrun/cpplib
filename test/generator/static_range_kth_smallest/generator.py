#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, queries):
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(f"{left} {right} {order}" for left, right, order in queries)
    answers = [
        str(sorted(values[left:right])[order])
        for left, right, order in queries
    ]
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
        ([], []),
        ([7], [(0, 1, 0)]),
        (
            [-(2**31), 5, 5, 0, 2**31 - 1],
            [(0, 5, 0), (0, 5, 4), (1, 4, 0), (1, 4, 2)],
        ),
    ]
    rng = random.Random(5160831)
    special = [-(2**31), -(10**9), -1, 0, 1, 10**9, 2**31 - 1]
    for _ in range(35):
        size = rng.randrange(1, 257)
        values = [
            rng.choice(special)
            if rng.randrange(4) == 0
            else rng.randrange(-(10**9), 10**9 + 1)
            for _ in range(size)
        ]
        queries = []
        for _ in range(200):
            left = rng.randrange(size)
            right = rng.randrange(left + 1, size + 1)
            order = rng.randrange(right - left)
            queries.append((left, right, order))
        cases.append((values, queries))

    for index, (values, queries) in enumerate(cases):
        write_case(out_dir, index, values, queries)


if __name__ == "__main__":
    main()
