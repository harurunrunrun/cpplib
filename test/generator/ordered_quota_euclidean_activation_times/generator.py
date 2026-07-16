#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import math
import random
from pathlib import Path

Activator = tuple[int, int, int, list[int]]
Case = tuple[list[Activator], int]


def solve(case: Case) -> list[float]:
    activators, initial = case
    vertex_count = len(activators)
    times = [math.inf] * vertex_count
    active = [False] * vertex_count
    events: list[tuple[float, int]] = []

    def schedule(source: int) -> None:
        x, y, quota, order = activators[source]
        remaining = quota
        for target in order:
            if remaining == 0:
                break
            if active[target]:
                continue
            target_x, target_y, _, _ = activators[target]
            heapq.heappush(
                events,
                (
                    times[source] + math.hypot(
                        x - target_x, y - target_y
                    ),
                    target,
                ),
            )
            remaining -= 1

    times[initial] = 0.0
    active[initial] = True
    schedule(initial)
    while events:
        time, target = heapq.heappop(events)
        if active[target]:
            continue
        active[target] = True
        times[target] = time
        schedule(target)
    return times


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        ([(10, 0, 0, [])], 0),
        (
            [
                (0, 0, 0, [1, 2]),
                (10, 0, 1, [2, 0]),
                (30, 0, 1, [1, 0]),
            ],
            1,
        ),
        (
            [
                (0, 0, 2, [1, 2, 3]),
                (10, 0, 1, [2, 3, 0]),
                (20, 0, 1, [3, 1, 0]),
                (35, 0, 0, [2, 1, 0]),
            ],
            0,
        ),
    ]
    source = random.Random(31005)
    for _ in range(100):
        vertex_count = source.randint(2, 25)
        coordinates = source.sample(range(-500, 501), vertex_count)
        activators: list[Activator] = []
        for vertex in range(vertex_count):
            order = [
                target for target in range(vertex_count)
                if target != vertex
            ]
            source.shuffle(order)
            activators.append(
                (
                    coordinates[vertex],
                    0,
                    source.randint(0, min(5, vertex_count)),
                    order,
                )
            )
        cases.append((activators, source.randrange(vertex_count)))

    input_lines = [str(len(cases))]
    output_lines = []
    for activators, initial in cases:
        input_lines.append(f"{len(activators)} {initial}")
        for x, y, quota, order in activators:
            suffix = " ".join(map(str, order))
            input_lines.append(
                f"{x} {y} {quota}" + (f" {suffix}" if suffix else "")
            )
        output_lines.append(" ".join(
            "INF" if math.isinf(time) else f"{time:.6f}"
            for time in solve((activators, initial))
        ))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
