#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path

Edge = tuple[str, str, int]
Case = tuple[list[Edge], str, str]


def solve(case: Case) -> int:
    edges, source, sink = case
    labels = sorted({
        source,
        sink,
        *(left for left, _, _ in edges),
        *(right for _, right, _ in edges),
    })
    index = {label: position for position, label in enumerate(labels)}
    capacity = [[0] * len(labels) for _ in labels]
    for left, right, value in edges:
        if left == right:
            continue
        left_id = index[left]
        right_id = index[right]
        capacity[left_id][right_id] += value
        capacity[right_id][left_id] += value

    source_id = index[source]
    sink_id = index[sink]
    answer = 0
    while True:
        parent = [-1] * len(labels)
        parent[source_id] = source_id
        queue = collections.deque([source_id])
        while queue and parent[sink_id] == -1:
            vertex = queue.popleft()
            for target, value in enumerate(capacity[vertex]):
                if value > 0 and parent[target] == -1:
                    parent[target] = vertex
                    queue.append(target)
        if parent[sink_id] == -1:
            return answer
        pushed = 10**30
        vertex = sink_id
        while vertex != source_id:
            pushed = min(pushed, capacity[parent[vertex]][vertex])
            vertex = parent[vertex]
        vertex = sink_id
        while vertex != source_id:
            previous = parent[vertex]
            capacity[previous][vertex] -= pushed
            capacity[vertex][previous] += pushed
            vertex = previous
        answer += pushed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[Case] = [
        ([], "@", "#"),
        ([("@", "x", 4), ("x", "#", 3)], "@", "#"),
        ([("1", "a", 5), ("a", "Z", 2), ("1", "Z", 1)], "1", "Z"),
        ([("%", "%", 99), ("%", "&", 7), ("%", "&", 8)], "%", "&"),
    ]
    labels = list("@#%&0123456789abcXYZ")
    source = random.Random(31007)
    for _ in range(100):
        used = source.sample(labels, source.randint(2, len(labels)))
        terminal_source, terminal_sink = source.sample(used, 2)
        edges = [
            (
                source.choice(used),
                source.choice(used),
                source.randint(0, 100),
            )
            for _ in range(source.randint(0, 80))
        ]
        cases.append((edges, terminal_source, terminal_sink))

    input_lines = [str(len(cases))]
    output_lines = []
    for edges, terminal_source, terminal_sink in cases:
        input_lines.append(
            f"{len(edges)} {terminal_source} {terminal_sink}"
        )
        input_lines.extend(
            f"{left} {right} {capacity}"
            for left, right, capacity in edges
        )
        output_lines.append(str(solve(
            (edges, terminal_source, terminal_sink)
        )))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


if __name__ == "__main__":
    main()
