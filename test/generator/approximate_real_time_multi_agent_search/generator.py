#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import itertools
import random
from collections import deque
from pathlib import Path

Point = tuple[int, int]


def adjacent(grid: list[str], point: Point) -> list[Point]:
    rows, columns = len(grid), len(grid[0])
    result = [point]
    for dr, dc in ((-1, 0), (0, 1), (1, 0), (0, -1)):
        nxt = (point[0] + dr, point[1] + dc)
        if (
            0 <= nxt[0] < rows
            and 0 <= nxt[1] < columns
            and grid[nxt[0]][nxt[1]] != "#"
        ):
            result.append(nxt)
    return result


def single_distance(grid: list[str], start: Point, goal: Point) -> int:
    queue = deque([start])
    distance = {start: 0}
    while queue:
        current = queue.popleft()
        if current == goal:
            return distance[current]
        for nxt in adjacent(grid, current)[1:]:
            if nxt not in distance:
                distance[nxt] = distance[current] + 1
                queue.append(nxt)
    return -1


def exact_sum_of_costs(
    grid: list[str], starts: tuple[Point, ...], goals: tuple[Point, ...]
) -> int:
    queue: list[tuple[int, int, tuple[Point, ...]]] = [(0, 0, starts)]
    distance = {starts: 0}
    order = 1
    while queue:
        cost, _, state = heapq.heappop(queue)
        if distance[state] != cost:
            continue
        if state == goals:
            return cost
        choices = [
            [position] if position == goals[agent] else adjacent(grid, position)
            for agent, position in enumerate(state)
        ]
        for nxt in itertools.product(*choices):
            if len(set(nxt)) != len(nxt):
                continue
            if any(
                state[first] == nxt[second] and state[second] == nxt[first]
                for first in range(len(state))
                for second in range(first + 1, len(state))
            ):
                continue
            candidate = cost + sum(
                state[agent] != goals[agent] for agent in range(len(state))
            )
            if candidate < distance.get(nxt, 10**9):
                distance[nxt] = candidate
                heapq.heappush(queue, (candidate, order, nxt))
                order += 1
    return -1


def write_case(
    output: Path,
    index: int,
    grid: list[str],
    starts: tuple[Point, ...],
    goals: tuple[Point, ...],
) -> None:
    single = single_distance(grid, starts[0], goals[0])
    optimum = exact_sum_of_costs(grid, starts, goals)
    assert single >= 0 and optimum >= 0
    lines = [f"{len(grid)} {len(grid[0])}", *grid, str(len(starts))]
    lines.extend(
        f"{start[0]} {start[1]} {goal[0]} {goal[1]}"
        for start, goal in zip(starts, goals)
    )
    stem = f"case_{index:02d}"
    (output / f"{stem}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (output / f"{stem}.out").write_text(
        f"{single} {optimum}\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    output = Path(arguments.out_dir)
    output.mkdir(parents=True, exist_ok=True)
    cases = [
        (
            ["....", ".##.", "...."],
            ((0, 0), (2, 0)),
            ((0, 3), (2, 3)),
        ),
        (
            ["...", "...", "..."],
            ((0, 0), (0, 2)),
            ((0, 2), (0, 0)),
        ),
        (
            ["....", ".#..", "....", "..#."],
            ((0, 0), (3, 3)),
            ((3, 3), (0, 0)),
        ),
    ]
    engine = random.Random(20260729)
    while len(cases) < 10:
        rows = engine.randrange(3, 6)
        columns = engine.randrange(3, 6)
        mutable = [
            ["#" if engine.random() < 0.15 else "." for _ in range(columns)]
            for _ in range(rows)
        ]
        free = [
            (row, column)
            for row in range(rows)
            for column in range(columns)
            if mutable[row][column] == "."
        ]
        if len(free) < 4:
            continue
        selected = engine.sample(free, 4)
        starts = tuple(selected[:2])
        goals = tuple(selected[2:])
        grid = ["".join(row) for row in mutable]
        if (
            single_distance(grid, starts[0], goals[0]) >= 0
            and exact_sum_of_costs(grid, starts, goals) >= 0
        ):
            cases.append((grid, starts, goals))
    for index, case in enumerate(cases):
        write_case(output, index, *case)


if __name__ == "__main__":
    main()
