#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def path_weights(graph, source, target):
    parent = [None] * len(graph)
    parent[source] = (source, 0)
    stack = [source]
    while stack:
        vertex = stack.pop()
        if vertex == target:
            break
        for next_vertex, weight in graph[vertex]:
            if parent[next_vertex] is None:
                parent[next_vertex] = (vertex, weight)
                stack.append(next_vertex)
    weights = []
    while target != source:
        target, weight = parent[target]
        weights.append(weight)
    return weights


def write_case(out_dir, index, size, query_count, rng):
    edges = [(rng.randrange(vertex), vertex, rng.randrange(1, 1000))
             for vertex in range(1, size)]
    graph = [[] for _ in range(size)]
    for left, right, weight in edges:
        graph[left].append((right, weight))
        graph[right].append((left, weight))
    queries = []
    for _ in range(query_count):
        left = rng.randrange(size)
        right = rng.randrange(size - 1)
        if right >= left:
            right += 1
        queries.append((left, right))
    lines = [str(size)]
    lines.extend(f"{left + 1} {right + 1} {weight}" for left, right, weight in edges)
    lines.append(str(query_count))
    lines.extend(f"{left + 1} {right + 1}" for left, right in queries)
    answers = []
    for left, right in queries:
        weights = path_weights(graph, left, right)
        answers.append(f"{min(weights)} {max(weights)}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(answers) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76012)
    write_case(out_dir, 0, 2, 5, rng)
    write_case(out_dir, 1, 37, 120, rng)


if __name__ == "__main__":
    main()
