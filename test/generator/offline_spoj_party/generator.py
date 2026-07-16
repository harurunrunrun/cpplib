#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Item = tuple[int, int]
Instance = tuple[int, list[Item]]


def brute_optimum(capacity: int, items: list[Item]) -> tuple[int, int]:
    best_cost = 0
    best_value = 0
    for mask in range(1 << len(items)):
        cost = 0
        value = 0
        for index, (item_cost, item_value) in enumerate(items):
            if mask >> index & 1:
                cost += item_cost
                value += item_value
        if cost <= capacity and (
            value > best_value or (value == best_value and cost < best_cost)
        ):
            best_cost = cost
            best_value = value
    return best_cost, best_value


def exact_cost_dp_optimum(capacity: int, items: list[Item]) -> tuple[int, int]:
    exact: list[int | None] = [None] * (capacity + 1)
    exact[0] = 0
    for item_cost, item_value in items:
        previous = exact.copy()
        for cost, value in enumerate(previous):
            next_cost = cost + item_cost
            if value is None or next_cost > capacity:
                continue
            candidate = value + item_value
            current = exact[next_cost]
            if current is None or candidate > current:
                exact[next_cost] = candidate

    best_cost = 0
    best_value = 0
    for cost, value in enumerate(exact):
        if value is not None and (
            value > best_value or (value == best_value and cost < best_cost)
        ):
            best_cost = cost
            best_value = value
    return best_cost, best_value


def write_case(
    directory: Path,
    name: str,
    instances: list[Instance],
    *,
    use_brute: bool,
) -> None:
    input_lines: list[str] = []
    output_lines: list[str] = []
    oracle = brute_optimum if use_brute else exact_cost_dp_optimum
    for capacity, items in instances:
        input_lines.append(f"{capacity} {len(items)}")
        input_lines.extend(f"{cost} {value}" for cost, value in items)
        minimum_cost, maximum_value = oracle(capacity, items)
        output_lines.append(f"{minimum_cost} {maximum_value}")
    input_lines.append("0 0")
    (directory / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (directory / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    directory = Path(parser.parse_args().out_dir)
    directory.mkdir(parents=True, exist_ok=True)

    write_case(directory, "case_00_empty", [(1, [])], use_brute=True)
    write_case(
        directory,
        "case_01_zero_budget",
        [(0, [(1, 10), (2, 20), (3, 30)])],
        use_brute=True,
    )
    write_case(
        directory,
        "case_02_ties",
        [
            (10, [(4, 10), (6, 10), (8, 20)]),
            (3, [(3, 0), (4, 100)]),
            (7, [(0, 2), (0, 3), (7, 5), (6, 5)]),
        ],
        use_brute=True,
    )

    rng = random.Random(2026071601)
    for case_index in range(3, 11):
        instances: list[Instance] = []
        for _ in range(30):
            capacity = rng.randrange(0, 31)
            item_count = rng.randrange(0, 16)
            items = [
                (rng.randrange(0, 36), rng.randrange(0, 51))
                for _ in range(item_count)
            ]
            if capacity == 0 and not items:
                items.append((1, 0))
            instances.append((capacity, items))
        write_case(
            directory,
            f"case_{case_index:02d}_brute_random",
            instances,
            use_brute=True,
        )

    boundary_items = [
        ((index * 37) % 501, (index * 97) % 101)
        for index in range(100)
    ]
    boundary_items[0] = (500, 100)
    boundary_items[1] = (501, 10_000)
    write_case(
        directory,
        "case_11_boundary_dp",
        [(500, boundary_items)],
        use_brute=False,
    )

    for case_index in range(12, 16):
        instances = []
        for _ in range(8):
            capacity = rng.randrange(100, 501)
            item_count = rng.randrange(40, 101)
            items = [
                (rng.randrange(0, 551), rng.randrange(0, 101))
                for _ in range(item_count)
            ]
            instances.append((capacity, items))
        write_case(
            directory,
            f"case_{case_index:02d}_dp_random",
            instances,
            use_brute=False,
        )


if __name__ == "__main__":
    main()
