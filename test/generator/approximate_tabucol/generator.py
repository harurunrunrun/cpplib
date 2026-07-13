#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int]
Case = tuple[int, list[Edge], int, int, int, int, bool, list[int] | None, bool]


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
            forbidden = {colors[x] for x in adjacency[vertex] if colors[x] >= 0}
            for color in range(min(color_count, used + 1)):
                if color in forbidden:
                    continue
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


def complete_bipartite(left_size: int, right_size: int) -> list[Edge]:
    return [
        (left, left_size + right)
        for left in range(left_size)
        for right in range(right_size)
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071319)
    complete_five = [
        (left, right) for left in range(5) for right in range(left + 1, 5)
    ]
    cases: list[Case] = [
        (0, [], 0, 0, 20, 7, True, None, True),
        (1, [], 1, 1, 20, 7, True, None, True),
        (6, [], 1, 2, 20, 7, True, None, True),
        (5, complete_five, 5, 3, 20, 7, True, None, True),
        (5, complete_five, 4, 4, 120, 7, True, None, False),
        (6, cycle(6), 2, 5, 200, 7, True, None, True),
        (5, cycle(5), 2, 6, 200, 7, False, None, False),
        (3, cycle(3), 3, 7, 20, 1, True, None, True),
        (9, complete_bipartite(4, 5), 2, 8, 200, 5, True, None, True),
        (3, [(0, 1), (1, 0), (0, 1), (1, 2)], 2, 9, 80, 3, True, None, True),
        (4, [(0, 1), (1, 2), (2, 3)], 2, 10, 100, 4, True, [0, 0, 0, 0], True),
        (3, [(0, 1), (1, 2)], 2, 11, 0, 4, True, [0, 0, 1], False),
    ]
    for _ in range(24):
        vertex_count = rng.randrange(2, 10)
        probability = rng.uniform(0.05, 0.62)
        edges: list[Edge] = []
        for left in range(vertex_count):
            for right in range(left + 1, vertex_count):
                if rng.random() < probability:
                    edges.append((left, right))
                    if rng.randrange(9) == 0:
                        edges.append((right, left))
        optimum = chromatic_number(adjacency_list(vertex_count, edges))
        target = optimum
        if optimum > 1 and rng.randrange(3) == 0:
            target = optimum - 1
        cases.append((
            vertex_count, edges, target, rng.getrandbits(64), 600, rng.randrange(1, 9),
            bool(rng.randrange(2)), None, False,
        ))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for identifier, case in enumerate(cases):
        vertex_count, edges, target, seed, iterations, tenure, aspiration, initial, required = case
        optimum = chromatic_number(adjacency_list(vertex_count, edges))
        input_lines.append(
            f"{identifier} {vertex_count} {len(edges)} {target} {seed} {iterations} "
            f"{tenure} {int(aspiration)} {optimum} {int(required)} "
            f"{int(initial is not None)}"
        )
        input_lines.extend(f"{left} {right}" for left, right in edges)
        if initial is not None:
            input_lines.append(" ".join(map(str, initial)))
        output_lines.append(f"OK {identifier}")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
