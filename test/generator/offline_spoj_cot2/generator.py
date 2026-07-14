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
        if vertex == target:
            break
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


def write_case(out_dir, index, size, query_count, rng):
    values = [rng.randrange(-4, 8) for _ in range(size)]
    edges = [(rng.randrange(vertex), vertex) for vertex in range(1, size)]
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    queries = [(rng.randrange(size), rng.randrange(size)) for _ in range(query_count)]
    lines = [f"{size} {query_count}", " ".join(map(str, values))]
    lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    lines.extend(f"{left + 1} {right + 1}" for left, right in queries)
    answers = [len({values[vertex] for vertex in path_vertices(graph, left, right)})
               for left, right in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76010)
    write_case(out_dir, 0, 1, 5, rng)
    write_case(out_dir, 1, 31, 120, rng)


if __name__ == "__main__":
    main()
