#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def path_vertices(graph, source, target):
    parent = [-1] * len(graph)
    parent[source] = source
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        if vertex == target:
            break
        for neighbor in graph[vertex]:
            if parent[neighbor] == -1:
                parent[neighbor] = vertex
                queue.append(neighbor)
    result = []
    current = target
    while current != source:
        result.append(current)
        current = parent[current]
    result.append(source)
    result.reverse()
    return result


def maximum_subarray_allow_empty(sequence):
    answer = current = 0
    for value in sequence:
        current = max(0, current + value)
        answer = max(answer, current)
    return answer


def make_case(seed, n, query_count):
    rng = random.Random(seed)
    values = [rng.randint(-12, 12) for _ in range(n)]
    initial = values[:]
    graph = [[] for _ in range(n)]
    edges = []
    for vertex in range(1, n):
        parent = rng.randrange(vertex)
        graph[parent].append(vertex)
        graph[vertex].append(parent)
        edges.append((parent, vertex))
    operations = []
    answers = []
    for index in range(query_count):
        left = rng.randrange(n)
        right = rng.randrange(n)
        path = path_vertices(graph, left, right)
        if index % 3:
            operations.append(f"1 {left + 1} {right + 1}")
            answers.append(str(maximum_subarray_allow_empty([values[v] for v in path])))
        else:
            value = rng.randint(-10, 10)
            operations.append(f"2 {left + 1} {right + 1} {value}")
            for vertex in path:
                values[vertex] = value
    lines = [str(n), " ".join(map(str, initial))]
    lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    lines.extend((str(len(operations)), *operations))
    return "\n".join(lines) + "\n", "\n".join(answers) + ("\n" if answers else "")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, parameters in enumerate(((7007, 1, 20), (7019, 8, 70), (7027, 14, 100))):
        data, expected = make_case(*parameters)
        (out_dir / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
        (out_dir / f"case_{index:02d}.out").write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
