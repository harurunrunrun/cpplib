#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def path_vertices(graph, source, target):
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
        result.append(target)
        target = parent[target]
    result.append(source)
    return result


def build_case(size, query_count, rng):
    values = [rng.randrange(-6, 7) for _ in range(size)]
    edges = [(rng.randrange(vertex), vertex) for vertex in range(1, size)]
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    queries = []
    answers = []
    for step in range(query_count):
        left = rng.randrange(size)
        right = rng.randrange(size)
        path = path_vertices(graph, left, right)
        target = values[rng.choice(path)] if step % 2 == 0 else rng.randrange(20, 30)
        queries.append((left, right, target))
        answers.append("Find" if any(values[vertex] == target for vertex in path) else "NotFind")
    lines = [f"{size} {query_count}", " ".join(map(str, values))]
    lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    lines.extend(f"{left + 1} {right + 1} {target}" for left, right, target in queries)
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76018)
    cases = [build_case(1, 7, rng), build_case(29, 130, rng)]
    input_lines = []
    output_lines = []
    for lines, answers in cases:
        input_lines.extend(lines)
        output_lines.extend(answers)
        output_lines.append("")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
