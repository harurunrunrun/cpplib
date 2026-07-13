#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


def solve(
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
    k: int,
) -> list[int]:
    graph: list[list[tuple[int, int]]] = [[] for _ in range(n)]
    for from_, to, cost in edges:
        graph[from_].append((to, cost))

    result: list[int] = []
    queue: list[tuple[int, int]] = [(0, source)]
    pop_count = [0] * n
    while queue and len(result) < k:
        distance, vertex = heapq.heappop(queue)
        if pop_count[vertex] == k:
            continue
        pop_count[vertex] += 1
        if vertex == target:
            result.append(distance)
        for to, cost in graph[vertex]:
            heapq.heappush(queue, (distance + cost, to))
    return result


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    edges: list[tuple[int, int, int]],
    source: int,
    target: int,
    k: int,
    expected: list[int] | None = None,
) -> None:
    costs = solve(n, edges, source, target, k) if expected is None else expected
    assert len(costs) <= k
    name = f"case_{index:02d}"
    (out_dir / f"{name}.in").write_text(
        "\n".join(
            [
                f"{n} {len(edges)} {source} {target} {k}",
                *[f"{from_} {to} {cost}" for from_, to, cost in edges],
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    kth_value = str(costs[k - 1]) if k > 0 and len(costs) == k else "NONE"
    (out_dir / f"{name}.out").write_text(
        f"{len(costs)}\n" + " ".join(map(str, costs)) + f"\n{kth_value}\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[tuple[int, list[tuple[int, int, int]], int, int, int]] = [
        (1, [], 0, 0, 3),
        (3, [(0, 1, 1), (0, 2, 2), (1, 2, 1), (1, 1, 2)], 0, 2, 4),
        (4, [(0, 1, 1), (2, 3, 1)], 0, 3, 5),
        (2, [(0, 0, 0), (0, 1, 7), (0, 1, 7)], 0, 1, 201),
        (2, [(0, 1, 0), (1, 0, 0), (0, 1, 0)], 0, 1, 80),
        (1, [(0, 0, 0)], 0, 0, 100),
        (3, [(0, 1, 5), (0, 1, 5), (1, 2, 0)], 0, 2, 3),
        (
            4,
            [(0, 1, 2), (1, 2, 0), (2, 1, 0), (1, 3, 3), (2, 3, 3), (2, 3, 3)],
            0,
            3,
            250,
        ),
        (2, [(0, 1, 2), (1, 0, 3), (0, 0, 7), (0, 0, 7)], 0, 0, 120),
        (
            5,
            [
                (0, 1, 1),
                (0, 1, 1),
                (0, 2, 2),
                (1, 3, 2),
                (1, 4, 8),
                (2, 3, 1),
                (3, 4, 3),
            ],
            0,
            4,
            20,
        ),
    ]

    rng = random.Random(20260730)
    for n in range(1, 9):
        for _ in range(4):
            edges: list[tuple[int, int, int]] = []
            for from_ in range(n):
                for to in range(n):
                    if rng.randrange(5) == 0:
                        cost = rng.randrange(6)
                        edges.append((from_, to, cost))
                        if rng.randrange(5) == 0:
                            edges.append(
                                (
                                    from_,
                                    to,
                                    cost if rng.randrange(2) == 0 else rng.randrange(6),
                                )
                            )
            cases.append((n, edges, rng.randrange(n), rng.randrange(n), 20))

    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)

    # A zero loop before a long chain has K equal answers. Its expected output
    # is analytic so the generator itself does not perform the old O(NK) search.
    large_n = 30000
    large_k = 20000
    large_edges = [(0, 0, 0)]
    large_edges.extend(
        (vertex, vertex + 1, 1) for vertex in range(large_n - 1)
    )
    write_case(
        out_dir,
        len(cases),
        large_n,
        large_edges,
        0,
        large_n - 1,
        large_k,
        [large_n - 1] * large_k,
    )


if __name__ == "__main__":
    main()
