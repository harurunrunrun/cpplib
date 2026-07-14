#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import math
import random
from pathlib import Path

Spark = tuple[int, int, int, list[int]]


def solve(sparks: list[Spark]) -> list[float]:
    spark_count = len(sparks)
    times = [math.inf] * spark_count
    lit = [False] * spark_count
    events: list[tuple[float, int]] = []

    def activate(source: int) -> None:
        x, y, arrow_count, order = sparks[source]
        remaining = arrow_count
        for target in order:
            if remaining == 0:
                break
            if lit[target]:
                continue
            target_x, target_y, _, _ = sparks[target]
            distance = math.sqrt((x - target_x) ** 2 + (y - target_y) ** 2)
            heapq.heappush(events, (times[source] + distance, target))
            remaining -= 1

    times[0] = 0.0
    lit[0] = True
    activate(0)
    while events:
        time, target = heapq.heappop(events)
        if lit[target]:
            continue
        lit[target] = True
        times[target] = time
        activate(target)
    assert all(lit)
    return times


def write_case(out_dir: Path, name: str, sparks: list[Spark]) -> None:
    input_lines = [str(len(sparks))]
    for x, y, arrow_count, order in sparks:
        input_lines.append(
            f"{x} {y} {arrow_count} " + " ".join(str(target + 1) for target in order)
        )
    output_lines = [f"{time:.6f}" for time in solve(sparks)]
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def random_case(source: random.Random, spark_count: int) -> list[Spark]:
    coordinates = source.sample(
        [(x, y) for x in range(1, 101) for y in range(1, 101)],
        spark_count,
    )
    result = []
    for spark in range(spark_count):
        order = [target for target in range(spark_count) if target != spark]
        source.shuffle(order)
        if spark + 1 < spark_count:
            order.remove(spark + 1)
            order.insert(0, spark + 1)
        result.append((*coordinates[spark], source.randint(1, min(5, spark_count)), order))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official_1", [
        (1, 1, 1, [1, 2, 3]),
        (1, 2, 1, [3, 0, 2]),
        (2, 1, 1, [1, 0, 3]),
        (2, 2, 1, [2, 1, 0]),
    ])
    write_case(out_dir, "official_2", [
        (4, 3, 2, [4, 1, 3, 2]),
        (4, 5, 1, [3, 0, 4, 2]),
        (4, 4, 1, [0, 3, 4, 1]),
        (2, 4, 1, [4, 1, 2, 0]),
        (3, 4, 2, [1, 3, 2, 0]),
    ])

    source = random.Random(142)
    for case_index in range(30):
        spark_count = source.randint(2, 40)
        write_case(out_dir, f"random_{case_index:02d}", random_case(source, spark_count))

    spark_count = 100
    maximum = []
    for spark in range(spark_count):
        order = [target for target in range(spark_count) if target != spark]
        order = order[spark % len(order):] + order[:spark % len(order)]
        maximum.append((spark * 10 + 1, (spark * spark * 37) % 997 + 1, 100, order))
    write_case(out_dir, "maximum", maximum)


if __name__ == "__main__":
    main()
