#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def connected(n, edges, source, target, removed_edge=None, removed_vertex=None):
    if source == removed_vertex or target == removed_vertex:
        return False
    graph = [[] for _ in range(n)]
    for index, (left, right) in enumerate(edges):
        if index == removed_edge or left == removed_vertex or right == removed_vertex:
            continue
        graph[left].append(right)
        graph[right].append(left)
    seen = {source}
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        if vertex == target:
            return True
        for neighbor in graph[vertex]:
            if neighbor not in seen:
                seen.add(neighbor)
                queue.append(neighbor)
    return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(4010)
    n = 18
    edge_set = {(rng.randrange(vertex), vertex) for vertex in range(1, n)}
    while len(edge_set) < 34:
        left = rng.randrange(n)
        right = rng.randrange(n)
        if left != right:
            edge_set.add(tuple(sorted((left, right))))
    edges = sorted(edge_set)
    queries = []
    answers = []
    for index in range(180):
        first = rng.randrange(n)
        second = rng.randrange(n)
        if index % 2 == 0:
            edge_index = rng.randrange(len(edges))
            edge_left, edge_right = edges[edge_index]
            queries.append(f"1 {first + 1} {second + 1} {edge_left + 1} {edge_right + 1}")
            answer = connected(n, edges, first, second, removed_edge=edge_index)
        else:
            choices = [vertex for vertex in range(n) if vertex not in (first, second)]
            removed = rng.choice(choices)
            queries.append(f"2 {first + 1} {second + 1} {removed + 1}")
            answer = connected(n, edges, first, second, removed_vertex=removed)
        answers.append("da" if answer else "ne")
    lines = [f"{n} {len(edges)}"]
    lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
    lines.extend((str(len(queries)), *queries))
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
