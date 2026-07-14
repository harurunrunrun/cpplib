#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Edge = tuple[int, int, int]
Case = tuple[int, list[Edge], list[Edge], int, int, int]


def random_distinct_pair(
    rng: random.Random, vertex_count: int
) -> tuple[int, int]:
    first = rng.randrange(vertex_count)
    second = rng.randrange(vertex_count - 1)
    if second >= first:
        second += 1
    return first, second


def state_graph_oracle(
    vertex_count: int,
    edges: list[Edge],
    candidates: list[Edge],
    source: int,
    target: int,
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
) -> Case:
    return (
        vertex_count,
        edges,
        candidates,
        source,
        target,
        state_graph_oracle(
            vertex_count, edges, candidates, source, target
        ),
    )


def validate_case(case: Case) -> None:
    vertex_count, edges, candidates, source, target, _ = case
    assert 2 <= vertex_count <= 10_000
    assert 1 <= len(edges) <= 100_000
    assert 1 <= len(candidates) < 300
    assert 0 <= source < vertex_count
    assert 0 <= target < vertex_count
    assert source != target
    for from_vertex, to_vertex, cost in edges:
        assert 0 <= from_vertex < vertex_count
        assert 0 <= to_vertex < vertex_count
        assert 1 <= cost <= 1000
    for from_vertex, to_vertex, cost in candidates:
        assert 0 <= from_vertex < vertex_count
        assert 0 <= to_vertex < vertex_count
        assert 1 <= cost <= 1000


def write_suite(out_dir: Path, stem: str, cases: list[Case]) -> None:
    assert 1 <= len(cases) <= 20
    for case in cases:
        validate_case(case)

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for vertex_count, edges, candidates, source, target, answer in cases:
        input_lines.append(
            f"{vertex_count} {len(edges)} {len(candidates)} "
            f"{source + 1} {target + 1}"
        )
        input_lines.extend(f"{u + 1} {v + 1} {w}" for u, v, w in edges)
        input_lines.extend(
            f"{u + 1} {v + 1} {w}" for u, v, w in candidates
        )
        output_lines.append(str(answer))
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def official_sample() -> Case:
    case = make_case(
        4,
        [(0, 1, 13), (1, 2, 19), (2, 0, 25), (2, 3, 17), (3, 0, 18)],
        [(0, 2, 23), (1, 2, 5), (1, 3, 25)],
        0,
        3,
    )
    assert case[-1] == 35
    return case


def maximum_case() -> Case:
    vertex_count = 10_000
    edges = [
        (from_vertex, (from_vertex + step) % vertex_count, 1000)
        for step in range(1, 11)
        for from_vertex in range(vertex_count)
    ]
    candidates: list[Edge] = [(0, vertex_count - 1, 1)]
    for index in range(1, 299):
        from_vertex = index
        to_vertex = (index * 37 + 123) % vertex_count
        if to_vertex == from_vertex:
            to_vertex = (to_vertex + 1) % vertex_count
        candidates.append((from_vertex, to_vertex, 1000))
    return vertex_count, edges, candidates, 0, vertex_count - 1, 1


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(out_dir, "official_sample", [official_sample()])

    hand_cases = [
        make_case(4, [(0, 2, 3), (1, 3, 4)], [(1, 2, 2)], 0, 3),
        make_case(4, [(0, 1, 3), (2, 3, 4)], [(1, 2, 2)], 0, 3),
        make_case(4, [(0, 1, 1)], [(1, 2, 1)], 0, 3),
        make_case(3, [(0, 1, 9), (0, 1, 2), (1, 2, 8)],
                  [(0, 2, 30), (0, 2, 5)], 0, 2),
        make_case(3, [(0, 2, 4)], [(0, 1, 10), (1, 2, 10)], 0, 2),
    ]
    write_suite(out_dir, "orientations_unreachable_parallel", hand_cases)

    maximum_test_count = [
        make_case(2, [(0, 1, 100)], [(0, 1, index + 1)], 0, 1)
        for index in range(20)
    ]
    write_suite(out_dir, "maximum_test_count", maximum_test_count)

    rng = random.Random(2026071400102)
    for suite_index in range(5):
        cases: list[Case] = []
        for _ in range(20):
            vertex_count = rng.randrange(2, 9)
            source, target = rng.sample(range(vertex_count), 2)
            edges: list[Edge] = []
            for _ in range(rng.randrange(1, 3 * vertex_count + 1)):
                from_vertex, to_vertex = random_distinct_pair(
                    rng, vertex_count
                )
                edges.append((from_vertex, to_vertex, rng.randrange(1, 31)))
            candidates: list[Edge] = []
            for _ in range(rng.randrange(1, vertex_count + 1)):
                from_vertex, to_vertex = random_distinct_pair(
                    rng, vertex_count
                )
                candidates.append(
                    (from_vertex, to_vertex, rng.randrange(1, 31))
                )
            cases.append(
                make_case(
                    vertex_count, edges, candidates, source, target
                )
            )
        write_suite(out_dir, f"random_state_graph_{suite_index:02d}", cases)

    write_suite(out_dir, "maximum_n_m_k", [maximum_case()])


if __name__ == "__main__":
    main()
