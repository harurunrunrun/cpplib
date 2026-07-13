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
    queries: list[tuple[int, int]],
    periodic: int | None = None,
) -> None:
    output: list[str] = []
    for left, right in queries:
        if periodic is None:
            output.append(str(len(set(values[left:right]))))
        else:
            output.append(str(min(right - left, periodic)))
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(f"{left} {right}" for left, right in queries)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [4, 4, -1, 7, 4, -1], [(0, 0), (0, 6), (1, 5), (2, 4), (5, 6)])
    for index, seed in enumerate([2026071471, 2026071472, 2026071473], start=1):
        rng = random.Random(seed)
        n = 500
        values = [rng.randrange(-80, 81) for _ in range(n)]
        queries = []
        for _ in range(4000):
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            queries.append((left, right))
        write_case(out_dir, index, values, queries)

    rng = random.Random(2026071474)
    n = 120000
    period = 2000
    values = [index % period for index in range(n)]
    queries = []
    for _ in range(30000):
        left = rng.randrange(n + 1)
        right = rng.randrange(left, n + 1)
        queries.append((left, right))
    write_case(out_dir, 4, values, queries, period)


if __name__ == "__main__":
    main()
