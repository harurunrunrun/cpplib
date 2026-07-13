#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


def path(start: int, goal: int, graph: list[set[int]]) -> list[int]:
    parent = [-1] * len(graph)
    parent[start] = start
    queue = deque([start])
    while queue:
        vertex = queue.popleft()
        if vertex == goal:
            break
        for next_vertex in graph[vertex]:
            if parent[next_vertex] == -1:
                parent[next_vertex] = vertex
                queue.append(next_vertex)
    if parent[goal] == -1:
        return []
    result = []
    vertex = goal
    while True:
        result.append(vertex)
        if vertex == start:
            break
        vertex = parent[vertex]
    result.reverse()
    return result


def write_case(out_dir: Path, index: int, n: int, initial_edges: list[tuple[int, int]],
               commands: list[str], output: list[str]) -> None:
    lines = [f"{n} {len(initial_edges)} {len(commands)}"]
    lines.extend(f"{left} {right}" for left, right in initial_edges)
    lines.extend(commands)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def random_case() -> tuple[int, list[tuple[int, int]], list[str], list[str]]:
    rng = random.Random(2026071333)
    n = 70
    graph = [set() for _ in range(n)]
    initial_edges: list[tuple[int, int]] = []
    for vertex in range(1, 45):
        parent = rng.randrange(vertex)
        graph[vertex].add(parent)
        graph[parent].add(vertex)
        initial_edges.append((vertex, parent))
    commands: list[str] = []
    output: list[str] = []

    for _ in range(4500):
        operation = rng.randrange(10)
        if operation < 2:
            candidates = [(left, right) for left in range(n) for right in range(left + 1, n)
                          if not path(left, right, graph)]
            if candidates:
                left, right = rng.choice(candidates)
                graph[left].add(right)
                graph[right].add(left)
                commands.append(f"LINK {left} {right}")
                output.append("1")
        elif operation < 4:
            edges = [(left, right) for left in range(n) for right in graph[left]
                     if left < right]
            if edges:
                left, right = rng.choice(edges)
                graph[left].remove(right)
                graph[right].remove(left)
                commands.append(f"CUT {left} {right}")
                output.append("1")
                commands.extend([
                    f"PATHSIZE {left} {right}",
                    f"KTH {left} {right} 0",
                    f"JUMP {left} {right} 1",
                ])
                output.extend(["-1", "-1", "-1"])
        elif operation == 4:
            vertex = rng.randrange(n)
            commands.extend([f"EVERT {vertex}", f"ROOT {vertex}"])
            output.append(str(vertex))
        elif operation == 5:
            left, right = rng.randrange(n), rng.randrange(n)
            exists = bool(path(left, right, graph))
            commands.append(f"LINK {left} {right}")
            output.append(str(int(not exists)))
            if not exists:
                graph[left].add(right)
                graph[right].add(left)
        elif operation == 6:
            left, right = rng.randrange(n), rng.randrange(n)
            if right in graph[left]:
                right = left
            commands.append(f"CUT {left} {right}")
            output.append("0")
        else:
            start, goal = rng.randrange(n), rng.randrange(n)
            vertices = path(start, goal, graph)
            size = len(vertices) if vertices else -1
            index = rng.randrange(-1, max(2, size + 2))
            expected = vertices[index] if 0 <= index < size else -1
            commands.extend([
                f"CONNECTED {start} {goal}",
                f"PATHSIZE {start} {goal}",
                f"KTH {start} {goal} {index}",
                f"JUMP {start} {goal} {index}",
            ])
            output.extend([str(int(bool(vertices))), str(size), str(expected), str(expected)])
    return n, initial_edges, commands, output


def large_case() -> tuple[int, list[tuple[int, int]], list[str], list[str]]:
    rng = random.Random(2026071334)
    n = 10000
    edges = [(vertex - 1, vertex) for vertex in range(1, n)]
    commands: list[str] = []
    output: list[str] = []
    for _ in range(12000):
        start, goal = rng.randrange(n), rng.randrange(n)
        size = abs(goal - start) + 1
        index = rng.randrange(-1, size + 2)
        expected = start + (index if goal >= start else -index) if 0 <= index < size else -1
        commands.extend([
            f"PATHSIZE {start} {goal}",
            f"KTH {start} {goal} {index}",
            f"JUMP {start} {goal} {index}",
        ])
        output.extend([str(size), str(expected), str(expected)])
    return n, edges, commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, 0, *random_case())
    write_case(out_dir, 1, *large_case())


if __name__ == "__main__":
    main()
