#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def path(graph, source, target):
    parent = {source: -1}
    queue = collections.deque([source])
    while queue:
        vertex = queue.popleft()
        if vertex == target:
            result = []
            while vertex != -1:
                result.append(vertex)
                vertex = parent[vertex]
            result.reverse()
            return result
        for neighbor in graph[vertex]:
            if neighbor not in parent:
                parent[neighbor] = vertex
                queue.append(neighbor)
    return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(5020)
    n = 16
    values = [rng.randint(0, 30) for _ in range(n)]
    initial = values[:]
    graph = [set() for _ in range(n)]
    commands = []
    answers = []
    for index in range(180):
        operation = ("bridge", "penguins", "excursion")[index % 3]
        left = rng.randrange(n)
        if operation == "penguins":
            value = rng.randint(0, 50)
            commands.append(f"penguins {left + 1} {value}")
            values[left] = value
            continue
        right = rng.randrange(n)
        route = path(graph, left, right)
        if operation == "bridge":
            commands.append(f"bridge {left + 1} {right + 1}")
            if route is None:
                graph[left].add(right)
                graph[right].add(left)
                answers.append("yes")
            else:
                answers.append("no")
        else:
            commands.append(f"excursion {left + 1} {right + 1}")
            answers.append("impossible" if route is None else str(sum(values[vertex] for vertex in route)))
    lines = [str(n), " ".join(map(str, initial)), str(len(commands)), *commands]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
