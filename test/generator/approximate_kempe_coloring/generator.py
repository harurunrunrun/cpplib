#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]
Case = tuple[int, list[Edge], int, int, int, list[int] | None, bool]


def adjacency_list(vertex_count: int, input_edges: list[Edge]) -> list[list[int]]:
    adjacency = [set() for _ in range(vertex_count)]
    for left, right in input_edges:
        assert 0 <= left < vertex_count and 0 <= right < vertex_count and left != right
        adjacency[left].add(right)
        adjacency[right].add(left)
    return [sorted(neighbors) for neighbors in adjacency]


def chromatic_number(adjacency: list[list[int]]) -> int:
    vertex_count = len(adjacency)
    if vertex_count == 0:
        return 0
    order = sorted(range(vertex_count), key=lambda v: (-len(adjacency[v]), v))

    def feasible(color_count: int) -> bool:
        colors = [-1] * vertex_count

        def search(index: int, used: int) -> bool:
            if index == vertex_count:
                return True
            vertex = order[index]
            forbidden = {colors[x] for x in adjacency[vertex] if colors[x] != -1}
            for color in range(min(color_count, used + 1)):
                if color not in forbidden:
                    colors[vertex] = color
                    if search(index + 1, max(used, color + 1)):
                        return True
                    colors[vertex] = -1
            return False

        return search(0, 0)

    for color_count in range(1, vertex_count + 1):
        if feasible(color_count):
            return color_count
    raise AssertionError("unreachable")


def cycle(size: int) -> list[Edge]:
    return [(vertex, (vertex + 1) % size) for vertex in range(size)]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071318)
    cases: list[Case] = [
        (0, [], 0, 0, 20, None, True),
        (3, [(0, 1), (0, 2)], 2, 19, 10, [2, 0, 1], True),
        (4, [(0, 1)], 2, 23, 10, [2, 0, 1, 1], True),
        (4, [(left, right) for left in range(4) for right in range(left + 1, 4)], 4, 3, 10, None, True),
        (5, cycle(5), 2, 5, 20, None, False),
        (6, cycle(6), 2, 7, 20, None, True),
        (6, [(0, 1), (1, 2), (2, 0), (3, 4), (4, 5), (5, 3)], 3, 11, 20, None, True),
        (3, [(0, 1), (1, 0), (0, 1), (1, 2)], 2, 13, 20, None, True),
    ]
    for _ in range(18):
        vertex_count = rng.randrange(2, 10)
        probability = rng.uniform(0.08, 0.58)
        edges: list[Edge] = []
        for left in range(vertex_count):
            for right in range(left + 1, vertex_count):
                if rng.random() < probability:
                    edges.append((left, right))
        adjacency = adjacency_list(vertex_count, edges)
        optimum = chromatic_number(adjacency)
        cases.append((vertex_count, edges, optimum, rng.getrandbits(64), 30, None, False))
        if optimum > 1 and rng.randrange(2) == 0:
            cases.append((
                vertex_count, edges, optimum - 1, rng.getrandbits(64), 20, None, False
            ))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for identifier, case in enumerate(cases):
        vertex_count, edges, target, seed, maximum_iterations, initial, required = case
        optimum = chromatic_number(adjacency_list(vertex_count, edges))
        input_lines.append(
            f"{identifier} {vertex_count} {len(edges)} {target} {seed} "
            f"{maximum_iterations} {optimum} {int(required)} {int(initial is not None)}"
        )
        input_lines.extend(f"{left} {right}" for left, right in edges)
        if initial is not None:
            input_lines.append(" ".join(map(str, initial)))
        output_lines.append(f"OK {identifier}")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
