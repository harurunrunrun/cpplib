#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


INT_MIN = -(2**31)
INT_MAX = 2**31 - 1


def write_case(
    out_dir: Path,
    case_index: int,
    values: list[int],
    queries: list[tuple[int, int, int]],
) -> None:
    answers: list[str] = []
    for left, right, upper in queries:
        selected = [value for value in values[left:right] if value <= upper]
        answers.append(f"{len(selected)} {sum(selected)}")

    stem = f"case_{case_index:03d}"
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(" ".join(map(str, query)) for query in queries)
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{stem}.out").write_text(
        "\n".join(answers) + ("\n" if answers else ""),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [], [(0, 0, INT_MAX), (0, 0, INT_MIN)])
    write_case(
        out_dir,
        1,
        [INT_MIN, -1, 0, INT_MAX],
        [
            (0, 4, INT_MAX),
            (0, 4, INT_MIN),
            (1, 3, -1),
            (2, 2, INT_MAX),
        ],
    )

    rng = random.Random(2026071504)
    boundaries = [INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX - 1, INT_MAX]
    for case_index in range(2, 42):
        size = rng.randrange(0, 129)
        values = [
            rng.choice(boundaries) if rng.randrange(5) == 0
            else rng.randrange(INT_MIN, INT_MAX + 1)
            for _ in range(size)
        ]
        queries: list[tuple[int, int, int]] = []
        for _ in range(rng.randrange(1, 180)):
            left = rng.randrange(size + 1)
            right = rng.randrange(left, size + 1)
            upper = (
                rng.choice(boundaries)
                if rng.randrange(3) == 0
                else rng.randrange(INT_MIN, INT_MAX + 1)
            )
            queries.append((left, right, upper))
        write_case(out_dir, case_index, values, queries)


if __name__ == "__main__":
    main()
