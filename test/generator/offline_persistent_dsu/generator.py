#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def leader(parent: list[int], vertex: int) -> int:
    while parent[vertex] >= 0:
        vertex = parent[vertex]
    return vertex


def merge(parent: list[int], left: int, right: int) -> None:
    left = leader(parent, left)
    right = leader(parent, right)
    if left == right:
        return
    if parent[left] > parent[right]:
        left, right = right, left
    parent[left] += parent[right]
    parent[right] = left


def write_case(
    out_dir: Path, index: int, size: int, operations: list[tuple[int, int, int, int]]
) -> None:
    states = [[-1] * size]
    version_at = [0]
    answers: list[int] = []
    for operation, (kind, base_operation, left, right) in enumerate(operations):
        base = version_at[base_operation + 1]
        if kind == 0:
            state = states[base][:]
            merge(state, left, right)
            states.append(state)
            version_at.append(len(states) - 1)
        else:
            answers.append(int(leader(states[base], left) == leader(states[base], right)))
            version_at.append(base)
    lines = [f"{size} {len(operations)}"]
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    name = f"case_{index:03d}"
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + ("\n" if answers else ""),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 1, [(1, -1, 0, 0)])
    write_case(out_dir, 1, 4, [(0, -1, 0, 1), (1, 0, 0, 1), (1, -1, 0, 1)])
    rng = random.Random(2026071403)
    for index in range(2, 32):
        size = rng.randrange(1, 31)
        operations: list[tuple[int, int, int, int]] = []
        for operation in range(rng.randrange(20, 180)):
            kind = rng.randrange(2)
            base = rng.randrange(-1, operation) if operation else -1
            operations.append((kind, base, rng.randrange(size), rng.randrange(size)))
        write_case(out_dir, index, size, operations)


if __name__ == "__main__":
    main()
