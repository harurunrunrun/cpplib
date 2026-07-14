#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, queries):
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(f"{left} {right}" for left, right in queries)
    answers = [str(sum(values[left:right])) for left, right in queries]
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
        ([], [(0, 0)]),
        ([7], [(0, 0), (0, 1), (1, 1)]),
        ([3, -1, 4, -1, 5], [(0, 5), (1, 4), (2, 2)]),
    ]
    rng = random.Random(9137701)
    for _ in range(30):
        n = rng.randrange(1, 257)
        values = [rng.randrange(-10**9, 10**9 + 1) for _ in range(n)]
        queries = []
        for _ in range(200):
            left = rng.randrange(n + 1)
            right = rng.randrange(left, n + 1)
            queries.append((left, right))
        cases.append((values, queries))
    for index, (values, queries) in enumerate(cases):
        write_case(out_dir, index, values, queries)


if __name__ == "__main__":
    main()
