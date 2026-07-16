#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(
    out_dir: Path,
    index: int,
    values: list[int],
    queries: list[tuple[int, ...]],
) -> None:
    current = values[:]
    answers: list[str] = []
    for query in queries:
        if query[0] == 1:
            _, left, right, value = query
            current[left:right] = [value] * (right - left)
        else:
            answers.append(str(current.count(query[1])))
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(" ".join(map(str, query)) for query in queries)
    (out_dir / f"case_{index:03d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:03d}.out").write_text(
        "\n".join(answers) + ("\n" if answers else ""), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    write_case(args.out_dir, 0, [], [(2, 0), (1, 0, 0, 5), (2, 5)])
    write_case(
        args.out_dir,
        1,
        [1, 1, 2, 2, 3, 3],
        [(2, 1), (1, 1, 5, 7), (2, 1), (2, 7), (1, 0, 6, -4), (2, -4)],
    )

    rng = random.Random(2026071616)
    for case_index in range(2, 42):
        size = rng.randint(1, 250)
        values = [rng.randint(-20, 20) for _ in range(size)]
        queries: list[tuple[int, ...]] = []
        for _ in range(1000):
            if rng.randrange(3) != 0:
                left = rng.randrange(size + 1)
                right = rng.randrange(size + 1)
                if left > right:
                    left, right = right, left
                queries.append((1, left, right, rng.randint(-20, 20)))
            else:
                queries.append((2, rng.randint(-25, 25)))
        write_case(args.out_dir, case_index, values, queries)

    size = 500000
    values = [index & 1 for index in range(size)]
    queries = [
        (2, 0),
        (2, 1),
        (1, 1, size - 1, 9),
        (2, 9),
        (2, 0),
        (2, 1),
    ]
    write_case(args.out_dir, 42, values, queries)


if __name__ == "__main__":
    main()
