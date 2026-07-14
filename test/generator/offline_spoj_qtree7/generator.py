#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def graph_of(n, edges):
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)
    return graph


def component_max(graph, colors, values, root):
    wanted = colors[root]
    answer = values[root]
    seen = {root}
    stack = [root]
    while stack:
        v = stack.pop()
        answer = max(answer, values[v])
        for to in graph[v]:
            if to not in seen and colors[to] == wanted:
                seen.add(to)
                stack.append(to)
    return answer


def write_case(out_dir, index, edges, colors, initial, operations):
    n = len(initial)
    graph = graph_of(n, edges)
    initial_colors = colors[:]
    colors = colors[:]
    values = initial[:]
    output = []
    for operation in operations:
        if operation[0] == 0:
            output.append(component_max(graph, colors, values, operation[1]))
        elif operation[0] == 1:
            colors[operation[1]] = not colors[operation[1]]
        else:
            values[operation[1]] = operation[2]
    lines = [str(n)] + [f"{u + 1} {v + 1}" for u, v in edges]
    lines += [" ".join("1" if color else "0" for color in initial_colors)]
    lines += [" ".join(map(str, initial)), str(len(operations))]
    for operation in operations:
        if operation[0] == 2:
            lines.append(f"2 {operation[1] + 1} {operation[2]}")
        else:
            lines.append(f"{operation[0]} {operation[1] + 1}")
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, output)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([], [False], [-7], [(0, 0), (2, 0, 9), (0, 0), (1, 0), (0, 0)])]
    rng = random.Random(730007)
    for n in range(2, 16):
        edges = [(v, rng.randrange(v)) for v in range(1, n)]
        colors = [bool(rng.randrange(2)) for _ in range(n)]
        values = [rng.randint(-30, 30) for _ in range(n)]
        operations = []
        for _ in range(60):
            operation = rng.randrange(3)
            vertex = rng.randrange(n)
            if operation == 2:
                operations.append((2, vertex, rng.randint(-30, 30)))
            else:
                operations.append((operation, vertex))
        cases.append((edges, colors, values, operations))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
