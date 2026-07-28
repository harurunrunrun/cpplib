#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def maximum_flow(
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    sink: int,
) -> int:
    return min(
        sum(
            capacity
            for u, v, capacity in edges
            if mask >> u & 1 and not (mask >> v & 1)
        )
        for mask in range(1 << n)
        if mask >> source & 1 and not (mask >> sink & 1)
    )


def feasible_circulation(
    n: int, edges: list[tuple[int, int, int, int]]
) -> bool:
    ranges = [range(lower, upper + 1) for _, _, lower, upper in edges]
    for values in itertools.product(*ranges):
        balance = [0] * n
        for (u, v, _, _), value in zip(edges, values):
            balance[u] += value
            balance[v] -= value
        if not any(balance):
            return True
    return False


def add_component(
    edge_flow: dict[tuple[int, int], int],
    vertices: list[int],
    amount: int,
    cycle: bool,
) -> None:
    limit = len(vertices) if cycle else len(vertices) - 1
    for index in range(limit):
        edge = (vertices[index], vertices[(index + 1) % len(vertices)])
        edge_flow[edge] = edge_flow.get(edge, 0) + amount


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[int, tuple[object, ...]]] = [
        (0, (2, 0, 1, [(0, 1, 7)])),
        (1, (2, [(0, 1, 1, 2)])),
        (1, (2, [(0, 1, 1, 2), (1, 0, 0, 3)])),
        (2, (2, 0, 1, [(0, 1, 5)])),
    ]
    for _ in range(110):
        n = rng.randint(2, 8)
        source, sink = rng.sample(range(n), 2)
        edges = [
            (u, v, rng.randint(0, 60))
            for u in range(n)
            for v in range(n)
            if u != v and rng.random() < 0.25
        ]
        cases.append((0, (n, source, sink, edges)))
    for _ in range(120):
        n = rng.randint(1, 6)
        pairs = [
            (rng.randrange(n), rng.randrange(n))
            for _ in range(rng.randint(0, 7))
        ]
        edges = []
        for u, v in pairs:
            lower = rng.randint(0, 2)
            edges.append((u, v, lower, rng.randint(lower, 3)))
        cases.append((1, (n, edges)))
    for _ in range(120):
        n = rng.randint(2, 8)
        source, sink = rng.sample(range(n), 2)
        edge_flow: dict[tuple[int, int], int] = {}
        for _ in range(rng.randint(0, 5)):
            internal = rng.sample(
                [v for v in range(n) if v not in (source, sink)],
                rng.randint(0, min(4, n - 2)),
            )
            add_component(
                edge_flow,
                [source, *internal, sink],
                rng.randint(1, 12),
                False,
            )
        for _ in range(rng.randint(0, 5)):
            length = rng.randint(1, min(5, n))
            cycle = rng.sample(range(n), length)
            add_component(
                edge_flow, cycle, rng.randint(1, 10), True
            )
        edges = [(u, v, value) for (u, v), value in edge_flow.items()]
        rng.shuffle(edges)
        cases.append((2, (n, source, sink, edges)))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for kind, data in cases:
        if kind == 0:
            n, source, sink, edges = data
            typed_edges = list(edges)  # type: ignore[arg-type]
            input_lines.append(f"0 {n} {len(typed_edges)}")
            input_lines.append(f"{source} {sink}")
            input_lines.extend(
                f"{u} {v} {capacity}" for u, v, capacity in typed_edges
            )
            output_lines.append(str(maximum_flow(
                int(n), typed_edges, int(source), int(sink)
            )))
        elif kind == 1:
            n, edges = data
            typed_edges = list(edges)  # type: ignore[arg-type]
            input_lines.append(f"1 {n} {len(typed_edges)}")
            input_lines.extend(
                f"{u} {v} {lower} {upper}"
                for u, v, lower, upper in typed_edges
            )
            output_lines.append(str(int(feasible_circulation(
                int(n), typed_edges
            ))))
        else:
            n, source, sink, edges = data
            typed_edges = list(edges)  # type: ignore[arg-type]
            input_lines.append(f"2 {n} {len(typed_edges)}")
            input_lines.append(f"{source} {sink}")
            input_lines.extend(
                f"{u} {v} {flow}" for u, v, flow in typed_edges
            )
            path_total = sum(
                flow for u, _, flow in typed_edges if u == source
            ) - sum(
                flow for _, v, flow in typed_edges if v == source
            )
            output_lines.append(
                f"{path_total} {sum(flow for _, _, flow in typed_edges)}"
            )
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
