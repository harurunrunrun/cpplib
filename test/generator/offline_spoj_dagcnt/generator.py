#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def count_arborescences(adjacency: list[str]) -> int:
    vertex_count = len(adjacency)
    answer = 0
    for root in range(vertex_count):
        vertices = [vertex for vertex in range(vertex_count) if vertex != root]
        choices = [
            [
                parent
                for parent in range(vertex_count)
                if parent != vertex and adjacency[parent][vertex] == "1"
            ]
            for vertex in vertices
        ]
        if any(not parents for parents in choices):
            continue
        for selected in itertools.product(*choices):
            children = [[] for _ in range(vertex_count)]
            for vertex, parent in zip(vertices, selected):
                children[parent].append(vertex)
            reached = [False] * vertex_count
            stack = [root]
            reached[root] = True
            while stack:
                vertex = stack.pop()
                for child in children[vertex]:
                    if not reached[child]:
                        reached[child] = True
                        stack.append(child)
            answer += all(reached)
    return answer


def write_file(
    out_dir: Path,
    name: str,
    cases: list[list[str]],
    answers: list[int] | None = None,
) -> None:
    lines: list[str] = []
    for adjacency in cases:
        lines.append(str(len(adjacency)))
        lines.extend(adjacency)
    lines.append("0")
    if answers is None:
        answers = [count_arborescences(adjacency) for adjacency in cases]
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="ascii",
    )


def random_adjacency(source: random.Random, vertex_count: int) -> list[str]:
    return [
        "".join(
            "0" if row == column else str(source.randrange(2))
            for column in range(vertex_count)
        )
        for row in range(vertex_count)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_file(out_dir, "boundaries", [
        ["0"],
        ["01", "00"],
        ["01", "10"],
        ["011", "101", "110"],
        ["010", "001", "100"],
    ])

    source = random.Random(148)
    write_file(
        out_dir,
        "random",
        [
            random_adjacency(source, source.randint(1, 5))
            for _ in range(55)
        ],
    )

    complete = [
        "".join("0" if row == column else "1" for column in range(8))
        for row in range(8)
    ]
    write_file(out_dir, "maximum_complete", [complete], [8 ** 7])


if __name__ == "__main__":
    main()
