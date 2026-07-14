#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def signatures(parents: list[int]) -> list[str]:
    children = [[] for _ in range(len(parents) + 1)]
    for vertex, parent in enumerate(parents, 1):
        children[parent].append(vertex)
    result = [""] * len(children)
    for vertex in range(len(children) - 1, -1, -1):
        result[vertex] = "(" + "".join(sorted(result[to] for to in children[vertex])) + ")"
    return result


def write_case(out_dir: Path, index: int, parents: list[int]) -> None:
    canonical = signatures(parents)
    class_count = len(set(canonical))
    lines = [str(len(parents) + 1)]
    if parents:
        lines.append(" ".join(map(str, parents)))
    expected = [str(class_count)]
    expected.extend(
        "".join("1" if left == right else "0" for right in canonical)
        for left in canonical
    )
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(expected) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [[], [0], [0, 0], [0, 1, 2, 3], [0, 0, 1, 1, 2, 2]]
    for index, parents in enumerate(fixed):
        write_case(out_dir, index, parents)

    rng = random.Random(20260714)
    for index in range(len(fixed), len(fixed) + 18):
        size = rng.randrange(1, 51)
        parents = [rng.randrange(vertex) for vertex in range(1, size)]
        write_case(out_dir, index, parents)


if __name__ == "__main__":
    main()
