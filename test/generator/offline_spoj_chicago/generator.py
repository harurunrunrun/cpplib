#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[int, list[tuple[int, int, int]], float | None]


def solve_independent(vertex_count: int, edges: list[tuple[int, int, int]]) -> float:
    graph: list[list[tuple[int, float]]] = [[] for _ in range(vertex_count)]
    for left, right, percent in edges:
        probability = percent / 100.0
        graph[left].append((right, probability))
        graph[right].append((left, probability))

    answer = 0.0
    visited = [False] * vertex_count
    visited[0] = True

    def enumerate_simple_paths(vertex: int, probability: float) -> None:
        nonlocal answer
        if vertex == vertex_count - 1:
            answer = max(answer, probability)
            return
        for neighbor, edge_probability in graph[vertex]:
            if not visited[neighbor]:
                visited[neighbor] = True
                enumerate_simple_paths(neighbor, probability * edge_probability)
                visited[neighbor] = False

    enumerate_simple_paths(0, 1.0)
    return answer


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[Case],
    *,
    final_newline: bool = True,
) -> None:
    input_lines: list[str] = []
    output_lines: list[str] = []
    for vertex_count, edges, answer in cases:
        assert 2 <= vertex_count <= 100
        assert 1 <= len(edges) <= vertex_count * (vertex_count - 1) // 2
        input_lines.append(f"{vertex_count} {len(edges)}")
        input_lines.extend(
            f"{left + 1} {right + 1} {percent}"
            for left, right, percent in edges
        )
        if answer is None:
            answer = solve_independent(vertex_count, edges)
        assert answer is not None
        output_lines.append(f"{answer * 100.0:.6f} percent")
    input_lines.append("0")
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(
        out_dir,
        "official",
        [
            (
                5,
                [
                    (4, 1, 100),
                    (2, 4, 80),
                    (1, 2, 70),
                    (1, 0, 50),
                    (2, 3, 90),
                    (3, 0, 85),
                    (2, 0, 70),
                ],
                0.612,
            )
        ],
    )

    rng = random.Random(100000109)
    random_cases: list[Case] = []
    for _ in range(220):
        vertex_count = rng.randint(2, 8)
        edges: list[tuple[int, int, int]] = []
        used: set[tuple[int, int]] = set()
        for vertex in range(1, vertex_count):
            parent = rng.randrange(vertex)
            edges.append((parent, vertex, rng.randint(1, 100)))
            used.add((parent, vertex))
        possible = [
            (left, right)
            for left in range(vertex_count)
            for right in range(left + 1, vertex_count)
            if (left, right) not in used
        ]
        rng.shuffle(possible)
        for left, right in possible[: rng.randint(0, min(6, len(possible)))]:
            edges.append((left, right, rng.randint(1, 100)))
        random_cases.append((vertex_count, edges, None))
    write_suite(out_dir, "random_independent", random_cases)

    chain = [(vertex, vertex + 1, 99) for vertex in range(99)]
    chain_answer = 1.0
    for _ in range(99):
        chain_answer *= 0.99
    write_suite(out_dir, "maximum_vertices", [(100, chain, chain_answer)])

    complete: list[tuple[int, int, int]] = []
    for left in range(100):
        for right in range(left + 1, 100):
            percent = 100 if (left, right) == (0, 99) else 1
            complete.append((left, right, percent))
    write_suite(
        out_dir,
        "maximum_edges",
        [(100, complete, 1.0)],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
