#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import heapq
import random
from fractions import Fraction
from pathlib import Path

Edge = tuple[int, int, int]
Case = tuple[list[list[int]], list[Edge]]


def graph_data(task_count: int, edges: list[Edge]):
    outgoing: list[list[int]] = [[] for _ in range(task_count)]
    incoming: list[list[int]] = [[] for _ in range(task_count)]
    indegree = [0] * task_count
    for index, (source, target, _) in enumerate(edges):
        outgoing[source].append(index)
        incoming[target].append(index)
        indegree[target] += 1
    ready = [task for task in range(task_count) if indegree[task] == 0]
    heapq.heapify(ready)
    topological: list[int] = []
    while ready:
        task = heapq.heappop(ready)
        topological.append(task)
        for edge_index in outgoing[task]:
            target = edges[edge_index][1]
            indegree[target] -= 1
            if indegree[target] == 0:
                heapq.heappush(ready, target)
    return outgoing, incoming, topological


def priorities(
    matrix: list[list[int]], edges: list[Edge], rule: str
) -> tuple[list[Fraction], list[int], int, list[list[int]], list[list[int]]]:
    task_count = len(matrix)
    if task_count == 0:
        return [], [], -1, [], []
    processor_count = len(matrix[0])
    outgoing, incoming, topological = graph_data(task_count, edges)
    if rule == "critical":
        processing_rank = [Fraction(min(row)) for row in matrix]
    else:
        processing_rank = [Fraction(sum(row), processor_count) for row in matrix]
    upward = [Fraction(0) for _ in range(task_count)]
    for task in reversed(topological):
        successor = max(
            (Fraction(edges[index][2]) + upward[edges[index][1]]
             for index in outgoing[task]),
            default=Fraction(0),
        )
        upward[task] = processing_rank[task] + successor
    if rule != "cpop":
        return upward, [], -1, outgoing, incoming

    downward = [Fraction(0) for _ in range(task_count)]
    for task in topological:
        for edge_index in outgoing[task]:
            _, target, communication = edges[edge_index]
            candidate = downward[task] + processing_rank[task] + communication
            downward[target] = max(downward[target], candidate)
    priority = [upward[task] + downward[task] for task in range(task_count)]
    sources = [task for task in range(task_count) if not incoming[task]]
    current = min(sources, key=lambda task: (-upward[task], task))
    path: list[int] = []
    while True:
        path.append(current)
        if not outgoing[current]:
            break
        edge_index = min(
            outgoing[current],
            key=lambda index: (
                -(Fraction(edges[index][2]) + upward[edges[index][1]]),
                edges[index][1],
            ),
        )
        current = edges[edge_index][1]
    critical_processor = min(
        range(processor_count),
        key=lambda processor: (sum(matrix[task][processor] for task in path), processor),
    )
    return priority, path, critical_processor, outgoing, incoming


def earliest_slot(
    slots: list[tuple[int, int, int]], ready_time: int, duration: int
) -> tuple[int, int]:
    start = ready_time
    for slot_start, slot_finish, _ in slots:
        if start <= slot_start and duration <= slot_start - start:
            return start, start + duration
        if start < slot_finish:
            start = slot_finish
    return start, start + duration


def schedule(matrix: list[list[int]], edges: list[Edge], rule: str):
    task_count = len(matrix)
    if task_count == 0:
        return 0, [], [], [], [], -1, []
    processor_count = len(matrix[0])
    priority, path, critical_processor, outgoing, incoming = priorities(
        matrix, edges, rule
    )
    critical = set(path)
    indegree = [len(incoming[task]) for task in range(task_count)]
    ready = [(-priority[task], task) for task in range(task_count) if indegree[task] == 0]
    heapq.heapify(ready)
    processor_of_task = [-1] * task_count
    start_time = [0] * task_count
    finish_time = [0] * task_count
    order: list[int] = []
    slots: list[list[tuple[int, int, int]]] = [[] for _ in range(processor_count)]
    while ready:
        _, task = heapq.heappop(ready)
        processors = [critical_processor] if task in critical else range(processor_count)
        choices = []
        for processor in processors:
            task_ready = 0
            for edge_index in incoming[task]:
                source, _, communication = edges[edge_index]
                arrival = finish_time[source]
                if processor_of_task[source] != processor:
                    arrival += communication
                task_ready = max(task_ready, arrival)
            start, finish = earliest_slot(
                slots[processor], task_ready, matrix[task][processor]
            )
            choices.append((finish, start, processor))
        finish, start, processor = min(choices)
        processor_of_task[task] = processor
        start_time[task] = start
        finish_time[task] = finish
        order.append(task)
        bisect.insort(slots[processor], (start, finish, task))
        for edge_index in outgoing[task]:
            target = edges[edge_index][1]
            indegree[target] -= 1
            if indegree[target] == 0:
                heapq.heappush(ready, (-priority[target], target))
    return (
        max(finish_time, default=0),
        processor_of_task,
        start_time,
        finish_time,
        order,
        critical_processor,
        path,
    )


def output_line(result) -> str:
    makespan, processors, starts, finishes, order, critical_processor, path = result
    values = [
        makespan,
        len(processors),
        *processors,
        *starts,
        *finishes,
        *order,
        critical_processor,
        len(path),
        *path,
    ]
    return " ".join(map(str, values))


def write_cases(path: Path, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for matrix, edges in cases:
        processors = len(matrix[0]) if matrix else 0
        input_lines.append(f"{len(matrix)} {processors} {len(edges)}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        input_lines.extend(" ".join(map(str, edge)) for edge in edges)
        for rule in ("critical", "heft", "cpop"):
            output_lines.append(output_line(schedule(matrix, edges, rule)))
    path.with_suffix(".in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    deterministic: list[Case] = [
        ([], []),
        ([[7]], []),
        ([[0, 0], [0, 1], [1, 0]], [(0, 2, 0), (1, 2, 0)]),
        ([[100, 1], [15, 100], [10, 10]], [(0, 1, 20)]),
        ([[8, 2], [4, 9], [7, 3], [1, 8]], [(0, 1, 4), (0, 2, 2), (1, 3, 7), (2, 3, 1)]),
        ([[3, 8], [5, 2], [4, 7]], [(0, 2, 1), (0, 2, 6), (1, 2, 3)]),
        ([[9, 1, 7], [4, 8, 3], [5, 2, 6], [0, 9, 4]], [(2, 3, 5)]),
    ]
    write_cases(out_dir / "case_00", deterministic)

    rng = random.Random(2026071331)
    random_cases: list[Case] = []
    while len(random_cases) < 48:
        task_count = rng.randrange(1, 13)
        processor_count = rng.randrange(1, 6)
        matrix = [
            [rng.randrange(0, 81) for _ in range(processor_count)]
            for _ in range(task_count)
        ]
        permutation = list(range(task_count))
        rng.shuffle(permutation)
        edges: list[Edge] = []
        for left in range(task_count):
            for right in range(left + 1, task_count):
                if rng.random() < 0.19:
                    edge = (permutation[left], permutation[right], rng.randrange(0, 41))
                    edges.append(edge)
                    if rng.random() < 0.04:
                        edges.append((edge[0], edge[1], rng.randrange(0, 41)))
        # The oracle uses exact rationals while the library exposes normalized
        # long-double ranks.  Keep random rank comparisons strict; deliberate
        # equal-priority cases remain in case_00 and exercise the ID tie-break.
        all_priorities = [priorities(matrix, edges, rule)[0]
                          for rule in ("critical", "heft", "cpop")]
        if any(len(set(rank)) != len(rank) for rank in all_priorities):
            continue
        random_cases.append((matrix, edges))
    write_cases(out_dir / "case_01", random_cases)


if __name__ == "__main__":
    main()
