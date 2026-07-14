#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]
Case = tuple[int, list[Edge], list[Edge], int, int, bool, int]


def state_graph_oracle(
    vertex_count: int,
    edges: list[Edge],
    candidates: list[Edge],
    source: int,
    target: int,
    bidirectional: bool,
) -> int:
    state_count = 2 * vertex_count
    infinity = 10**30
    distance = [infinity] * state_count
    distance[source] = 0

    transitions: list[Edge] = []
    for from_vertex, to_vertex, cost in edges:
        transitions.append((from_vertex, to_vertex, cost))
        transitions.append(
            (from_vertex + vertex_count, to_vertex + vertex_count, cost)
        )
    for from_vertex, to_vertex, cost in candidates:
        transitions.append((from_vertex, to_vertex + vertex_count, cost))
        if bidirectional:
            transitions.append((to_vertex, from_vertex + vertex_count, cost))

    for _ in range(state_count - 1):
        changed = False
        for from_vertex, to_vertex, cost in transitions:
            candidate = distance[from_vertex] + cost
            if candidate < distance[to_vertex]:
                distance[to_vertex] = candidate
                changed = True
        if not changed:
            break
    answer = min(distance[target], distance[target + vertex_count])
    return -1 if answer == infinity else answer


def make_case(
    vertex_count: int,
    edges: list[Edge],
    candidates: list[Edge],
    source: int,
    target: int,
    bidirectional: bool,
) -> Case:
    return (
        vertex_count,
        edges,
        candidates,
        source,
        target,
        bidirectional,
        state_graph_oracle(
            vertex_count, edges, candidates, source, target, bidirectional
        ),
    )


def write_suite(out_dir: Path, stem: str, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, candidates, source, target, bidirectional, answer in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} {len(candidates)} "
            f"{source} {target} {int(bidirectional)}"
        )
        input_lines.extend(f"{u} {v} {w}" for u, v, w in edges)
        input_lines.extend(f"{u} {v} {w}" for u, v, w in candidates)
        output_lines.append(str(answer))
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    hand_cases = [
        make_case(4, [(0, 2, 3), (1, 3, 4)], [(1, 2, 2)], 0, 3, False),
        make_case(4, [(0, 2, 3), (1, 3, 4)], [(1, 2, 2)], 0, 3, True),
        make_case(4, [(0, 1, 5), (2, 3, 7)], [(1, 2, 3)], 0, 3, False),
        make_case(3, [(0, 1, 1), (0, 1, 8), (1, 2, 9)],
                  [(0, 2, 20), (0, 2, 4)], 0, 2, False),
        make_case(4, [(0, 1, 1)], [(1, 2, 1)], 0, 3, True),
        make_case(2, [], [], 0, 0, False),
    ]
    write_suite(out_dir, "hand", hand_cases)

    rng = random.Random(202607140102)
    random_cases: list[Case] = []
    for _ in range(300):
        vertex_count = rng.randrange(2, 9)
        edges = [
            (
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randrange(21),
            )
            for _ in range(rng.randrange(0, 3 * vertex_count + 1))
        ]
        candidates = [
            (
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                rng.randrange(21),
            )
            for _ in range(rng.randrange(0, vertex_count + 1))
        ]
        random_cases.append(
            make_case(
                vertex_count,
                edges,
                candidates,
                rng.randrange(vertex_count),
                rng.randrange(vertex_count),
                bool(rng.randrange(2)),
            )
        )
    write_suite(out_dir, "random_state_graph", random_cases)

    vertex_count = 20_000
    large_edges = [
        (vertex, vertex + 1, 3) for vertex in range(vertex_count - 1)
    ]
    write_suite(
        out_dir,
        "large_linear",
        [(
            vertex_count,
            large_edges,
            [(0, vertex_count - 1, 1)],
            0,
            vertex_count - 1,
            False,
            1,
        )],
    )


if __name__ == "__main__":
    main()
