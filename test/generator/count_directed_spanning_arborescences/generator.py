#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def brute(adjacency: list[str]) -> int:
    vertex_count = len(adjacency)
    answer = 0
    for root in range(vertex_count):
        candidates = [
            [
                parent
                for parent in range(vertex_count)
                if parent != vertex and adjacency[parent][vertex] == "1"
            ]
            for vertex in range(vertex_count)
        ]
        parents = [-1] * vertex_count
        parents[root] = root

        def enumerate_parents(vertex: int) -> None:
            nonlocal answer
            if vertex == vertex_count:
                for start in range(vertex_count):
                    current = start
                    seen: set[int] = set()
                    while current != root and current != -1 and current not in seen:
                        seen.add(current)
                        current = parents[current]
                    if current != root:
                        return
                answer += 1
                return
            if vertex == root:
                enumerate_parents(vertex + 1)
                return
            for parent in candidates[vertex]:
                parents[vertex] = parent
                enumerate_parents(vertex + 1)
            parents[vertex] = -1

        enumerate_parents(0)
    return answer


def write_case(out_dir: Path, index: int, adjacency: list[str]) -> None:
    name = f"case_{index:02d}"
    (out_dir / f"{name}.in").write_text(
        f"{len(adjacency)}\n" + "\n".join(adjacency) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        f"{brute(adjacency)}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ["0"],
        ["1"],
        ["0100", "0010", "0001", "0000"],
        ["0100", "0010", "0001", "1000"],
        ["000", "000", "000"],
        ["01111", "10111", "11011", "11101", "11110"],
    ]
    random_source = random.Random(20260717)
    for _ in range(50):
        vertex_count = random_source.randrange(1, 7)
        cases.append(
            [
                "".join(
                    "1" if random_source.randrange(4) == 0 else "0"
                    for _ in range(vertex_count)
                )
                for _ in range(vertex_count)
            ]
        )

    for index, adjacency in enumerate(cases):
        write_case(args.out_dir, index, adjacency)

    index = len(cases)
    (args.out_dir / f"case_{index:02d}.in").write_text("", encoding="utf-8")
    (args.out_dir / f"case_{index:02d}.out").write_text("", encoding="utf-8")

    complete = ["1" * 8 for _ in range(8)]
    name = f"case_{index + 1:02d}"
    (args.out_dir / f"{name}.in").write_text(
        "8\n" + "\n".join(complete) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / f"{name}.out").write_text("2097152\n", encoding="utf-8")


if __name__ == "__main__":
    main()
