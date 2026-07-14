#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def parent_array(values: list[int]) -> list[int]:
    parent = [-1] * len(values)

    def build(left: int, right: int, parent_vertex: int) -> None:
        if left == right:
            return
        root = min(range(left, right), key=lambda index: (values[index], index))
        parent[root] = root if parent_vertex == -1 else parent_vertex
        build(left, root, root)
        build(root + 1, right, root)

    build(0, len(values), -1)
    return parent


def write_case(out_dir: Path, index: int, values: list[int]) -> None:
    answer = parent_array(values)
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(values)}\n" + " ".join(map(str, values)) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [[0], [3, 1, 2], [5, 5, 5, 5], [4, 3, 2, 1], [1, 2, 3, 4]]
    for index, values in enumerate(fixed):
        write_case(out_dir, index, values)

    rng = random.Random(20260714)
    for index in range(len(fixed), len(fixed) + 20):
        size = rng.randrange(1, 61)
        values = [rng.randrange(-20, 21) for _ in range(size)]
        write_case(out_dir, index, values)


if __name__ == "__main__":
    main()
