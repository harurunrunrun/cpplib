#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def path_edges(graph, source, target):
    parent = [-1] * len(graph)
    parent[source] = source
    stack = [source]
    while stack:
        vertex = stack.pop()
        for next_vertex in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                stack.append(next_vertex)
    result = []
    while target != source:
        result.append(tuple(sorted((target, parent[target]))))
        target = parent[target]
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76019)
    size = 37
    edges = [(rng.randrange(vertex), vertex) for vertex in range(1, size)]
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    count = {tuple(sorted(edge)): 0 for edge in edges}
    operations = []
    answers = []
    for step in range(180):
        if step % 3 != 2:
            left = rng.randrange(size)
            right = rng.randrange(size)
            operations.append(f"P {left + 1} {right + 1}")
            for edge in path_edges(graph, left, right):
                count[edge] += 1
        else:
            left, right = rng.choice(edges)
            if rng.randrange(2):
                left, right = right, left
            operations.append(f"Q {left + 1} {right + 1}")
            answers.append(count[tuple(sorted((left, right)))])
    lines = [f"{size} {len(operations)}"]
    lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    lines.extend(operations)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(map(str, answers)) + "\n")
    (out_dir / "case_01.in").write_text(
        "2 6\n1 2\nQ 1 2\nP 1 2\nQ 2 1\nP 2 1\nQ 1 2\nQ 2 1\n"
    )
    (out_dir / "case_01.out").write_text("0\n1\n2\n2\n")



if __name__ == "__main__":
    main()
